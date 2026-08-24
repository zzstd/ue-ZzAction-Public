// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZzActionPactActor.generated.h"


class UZzNotifyNode_MotionWarping;

USTRUCT()
struct FZzActionPactActorLinkWarpInfo
{
	GENERATED_BODY()
	
	FGuid LinkGUID;
	
	UPROPERTY()
	TObjectPtr<UZzNotifyNode_MotionWarping> MotionWarpingNode;
};

USTRUCT()
struct FZzActionPactActorAnchorInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* Actor = nullptr;
	
	UPROPERTY()
	USceneComponent* Component = nullptr;
	
	FName SocketName;
	
	TOptional<FTransform> WorldTransform;
	
	// @see: EZzActionPactSimpleDirectionMode
	TOptional<FRotator> AnchorDirection;
	
	TOptional<FTransform> GetAnchorTransform(FQuat RelRot) const;
};

class UMotionWarpingComponent;
class UZzActionPactUserData;
class UZzActionInstance;

/*
 * How to set Anchor:
 *		1. Default: UZzActionPactUserData::BindPactAnchorName
 *		2. Advanced: Manually create PactActor using SpawnPactActor and pass in ActionData
 */
UCLASS()
class ZZACTIONPACT_API AZzActionPactActor : public AActor
{
	GENERATED_BODY()
public:
	static FName ActionPactActorName;
	
	AZzActionPactActor();
	
	virtual void AddLink(FGuid LinkGUID, UZzNotifyNode_MotionWarping* MotionWarpingNode);
	
	virtual void RemoveLink(FGuid LinkGUID);
	
	virtual void SetPactData(UZzActionPactUserData* InPactData);
protected:
	UPROPERTY(BlueprintReadOnly, Category="Pact")
	UZzActionPactUserData* PactData = nullptr;
	
	UPROPERTY()
	TArray<FZzActionPactActorLinkWarpInfo> PendingLinkWarpInfos;
	
	virtual void UpdateWarpTarget(FGuid LinkGUID, UMotionWarpingComponent* MWComp, FName WarpTargetName) const;
	
	FTimerHandle DestroyPactTimerHandle;
	
	UFUNCTION()
	void OnDestroyPactActor();
public:
	virtual TOptional<FTransform> GetAnchorBaseTransform() const;
	
	TOptional<FTransform> GetAnchorFinalTransform() const;
	
	UFUNCTION(BlueprintPure, Category="Pact")
	bool HasAnchor() const;
	
	UFUNCTION(BlueprintPure, Category="Pact", DisplayName = "GetAnchorBaseTransform")
	bool BP_GetAnchorBaseTransform(FTransform& OutTransform) const;
	
	UFUNCTION(BlueprintPure, Category="Pact", DisplayName = "GetAnchorFinalTransform")
	bool BP_GetAnchorFinalTransform(FTransform& OutTransform) const;
	
	virtual void OnAnchorReady();
	
/*********************** BEGIN Anchor Info ****************************************/
	UPROPERTY()
	FZzActionPactActorAnchorInfo AnchorInfo;
	
	UFUNCTION(BlueprintCallable, Category="Pact|Anchor")
	void SetAnchorFromActor(AActor* InActor, FName Socket = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category="Pact|Anchor")
	void SetAnchorFromComponent(USceneComponent* InComp, FName Socket = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category="Pact|Anchor")
	void SetAnchorFromLocationAndRotation(FVector Loc, FRotator Rot);
	
	UFUNCTION(BlueprintCallable, Category="Pact|Anchor")
	void SetAnchorDirection(FRotator Rot);
/*********************** END Anchor Info ****************************************/
};
