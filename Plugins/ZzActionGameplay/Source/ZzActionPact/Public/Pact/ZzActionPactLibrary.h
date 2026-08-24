// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ZzActionPactLibrary.generated.h"

class UZzActionLinkNode;
struct FZzActionData;
class AZzActionPactActor;

UCLASS()
class ZZACTIONPACT_API UZzActionPactLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Pact", meta = (WorldContext = "WorldContextObject"))
	static AZzActionPactActor* SpawnPactActorFromActor(const UObject* WorldContextObject, AActor* Actor, FName Socket = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Pact", meta = (WorldContext = "WorldContextObject"))
	static AZzActionPactActor* SpawnPactActorFromComponent(const UObject* WorldContextObject, USceneComponent* Comp, FName Socket = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Pact", meta = (WorldContext = "WorldContextObject"))
	static AZzActionPactActor* SpawnPactActorFromLocationAndRotation(const UObject* WorldContextObject, FVector Loc, FRotator Rot);
	
	UFUNCTION(BlueprintCallable, Category = "Pact", meta = (WorldContext = "WorldContextObject"))
	static AZzActionPactActor* SpawnPactActorWithDirection(const UObject* WorldContextObject, FRotator OverrideRotation);
	
	// Fill in ActionData for Pact when running LinkAction in LinkNode
	UFUNCTION(BlueprintCallable, Category="Pact")
	static void PushPactData(UPARAM(Ref) FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance);
};
