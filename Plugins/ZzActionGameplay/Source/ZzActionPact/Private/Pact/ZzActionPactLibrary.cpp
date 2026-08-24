// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionPactLibrary.h"

#include "Pact/ZzActionPactActor.h"
#include "ZzActionData.h"
#include "ZzActionInstance.h"
#include "ZzActionLinkData.h"
#include "Pact/ZzActionPactHook.h"

namespace ZZ::Private
{
	AZzActionPactActor* SpawnPactActorFromContextObject(const UObject* WorldContextObject)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->SpawnActor<AZzActionPactActor>();
		}
		
		return nullptr;
	}
}

AZzActionPactActor* UZzActionPactLibrary::SpawnPactActorFromActor(const UObject* WorldContextObject, AActor* Actor, FName Socket)
{
	if (AZzActionPactActor* PactActor = ZZ::Private::SpawnPactActorFromContextObject(WorldContextObject))
	{
		PactActor->SetAnchorFromActor(Actor, Socket);
		return PactActor;
	}
	
	return nullptr;
}

AZzActionPactActor* UZzActionPactLibrary::SpawnPactActorFromComponent(const UObject* WorldContextObject, USceneComponent* Comp, FName Socket)
{
	if (AZzActionPactActor* PactActor = ZZ::Private::SpawnPactActorFromContextObject(WorldContextObject))
	{
		PactActor->SetAnchorFromComponent(Comp, Socket);
		return PactActor;
	}
	
	return nullptr;
}

AZzActionPactActor* UZzActionPactLibrary::SpawnPactActorFromLocationAndRotation(const UObject* WorldContextObject, FVector Loc, FRotator Rot)
{
	if (AZzActionPactActor* PactActor = ZZ::Private::SpawnPactActorFromContextObject(WorldContextObject))
	{
		PactActor->SetAnchorFromLocationAndRotation(Loc, Rot);
		return PactActor;
	}
	
	return nullptr;
}

AZzActionPactActor* UZzActionPactLibrary::SpawnPactActorWithDirection(const UObject* WorldContextObject, FRotator OverrideRotation)
{
	if (AZzActionPactActor* PactActor = ZZ::Private::SpawnPactActorFromContextObject(WorldContextObject))
	{
		PactActor->SetAnchorDirection(OverrideRotation);
		return PactActor;
	}
	
	return nullptr;
}

void UZzActionPactLibrary::PushPactData(FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance)
{
	FZzActionLinkData::Push(InActionDataRef, InLinkNode, InActionInstance);
	
	if (InActionInstance)
	{
		InActionInstance->GetActionData().CopyNamedValuesTo(InActionDataRef, {AZzActionPactActor::ActionPactActorName});
	}
}
