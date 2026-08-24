// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzNotifyNode_Trace.h"

#include "KismetTraceUtils.h"
#include "ZzActionInstance.h"

void UZzNotifyNode_BakedBase::Bake()
{
	Modify();
	
	BP_PreBake();
	BakedTransforms.Bake(GetTimelineAsset(), GetBeginTime(), GetEndTime());

#if WITH_EDITOR
	FPropertyChangedEvent Event(StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UZzNotifyNode_BakedBase, BakedTransforms)));
	PostEditChangeProperty(Event);
#endif
}

#if WITH_EDITOR
void UZzNotifyNode_BakedBase::OnPreviewEditorTick(float NewPos, bool bPlaying) const
{
	Super::OnPreviewEditorTick(NewPos, bPlaying);

	if (bDrawDebugBakedTransforms)
	{
		BakedTransforms.DrawDebug(GetWorld());
	}
}
#endif

void UZzNotifyNode_Trace::OnBegin()
{
	Super::OnBegin();
	CurrentSubstepTime.Reset();
	LastExecuteTransform.Reset();
	UpdateAndExecuteWithContext();
}

void UZzNotifyNode_Trace::OnTick(float DeltaTime)
{
	Super::OnTick(DeltaTime);
	UpdateAndExecuteWithContext();
}

void UZzNotifyNode_Trace::OnEnd(bool Abort)
{
	Super::OnEnd(Abort);
	UpdateAndExecuteWithContext();
}

void UZzNotifyNode_Trace::UpdateAndExecute(const FTransform& BaseTransform, double Time)
{
	auto SetOverrideCreateTransform = [this, BaseTransform](const FTransform& BoneLocalTF)
	{
		auto Result = FTransform(
			BaseTransform.TransformRotation(BoneLocalTF.GetRotation()),
			BaseTransform.TransformPosition(BoneLocalTF.GetLocation()));

		OnExecute(Result);
	};
	
	if (bUseBakedSubstepTick && BakedTransforms.HasBakedTransforms())
	{
		if (!CurrentSubstepTime.IsSet())
		{
			// setup current time to begin time.
			CurrentSubstepTime = BakedTransforms.BakedTimeRange.X;
		}
		
		const double TargetTime = FMath::Min(Time, BakedTransforms.BakedTimeRange.Y);
		const double SubDeltaTime = 1.0 / FMath::Max(BakedTransforms.BakeFPS, 10);
		
		while (CurrentSubstepTime.GetValue() < TargetTime)
		{
			CurrentSubstepTime = FMath::Min(CurrentSubstepTime.GetValue() + SubDeltaTime, TargetTime);
			const auto BakedBoneTF = BakedTransforms.GetBakedTransform(Time, CurrentSubstepTime.GetValue());
			SetOverrideCreateTransform(BakedBoneTF);
		}
	}
	else
	{
		if (auto Comp = GetNodeContext().TryGetSkeletonComponent())
		{
			const auto ComponentLocalTF = Comp->GetSocketTransform(BakedTransforms.Socket, RTS_Component);
			SetOverrideCreateTransform(ComponentLocalTF);
		}
	}
}

void UZzNotifyNode_Trace::UpdateAndExecuteWithContext()
{
	if (GetNodeContext().IsPreview())
	{
		return;
	}
	
	auto Comp = GetNodeContext().TryGetSkeletonComponent();
	if (!Comp)
	{
		return;
	}
	
	UpdateAndExecute(Comp->GetComponentTransform(), GetNodeContext().GetCurrentTime());
}

void UZzNotifyNode_Trace::OnPreExecute_Implementation()
{
}

