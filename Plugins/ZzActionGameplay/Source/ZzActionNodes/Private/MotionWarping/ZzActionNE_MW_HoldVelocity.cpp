// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "MotionWarping/ZzActionNE_MW_HoldVelocity.h"

#include "ZzActionInstance.h"
#include "Timeline/ZzActionTimeline.h"
#include "Node/ZzActionNode.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"
#include "Nodes/Anim/ZzActionAnimRuntime.h"
#include "MotionWarping/ZzActionNE_MW_Transform.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"

void UZzActionNE_MW_HoldVelocity::OnWarpBegin(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const
{
	if (auto Inst = GetActionMontageInstance())
	{
		if (Inst->bEnableAutoBlendOut)
		{
			// Disable automatic blend-out to prevent an unexpectedly early blend-out at high montage play rates.
			GetNodeContext().GetActionInstance()->ActionData.SetFloatAsBool("ZzActionNE_MW_HoldVelocity_GuardEnableAutoBlendOut", Inst->bEnableAutoBlendOut);
			Inst->bEnableAutoBlendOut = false;
		}
	}
}

void UZzActionNE_MW_HoldVelocity::OnWarpUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const
{
	UpdateState(MotionWarpingComp, Modifier);
}

void UZzActionNE_MW_HoldVelocity::OnWarpEnd(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const
{
	UpdateState(MotionWarpingComp, Modifier);
	ClearState();
}

void UZzActionNE_MW_HoldVelocity::UpdateState(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const
{
	if (GetNodeContext().IsPreview() || !GetNodeContext().IsValid())
	{
		return;
	}
	
	if (DesiredSpeed <= 0.f)
	{
		return;
	}

	bool bHasWarpTarget = false;
	FVector WarpTargetLocation;
	if (auto WarpModifier = Cast<URootMotionModifier_Warp>(Modifier))
	{
		if (auto WarpTarget = MotionWarpingComp->FindWarpTarget(WarpModifier->WarpTargetName))
		{
			WarpTargetLocation = WarpTarget->GetLocation();
			bHasWarpTarget = true;
		}
	}
	if (!bHasWarpTarget)
	{
		return;
	}

	if (FAnimMontageInstance* MontageInst = GetActionMontageInstance())
	{
		const float NewPlayRate = ComputePlayRate(MontageInst, WarpTargetLocation);
		MontageInst->SetPlayRate(NewPlayRate);
		
		GetNodeContext().GetAnimRuntime()->SetAnimSyncMode(EZzActionAnimSyncMode::NoSync);
		GetNodeContext().GetActionInstance()->SetInternalPlayRate(NewPlayRate * 0.75f);
	}
}

void UZzActionNE_MW_HoldVelocity::OnTick_Implementation(float DeltaTime)
{
	if (FAnimMontageInstance* MontageInst = GetActionMontageInstance())
	{
		const auto ActionDeltaTime = MontageInst->GetPosition() - GetNodeContext().GetCurrentTime();
		if (ActionDeltaTime > 0.f)
		{
			//manual sync montage to action.
			GetNodeContext().GetActionInstance()->SetTime(MontageInst->GetPosition(), false);
		}
	}
}

void UZzActionNE_MW_HoldVelocity::OnEnd_Implementation(bool Abort)
{
	ClearState();
}

float UZzActionNE_MW_HoldVelocity::ComputePlayRate(const FAnimMontageInstance* MontageInst, const FVector& WarpTarget) const
{
	const float WarpEndPos = GetNodeContext().GetNode()->GetEndTime();
	const float CurPos = MontageInst->GetPosition();
	const float RemainingPos = WarpEndPos - CurPos;
	
	if (RemainingPos <= UE_KINDA_SMALL_NUMBER)
	{
		return 1.f;
	}

	float RemainingWarpDist = bIgnoreWarpTargetZAxis
		? (WarpTarget - GetNodeContext().GetOwnedActor()->GetActorLocation()).Size2D()
		: (WarpTarget - GetNodeContext().GetOwnedActor()->GetActorLocation()).Size();
	RemainingWarpDist = FMath::Max(RemainingWarpDist, 1.f);
	
	float DesiredPlayRate = RemainingPos / (RemainingWarpDist / DesiredSpeed);
	const float PredictedNextAnimPos = (MontageInst->GetDeltaMoved() / MontageInst->GetPlayRate()) * DesiredPlayRate;
	
	// ensure we reach exactly at the end time
	if (PredictedNextAnimPos > WarpEndPos)
	{
		DesiredPlayRate *= FMath::GetMappedRangeValueClamped(FVector2D(CurPos, PredictedNextAnimPos), FVector2D(0.f, 1.f), WarpEndPos);
		DesiredPlayRate *= 0.95f; // slow down a bit earlier. The value of 0.95 performs well in testing
	}

	return FMath::Clamp(DesiredPlayRate, ClampPlayRate.X, ClampPlayRate.Y);
}

FAnimMontageInstance* UZzActionNE_MW_HoldVelocity::GetActionMontageInstance() const
{
	return GetNodeContext().GetAnimRuntime() ? GetNodeContext().GetAnimRuntime()->GetMontageInstance() : nullptr;
}

void UZzActionNE_MW_HoldVelocity::ClearState() const
{
	if (GetNodeContext().IsValid())
	{
		if (auto Inst = GetActionMontageInstance())
		{
			Inst->bEnableAutoBlendOut = GetNodeContext().GetActionData().GetFloatAsBool("ZzActionNE_MW_HoldVelocity_GuardEnableAutoBlendOut", true);
			Inst->SetPlayRate(1.f);
		}
		if (GetNodeContext().GetAnimRuntime())
		{
			GetNodeContext().GetAnimRuntime()->SetAnimSyncMode(EZzActionAnimSyncMode::ActionToMontage);
		}
		GetNodeContext().GetActionInstance()->SetInternalPlayRate(1);
	}
}

void UZzActionNE_MW_HoldVelocity::UpdateDesiredSpeed()
{
#if WITH_EDITOR
	Modify();
	
	auto OuterTimeline = GetTypedOuter<UZzActionTimeline>();
	auto OuterNode = GetTypedOuter<UZzActionNode>();
	if (OuterTimeline && OuterNode)
	{
		auto StartLoc = ZZ::Action::ExtractRootMotionWithCharacter(OuterTimeline, 0, OuterNode->GetBeginTime()).GetLocation();
		auto EndLoc = ZZ::Action::ExtractRootMotionWithCharacter(OuterTimeline, 0, OuterNode->GetEndTime()).GetLocation();
		const float Dur = OuterNode->GetEndTime() - OuterNode->GetBeginTime();
		DesiredSpeed = (StartLoc - EndLoc).Size() / Dur;
	}
	else
	{
		DesiredSpeed = 0.f;
	}
	
	FPropertyChangedEvent Event(ThisClass::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ThisClass, DesiredSpeed)));
	PostEditChangeProperty(Event);
#endif
}

/*#if WITH_EDITOR
void UZzActionNE_MW_HoldVelocity::OnPreviewEditorTick(const FZzActionNodePreviewContext& PreviewContext) const
{
	Super::OnPreviewEditorTick(PreviewContext);
	
	auto StartLoc = UZzActionEditorBlueprintFunctionLibrary::GetPreviewWorldTransformByTime(this, GetNodeContext().GetNode()->GetBeginTime()).GetLocation();
	auto EndLoc = UZzActionEditorBlueprintFunctionLibrary::GetPreviewWorldTransformByTime(this, GetNodeContext().GetNode()->GetEndTime()).GetLocation();

	const float Dur = GetNodeContext().GetNode()->GetEndTime() - GetNodeContext().GetNode()->GetBeginTime();
	
	auto AnimSpeed = (StartLoc - EndLoc).Size() / Dur;
	GEngine->AddOnScreenDebugMessage((uint64)this, 0, FColor::Orange, FString::Printf(TEXT("AnimSpeed: %.2f"), AnimSpeed));
}
#endif*/