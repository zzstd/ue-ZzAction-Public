// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionPactHook.h"

#include "Pact/ZzActionPactActor.h"
#include "Pact/ZzActionPactUserData.h"
#include "ZzActionInstance.h"
#include "MotionWarping/ZzNotifyNode_MotionWarping.h"
#include "Nodes/ZzActionLinkTrack.h"
#include "Timeline/ZzActionTimeline.h"
#include "ZzActionComponent.h"
#include "ZzActionLinkData.h"

void UZzActionPactHook::SetupPact(UZzActionInstance* InActionInstance, UZzActionPactUserData* InUserData,
								  AZzActionPactActor* InPactActor)
{
	check(InActionInstance && InUserData && InPactActor);
	
	MainActionInstance = InActionInstance;
	PactAnimationUserData = InUserData;
	PactActor = InPactActor;
	
	OnMainActionConstructed(InActionInstance);
}

void UZzActionPactHook::OnActionConstructed_Implementation(UZzActionInstance* InActionInstance)
{
	check(InActionInstance && MainActionInstance);
	
	// skip main action, only handle link action
	if (InActionInstance == MainActionInstance)
		return;
	
	auto RuntimeLinkInfoPtr = InActionInstance->GetActionData().TryGetStruct<FZzActionLinkData>();
	if (!RuntimeLinkInfoPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("ActionPact: No find FZzActionLinkData, ensure the RunAction logic of LinkNode, "
							  "@See: UZzActionNodesFunctionLibrary::PushLinkData"))
		return;
	}
	
	if (InActionInstance->GetActionAsset() != RuntimeLinkInfoPtr->LinkNode->LinkActionAsset)
	{
		checkNoEntry();
		return;
	}
	
	OnLinkActionConstructed(InActionInstance, RuntimeLinkInfoPtr->LinkNode->LinkGUID);
}

void UZzActionPactHook::OnMainActionConstructed(UZzActionInstance* InActionInstance)
{
	if (!PactActor->HasAnchor())
	{
		auto AnchorLinkGUID = PactAnimationUserData->GetAnchorLinkGUID();
		if (!AnchorLinkGUID.IsValid())
		{
			PactActor->SetAnchorFromActor(InActionInstance->GetActionComponent()->GetOwner());
		}
	}
	
	AddPactMotionWarpingLinker(InActionInstance, FGuid());
}

void UZzActionPactHook::OnLinkActionConstructed(UZzActionInstance* InActionInstance, FGuid LinkGUID)
{
	check(LinkGUID.IsValid());
	
	if (!PactActor->HasAnchor())
	{
		auto AnchorLinkGUID = PactAnimationUserData->GetAnchorLinkGUID();
		if (AnchorLinkGUID == LinkGUID)
		{
			/*
			 * @FIXME: If MotionWarp in MianAction is triggered before LinkAction (such as MotionWarp in the first frame), 
			 * it will result in the Target not being set
			 */
			PactActor->SetAnchorFromActor(InActionInstance->GetActionComponent()->GetOwner());
		}
	}
	
	AddPactMotionWarpingLinker(InActionInstance, LinkGUID);
}

void UZzActionPactHook::AddPactMotionWarpingLinker(UZzActionInstance* InActionInstance, FGuid LinkGUID)
{
	// @TODO: Add MotionWarping forwarding for every action instance.
	if (auto MWNode = InActionInstance->GetTimeline()->FindNode<UZzNotifyNode_MotionWarping>())
	{
		auto PactExtender = MWNode->AddExtender<UZzActionPactMotionWarpingLinker>();
		PactExtender->LinkGUID = LinkGUID;
	}
}

void UZzActionPactMotionWarpingLinker::OnBeginConst_Implementation(const FZzActionNodeContext& Context) const
{
	if (Context.IsPreview())
		return;
	
	if (auto MotionWarpingNode = GetOwnerNode<UZzNotifyNode_MotionWarping>())
	{
		if (auto PactActor = Context.GetActionData().GetObject<AZzActionPactActor>(AZzActionPactActor::ActionPactActorName))
		{
			PactActor->AddLink(LinkGUID, MotionWarpingNode);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ActionPact: No find PactActor, ensure the RunAction logic of LinkNode, "
								  "@See: UZzActionNodesFunctionLibrary::PushLinkData"))
		}
	}
}

void UZzActionPactMotionWarpingLinker::OnEndConst_Implementation(const FZzActionNodeContext& Context, bool Abort) const
{
	if (Context.IsPreview())
		return;
	
	if (auto MotionWarpingNode = GetOwnerNode<UZzNotifyNode_MotionWarping>())
	{
		if (auto PactActor = Context.GetActionData().GetObject<AZzActionPactActor>(AZzActionPactActor::ActionPactActorName))
		{
			PactActor->RemoveLink(LinkGUID);
		}
	}
}
