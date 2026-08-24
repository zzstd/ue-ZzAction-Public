// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionBlueprint.h"
#include "ZzActionComponent.h"
#include "ZzActionInstance.h"
#include "Pact/ZzActionPactHook.h"
#include "Pact/ZzActionPactActor.h"
#include "Pact/ZzActionPactUserData.h"


void UZzActionPactUserData::OnActionConstructed(UZzActionInstance* InActionInstance)
{
	// AZzActionPactActor may be created manually.
	AZzActionPactActor* PactActor = InActionInstance->ActionData.GetObject<AZzActionPactActor>(AZzActionPactActor::ActionPactActorName);
	if (!PactActor)
	{
		PactActor = InActionInstance->GetWorld()->SpawnActor<AZzActionPactActor>();
		InActionInstance->ActionData.Objects.Add(AZzActionPactActor::ActionPactActorName, PactActor);
	}
	check(PactActor);
	PactActor->SetPactData(this);
	
	{
		// Register the hook so action instances created by link nodes are collected as part of the pact.
		UZzActionPactHook* PackHook = NewObject<UZzActionPactHook>();
		PackHook->SetupPact(InActionInstance, this, PactActor);
		InActionInstance->ActionData.InstanceHooks.Add(PackHook);
	}
	
	BindPactAnchorFromActionData(PactActor, InActionInstance);
	BindPactSimpleAnchorDirection(PactActor, InActionInstance->GetActionComponent()->GetOwner());
}

void UZzActionPactUserData::BindPactAnchorFromActionData(AZzActionPactActor* PactActor, UZzActionInstance* InActionInstance) const
{
	if (BindPactAnchorName.IsNone())
		return;
	
	if (PactActor->HasAnchor())
	{
		UE_LOG(LogTemp, Warning, TEXT("BindPactAnchorFromActionData Attempt to bind to an existing anchor, Asset:%s  BindPactAnchorName:%s"), *InActionInstance->GetActionAsset()->GetName(), *BindPactAnchorName.ToString());
		return;
	}
	
	FTransform FindTransform;
	if (auto Actor = InActionInstance->ActionData.GetObject<AActor>(BindPactAnchorName))
	{
		PactActor->SetAnchorFromActor(Actor);
	}
	else if (auto Comp = InActionInstance->ActionData.GetObject<USceneComponent>(BindPactAnchorName))
	{
		PactActor->SetAnchorFromComponent(Comp);
	}
	else if (InActionInstance->ActionData.TryFindTransform(BindPactAnchorName, FindTransform))
	{
		PactActor->SetAnchorFromLocationAndRotation(FindTransform.GetLocation(), FindTransform.GetRotation().Rotator());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BindPactAnchorFromActionData No find BindPactAnchorName:%s, Asset:%s"), *BindPactAnchorName.ToString(), *InActionInstance->GetActionAsset()->GetName());
	}
}

void UZzActionPactUserData::BindPactSimpleAnchorDirection(AZzActionPactActor* PactActor, AActor* OwnerActor) const
{
	if (!OwnerActor)
		return;
	
	if (!PactActor->HasAnchor())
		return;
	
	if (PactActor->AnchorInfo.AnchorDirection.IsSet())
		return;

	TOptional<FRotator> Rot;
	if (SimpleAnchorDirectionMode == EZzActionPactSimpleDirectionMode::UseAnchor)
	{
		// Pass
	}
	else if (SimpleAnchorDirectionMode == EZzActionPactSimpleDirectionMode::UseOwner)
	{
		Rot = OwnerActor->GetActorRotation();
	}
	else if (SimpleAnchorDirectionMode == EZzActionPactSimpleDirectionMode::FaceToAnchor)
	{
		if (!IsOwnerAnchor())
		{
			if (auto AnchorTF = PactActor->GetAnchorBaseTransform())
			{
				Rot = (AnchorTF.GetValue().GetLocation() - OwnerActor->GetActorLocation()).GetSafeNormal2D().Rotation();
			}
		} // else Pass
	}
	
	if (Rot.IsSet())
	{
		PactActor->SetAnchorDirection(*Rot);
	}
}