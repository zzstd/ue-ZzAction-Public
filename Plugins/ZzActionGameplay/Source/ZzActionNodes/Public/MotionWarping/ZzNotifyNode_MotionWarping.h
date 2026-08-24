// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_MotionWarping.generated.h"

class UMotionWarpingComponent;
class UAnimSequenceBase;
class URootMotionModifier;

// copy from UAnimNotifyState_MotionWarping

UCLASS(DisplayName="Motion Warping")
class ZZACTIONNODES_API UZzNotifyNode_MotionWarping : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UZzNotifyNode_MotionWarping(const FObjectInitializer& ObjectInitializer);

	virtual void OnInit() override;
	
	virtual FText GetNotifyName_Implementation() const override;
	virtual FLinearColor GetNotifyColor_Implementation() const override;
	
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Config")
	TObjectPtr<URootMotionModifier> RootMotionModifier;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Motion Warping")
	TObjectPtr<URootMotionModifier> ModifierInstance;

	void OnBecomeRelevant(UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float InStartTime, float InEndTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Motion Warping")
	URootMotionModifier* AddRootMotionModifier(UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float InStartTime, float InEndTime) const;

	UFUNCTION()
	void OnRootMotionModifierActivate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier);

	UFUNCTION()
	void OnRootMotionModifierUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier);

	UFUNCTION()
	void OnRootMotionModifierDeactivate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Motion Warping")
	void OnWarpBegin(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Motion Warping")
	void OnWarpUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Motion Warping")
	void OnWarpEnd(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const;

#if WITH_EDITOR
	virtual void OnPreviewEditorDraw(FPrimitiveDrawInterface* PDI) const override;
	virtual void OnPreviewEditorDrawCanvas(class FCanvas& Canvas, FSceneView& View) const override;
#endif
};

/*
 * OnWarp is called from UCharacterMovementComponent::PerformMovement, 
 * so it does not share ZzAction's update order.
 */
UCLASS()
class ZZACTIONNODES_API UZzActionNodeExtender_MotionWarping : public UZzActionNodeExtender
{
	GENERATED_BODY()
public:
	virtual void OnWarpBegin(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const {}
	virtual void OnWarpUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const {}
	virtual void OnWarpEnd(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const {}
};
