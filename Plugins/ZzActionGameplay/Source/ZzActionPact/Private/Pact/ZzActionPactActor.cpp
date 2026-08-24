// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionPactActor.h"

#include "MotionWarpingComponent.h"
#include "ZzActionBlueprint.h"
#include "ZzActionInstance.h"
#include "MotionWarping/ZzNotifyNode_MotionWarping.h"
#include "Pact/ZzActionPactUserData.h"
#include "Timeline/ZzActionTimeline.h"

TOptional<FTransform> FZzActionPactActorAnchorInfo::GetAnchorTransform(FQuat RelRot) const
{
	FTransform Result = FTransform::Identity;
	
	if (Actor)
	{
		if (Actor->HasValidRootComponent())
		{
			Result = Actor->GetRootComponent()->GetSocketTransform(SocketName);
		}
	}
	else if (Component)
	{
		Result = Component->GetSocketTransform(SocketName);
	}
	else if (WorldTransform.IsSet())
	{
		Result = WorldTransform.GetValue();
	}
	else
	{
		return TOptional<FTransform>();
	}
	
	if (AnchorDirection.IsSet())
	{
		//FQuat RelRot = PactData->AnchorRelativeTransform.GetRotation();
		FQuat OverRot = AnchorDirection.GetValue().Quaternion();
		Result.SetRotation(OverRot * RelRot);
	}
	
	return Result;
}

FName AZzActionPactActor::ActionPactActorName("ActionPactActor");

AZzActionPactActor::AZzActionPactActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZzActionPactActor::AddLink(FGuid LinkGUID, UZzNotifyNode_MotionWarping* MotionWarpingNode)
{
	if (!MotionWarpingNode)
	{
		return;
	}
	
	if (!HasAnchor())
	{
		// We tolerate waiting for ready without PactAnchor.
		auto& Info = PendingLinkWarpInfos.AddDefaulted_GetRef();
		Info.LinkGUID = LinkGUID;
		Info.MotionWarpingNode = MotionWarpingNode;
		return;
	}
	
	UMotionWarpingComponent* MWComp = nullptr;
	TOptional<FName> WarpTargetName;
	{
		if (auto Actor = MotionWarpingNode->GetNodeContext().GetOwnedActor())
		{
			MWComp = Actor->GetComponentByClass<UMotionWarpingComponent>();
		}
		if (auto WarpModifier = Cast<URootMotionModifier_Warp>(MotionWarpingNode->RootMotionModifier))
		{
			WarpTargetName = WarpModifier->WarpTargetName;
		}
	}
	
	if (!MWComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("AZzActionPactActor::AddLink: NoFind MotionWarpingComponent"));
		return;
	}
	
	if (!WarpTargetName.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("AZzActionPactActor::AddLink: NoFind WarpTargetName"));
		return;
	}
	
	UpdateWarpTarget(LinkGUID, MWComp, WarpTargetName.GetValue());
}

void AZzActionPactActor::RemoveLink(FGuid LinkGUID)
{
	PendingLinkWarpInfos.RemoveAll([LinkGUID](const FZzActionPactActorLinkWarpInfo& Info)
	{
		return Info.LinkGUID == LinkGUID;
	});
}

void AZzActionPactActor::SetPactData(UZzActionPactUserData* InPactData)
{
	PactData = InPactData;
	
	auto ActionAsset = InPactData->GetTypedOuter<UZzActionBlueprint>();
	
	float PactLifetime = ActionAsset->GetTimeline()->GetLength();
	PactLifetime *= 2.0;
	
	GetWorld()->GetTimerManager().SetTimer(DestroyPactTimerHandle, this, &AZzActionPactActor::OnDestroyPactActor, PactLifetime);
}

void AZzActionPactActor::UpdateWarpTarget(FGuid LinkGUID, UMotionWarpingComponent* MWComp, FName WarpTargetName) const
{
	if (auto TargetTF = GetAnchorFinalTransform())
	{
		FVector LocationOffset;
		FRotator RotationOffset;
		if (PactData->GetLinkOffset(LinkGUID, LocationOffset, RotationOffset))
		{
			auto ToWorldTransform = TargetTF.GetValue();
			
			auto WarpTargetLoc = ToWorldTransform.TransformPosition(LocationOffset);
			auto WarpTargetRot = ToWorldTransform.TransformRotation(RotationOffset.Quaternion()).Rotator();
		
			MWComp->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, WarpTargetLoc, WarpTargetRot);
		}
	}
}

void AZzActionPactActor::OnDestroyPactActor()
{
	K2_DestroyActor();
}

TOptional<FTransform> AZzActionPactActor::GetAnchorBaseTransform() const
{
	return AnchorInfo.GetAnchorTransform(FQuat::Identity);
}

TOptional<FTransform> AZzActionPactActor::GetAnchorFinalTransform() const
{
	const FQuat RelativeRotation = ensure(PactData) ? PactData->AnchorRelativeTransform.GetRotation() : FQuat::Identity;
	return AnchorInfo.GetAnchorTransform(RelativeRotation);
}

bool AZzActionPactActor::HasAnchor() const
{
	return GetAnchorBaseTransform().IsSet();
}

bool AZzActionPactActor::BP_GetAnchorBaseTransform(FTransform& OutTransform) const
{
	auto Result = GetAnchorBaseTransform();
	OutTransform = Result.Get(FTransform::Identity);
	return Result.IsSet();
}

bool AZzActionPactActor::BP_GetAnchorFinalTransform(FTransform& OutTransform) const
{
	auto Result = GetAnchorFinalTransform();
	OutTransform = Result.Get(FTransform::Identity);
	return Result.IsSet();
}

void AZzActionPactActor::OnAnchorReady()
{
	for (auto& WarpInfo : PendingLinkWarpInfos)
	{
		// @TODO: Warn when there is no Target after a certain period of time (such as 30%)
		AddLink(WarpInfo.LinkGUID, WarpInfo.MotionWarpingNode);
	}
	
	PendingLinkWarpInfos.Empty();
}

void AZzActionPactActor::SetAnchorFromActor(AActor* InActor, FName Socket)
{
	if (ensure(!HasAnchor()))
	{
		AnchorInfo.Actor = InActor;
		AnchorInfo.SocketName = Socket;
		OnAnchorReady();
	}
}

void AZzActionPactActor::SetAnchorFromComponent(USceneComponent* InComp, FName Socket)
{
	if (ensure(!HasAnchor()))
	{
		AnchorInfo.Component = InComp;
		AnchorInfo.SocketName = Socket;
		OnAnchorReady();
	}
}

void AZzActionPactActor::SetAnchorFromLocationAndRotation(FVector Loc, FRotator Rot)
{
	if (ensure(!HasAnchor()))
	{
		AnchorInfo.WorldTransform = FTransform(Rot, Loc);
		OnAnchorReady();
	}
}

void AZzActionPactActor::SetAnchorDirection(FRotator Rot)
{
	// @TODO: Warn when Direction is set but the action has no MotionWarp node.
	AnchorInfo.AnchorDirection = Rot;
}