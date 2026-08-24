// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzActionInstanceHook.h"
#include "Node/ZzActionNodeExtender.h"
#include "ZzActionPactHook.generated.h"


class UZzActionInstance;
class UZzActionLinkNode;
class UZzActionPactUserData;
class AZzActionPactActor;

UCLASS()
class ZZACTIONPACT_API UZzActionPactHook : public UZzActionInstanceHook
{
	GENERATED_BODY()
public:
	void SetupPact(UZzActionInstance* InActionInstance, UZzActionPactUserData* InUserData, AZzActionPactActor* InPactActor);
	
	virtual void OnActionConstructed_Implementation(UZzActionInstance* InActionInstance) override;
protected:
	UPROPERTY()
	TObjectPtr<UZzActionInstance> MainActionInstance;
	
	UPROPERTY()
	TObjectPtr<UZzActionPactUserData> PactAnimationUserData;
	
	UPROPERTY()
	TObjectPtr<AZzActionPactActor> PactActor;
	
	void OnMainActionConstructed(UZzActionInstance* InActionInstance);
	void OnLinkActionConstructed(UZzActionInstance* InActionInstance, FGuid LinkGUID);

	void AddPactMotionWarpingLinker(UZzActionInstance* InActionInstance, FGuid LinkGUID);
};

// Automatically link MotionWarpingNode to PactActor
UCLASS(Hidden)
class ZZACTIONPACT_API UZzActionPactMotionWarpingLinker : public UZzActionNodeExtender
{
	GENERATED_BODY()
public:
	FGuid LinkGUID;
	
	virtual void OnBeginConst_Implementation(const FZzActionNodeContext& Context) const override;
	virtual void OnEndConst_Implementation(const FZzActionNodeContext& Context, bool Abort) const override;
};