void UZzNotifyNode_Trace::OnExecute(const FTransform& InTF)
{
	OnPreExecute();
	
	if (LastExecuteTransform.IsSet())
	{
		FVector StartPos = InTF.TransformPosition(StartOffset);
		FVector EndPos = InTF.TransformPosition(EndOffset);
		
		FVector LastStartPos = LastExecuteTransform.GetValue().TransformPosition(StartOffset);
		FVector LastEndPos = LastExecuteTransform.GetValue().TransformPosition(EndOffset);
		
		const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel); 
		const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(TraceRadius);
		
		static const FName SphereTraceMultiName(TEXT("ZzNotifyNode_Trace"));
		FCollisionQueryParams CQP = FCollisionQueryParams(SphereTraceMultiName, false, GetNodeContext().GetOwnedActor());
		
		const int32 SafeTraceNum = FMath::Max(TraceNum, 2);
		for (int32 j = 0; j < SafeTraceNum; j++)
		{
			float Alpha = (float)j / (float)(SafeTraceNum - 1);
			FVector TraceBegin = FMath::Lerp(LastStartPos, LastEndPos, Alpha);
			FVector TraceEnd = FMath::Lerp(StartPos, EndPos, Alpha);
			
			TArray<FHitResult> HitResults;
			bool bHit = GetWorld()->SweepMultiByChannel(HitResults, TraceBegin, TraceEnd, FQuat::Identity, CollisionChannel, CollisionShape, CQP);
#if ENABLE_DRAW_DEBUG
			if (bDrawDebugTrace)
			{
				DrawDebugSphereTraceMulti(GetWorld(), TraceBegin, TraceEnd, TraceRadius, EDrawDebugTrace::ForDuration, bHit, HitResults, FLinearColor::Red, FLinearColor::Green, 3);
			}
#endif
			OnHits(HitResults);
		}
	}

	LastExecuteTransform = InTF;
}

void UZzNotifyNode_Trace::OnHits_Implementation(const TArray<FHitResult>& InHits)
{
	if (InHits.IsEmpty())
	{
		return;
	}
	
	auto ActionInst = GetNodeContext().GetActionInstance();
	if (!ActionInst)
	{
		return;
	}
	
	FZzNotifyNode_TraceOnHitsParam Param;
	Param.Hits = InHits;
	ActionInst->ActionData.SetStruct(Param);
	
	for (auto e : Extensions)
	{
		if (e)
		{
			e->Execute();
		} 
	}
	
	ActionInst->ActionData.RemoveStruct<FZzNotifyNode_TraceOnHitsParam>();
}

TArray<FHitResult> UZzNotifyNode_Trace::GetTraceHits(const UZzActionInstance* ActionInst)
{
	if (ActionInst)
	{
		if (auto Param = ActionInst->ActionData.TryGetStruct<FZzNotifyNode_TraceOnHitsParam>())
		{
			return Param->Hits;
		}
	}
	
	return TArray<FHitResult>();
}

#if WITH_EDITOR
void UZzNotifyNode_Trace::OnPreviewEditorTick(float NewPos, bool bPlaying) const
{
	Super::OnPreviewEditorTick(NewPos, bPlaying);

	FTransform PreviewMeshTF = FTransform::Identity;
	PreviewMeshTF.SetRotation(FRotator(0, -90, 0).Quaternion());
	
	FVector LastStartPos, LastEndPos;
	for (int32 i = 0; i < BakedTransforms.BakedTransformsWithRootMotion.Num(); i++)
	{
		auto BoneTF = BakedTransforms.BakedTransformsWithRootMotion[i];
		BoneTF *= PreviewMeshTF;

		const int32 SafeTraceNum = FMath::Max(TraceNum, 2);
		
		FVector StartPos = BoneTF.TransformPosition(StartOffset);
		FVector EndPos = BoneTF.TransformPosition(EndOffset);
		if (i != 0)
		{
			for (int32 j = 0; j < SafeTraceNum; j++)
			{
				float Alpha = (float)j / (float)(SafeTraceNum - 1);
				FVector TraceA = FMath::Lerp(StartPos, EndPos, Alpha);
				FVector TraceB = FMath::Lerp(LastStartPos, LastEndPos, Alpha);

				if (TraceRadius > 0.f)
				{
					auto Delta = (TraceB - TraceA);
					DrawDebugCapsule(GetWorld(), TraceA + Delta * 0.5f, Delta.Size() * 0.5f + TraceRadius, TraceRadius, FRotationMatrix(Delta.Rotation()).GetUnitAxis(EAxis::Z).ToOrientationQuat(), FColor::Green);
				}
				else
				{
					DrawDebugLine(GetWorld(), TraceA, TraceB, FColor::Green);
				}
			}
		}
		LastStartPos = StartPos;
		LastEndPos = EndPos;
	}
}
#endif

void UZzNotifyNode_Overlap::OnTick(float DeltaTime)
{
	Super::OnTick(DeltaTime);
}

#if WITH_EDITOR
void UZzNotifyNode_Overlap::OnPreviewEditorTick(float NewPos, bool bPlaying) const
{
	Super::OnPreviewEditorTick(NewPos, bPlaying);
}
#endif
