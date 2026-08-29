// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzActionBlueprintUserData.h"
#include "ZzActionPactUserData.generated.h"

class UZzActionTimeline;
class UZzActionLinkNode;
class UZzActionNode;
class UZzNotifyNode_MotionWarping;
class AZzActionPactActor;

USTRUCT(BlueprintType)
struct ZZACTIONPACT_API FZzActionPactLinkInfo
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LinkInfo")
	FVector LocationOffset = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LinkInfo")
	FRotator RotationOffset = FRotator::ZeroRotator;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LinkInfo")
	bool bHasMotionWarpNode = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LinkInfo")
	FGuid LinkGUID;
	
	UZzActionLinkNode* GetLinkNode(const UZzActionTimeline* MainTimeline) const;
	
	UZzActionTimeline* GetLinkTimeline(UZzActionTimeline* MainTimeline) const;
	const UZzActionTimeline* GetLinkTimeline(const UZzActionTimeline* MainTimeline) const;
	
	// world space is main space
	FTransform GetTransformWS(const UZzActionTimeline* MainTimeline, float MainTime) const;
	
	UZzNotifyNode_MotionWarping* GetMotionWarpingNode(const UZzActionTimeline* MainTimeline) const;
};

UENUM(BlueprintType)
enum class EZzActionPactSimpleDirectionMode : uint8
{
	UseAnchor,		// Default, Pass
	UseOwner,		// use owner direction
	FaceToAnchor,	// face to anchor direction, if owner is anchor, PASS

	/*
	 * Link-to-actor ownership is project-specific.
	 * Implementing it here will incur a lot of abstract costs
	 * So do not implement here.
	 */

	// use link direction
	UseLink,
	// face to link index direction
	FaceToLink,
};

UCLASS()
class ZZACTIONPACT_API UZzActionPactUserData : public UZzActionBlueprintUserData
{
	GENERATED_BODY()
public:
	// see UpdateLinks
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, EditFixedSize, Category = "Pact")
	TArray<FZzActionPactLinkInfo> Links;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pact")
	int32 AnchorLinkIndex = 0;
	
	// see UpdateLinks
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pact") 
	FTransform AnchorRelativeTransform;
	
	UPROPERTY()
	TArray<int32> DrawDebugLinks;
	
	UFUNCTION(BlueprintPure, Category = "Pact")
	FGuid GetAnchorLinkGUID() const;
	
	UFUNCTION(BlueprintPure, Category = "Pact")
	const FZzActionPactLinkInfo& GetAnchorLinkInfo() const;
	
	UFUNCTION(BlueprintPure, Category = "Pact")
	bool IsOwnerAnchor() const;

	UFUNCTION(BlueprintCallable, Category = "Pact")
	bool GetLinkOffset(FGuid LinkGUID, FVector& Loc, FRotator& Rot) const;
protected:
	FZzActionPactLinkInfo* GetLinkInfo(FGuid LinkGUID);
public:
//////////////// BEGIN editor ////////////////
	
	// bind Links Data in editor
	void UpdateLinks();
	void UpdateLinksOffset();
	
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const override;
	virtual void PostEditChangeOwner(const FPropertyChangedEvent& PropertyChangedEvent) override;
public:
//////////////// BEGIN Binds ////////////////
	/*
	 * hook & binds
	 */
	virtual void OnActionConstructed(UZzActionInstance* InActionInstance) override;

	// Resolve and assign the pact target from ActionData by name.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binds")
	FName BindPactAnchorName;
	
	void BindPactAnchorFromActionData(AZzActionPactActor* PactActor, UZzActionInstance* InActionInstance) const;
	
	/*
	 * Skip this when AnchorDirection has already been provided externally.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binds")
	EZzActionPactSimpleDirectionMode SimpleAnchorDirectionMode = EZzActionPactSimpleDirectionMode::UseAnchor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binds", meta=(EditCondition="SimpleAnchorDirectionMode == EZzActionPactSimpleDirectionMode::UseLink || SimpleAnchorDirectionMode == EZzActionPactSimpleDirectionMode::FaceToLink"))
	int32 DirectionLinkIndex = 1;

	void BindPactSimpleAnchorDirection(AZzActionPactActor* PactActor, AActor* OwnerActor) const;
};
