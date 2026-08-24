// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "NodeExtender/ZzActionNE_RunLinkAction.h"

#include "ZzActionComponent.h"
#include "ZzActionInstance.h"
#include "ZzActionLinkData.h"
#include "ZzActionNodes.h"
#include "ZzActionNodesFunctionLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Nodes/ZzActionLinkTrack.h"

void UZzActionNE_RunLinkAction::OnBegin_Implementation()
{
	Super::OnBegin_Implementation();

	if (GetNodeContext().IsPreview())
		return;
	
	auto LinkNode = GetOwnerNode<UZzActionLinkNode>();
	if (!LinkNode)
	{
		UE_LOG(LogZzActionNodes, Warning, TEXT("[ZzActionNE_RunLinkActionBase] Owner node is not link node [%s]"), *GetNameSafe(GetOwnerNode<>()));
		return;
	}
	
	if (!LinkNode->LinkActionAsset)
	{
		UE_LOG(LogZzActionNodes, Warning, TEXT("[ZzActionNE_RunLinkActionBase] LinkActionNode is NULL, [%s]"), *GetNameSafe(GetOwnerNode<>()));
		return;
	}
	
	FZzActionData LinkActionData;
	GetLinkActionData(LinkActionData);
	RunAction(LinkNode->LinkActionAsset, LinkActionData);
	
	if (!LinkActionInstance)
	{
		UE_LOG(LogZzActionNodes, Warning, TEXT("[ZzActionNE_RunLinkActionBase] Failed to run link action asset, [%s]"), *GetNameSafe(GetOwnerNode<>()));
		return;
	}

	GetNodeContext().GetActionInstance()->SetTimeDelegate.AddDynamic(this, &UZzActionNE_RunLinkAction::OnSetTime);
}

void UZzActionNE_RunLinkAction::OnTick_Implementation(float DeltaTime)
{
	Super::OnTick_Implementation(DeltaTime);

	if (LinkActionInstance)
	{
		LinkActionInstance->SetInternalPlayRate(GetNodeContext().GetActionInstance()->GetFinalPlayRate());
		LinkActionInstance->SetPlayRate(1.f);
	}
}

UZzActionComponent* UZzActionNE_RunLinkAction::GetLinkActionComponent() const
{
	if (auto ActionComp = GetNodeContext().GetActionData().GetObject<UZzActionComponent>(TargetDataName))
	{
		return ActionComp;
	}

	if (auto Actor = GetNodeContext().GetActionData().GetObject<AActor>(TargetDataName))
	{
		return Actor->FindComponentByClass<UZzActionComponent>();
	}

	if (auto Controller = GetNodeContext().GetActionData().GetObject<AController>(TargetDataName))
	{
		if (auto Pawn = Controller->GetPawn())
		{
			return Pawn->FindComponentByClass<UZzActionComponent>();
		}

		if (Controller->PlayerState)
		{
			return Controller->PlayerState->FindComponentByClass<UZzActionComponent>();
		}
	}
	
	return nullptr;
}

void UZzActionNE_RunLinkAction::RunAction(UZzActionBlueprint* InActionAsset, const FZzActionData& InLinkActionData)
{
	auto LinkActionComp = GetLinkActionComponent();
	if (!LinkActionComp)
	{
		UE_LOG(LogZzActionNodes, Warning, TEXT("[UZzActionNE_RunLinkAction] LinkActionComponent is NULL, [%s]"), *GetNameSafe(GetOwnerNode<>()));
		return;
	}
	
	LinkActionInstance = LinkActionComp->RunActionWithData(InActionAsset, InLinkActionData, this);
}

void UZzActionNE_RunLinkAction::GetLinkActionData(FZzActionData& OutLinkActionData) const
{
	FZzActionLinkData::Push(OutLinkActionData, GetOwnerNode<UZzActionLinkNode>(), GetNodeContext().GetActionInstance());
}

void UZzActionNE_RunLinkAction::OnSetTime(UZzActionInstance* Instance, double NewTime, bool bTeleport)
{
	if (LinkActionInstance)
	{
		LinkActionInstance->SetTime(NewTime, bTeleport);
	}
}
