// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Node/ZzActionNodeExtender.h"
#include "ZzActionNE_MW_Transform.generated.h"


class UMotionWarpingComponent;
class URootMotionModifier_SkewWarp;

UCLASS(Abstract)
class ZZACTIONNODES_API UZzActionNE_MW_TransformBase : public UZzActionNodeExtender
{
	GENERATED_BODY()
public:
	virtual void OnPreBegin_Implementation() override;
	virtual void OnPreTick_Implementation(float DeltaTime) override;
	virtual void OnEnd_Implementation(bool Abort) override;
	
	UFUNCTION(BlueprintCallable, Category = "Warp Transform")
	void AlignToPreviewScene();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	FVector LocationOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	FRotator RotationOffset = FRotator::ZeroRotator;
	
	// if true use target facing rotation, else use owner to target direction.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	bool bUseTargetRotationSpace = false;
	
	// if target is character will use feet location, need implementation in GetBaseTransform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	bool bUseTargetFeetLocation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	bool bTickUpdateTargetTransform = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform")
	float MaxWarpDistance = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform", meta=(InlineEditConditionToggle))
	bool bEnableMinWarpDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Transform", meta=(EditCondition="bEnableMinWarpDistance"))
	float MinWarpDistance = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Preview")
	bool bShowPreviewTransform = true;
	
	UPROPERTY(EditAnywhere, Category = "Preview", meta=(EditCondition="bShowPreviewTransform"))
	bool bShowPreviewCapsule = true;
	
	virtual void UpdateWarpTarget();
	
	// return like lock target transform
	virtual bool GetBaseTransform(FTransform& OutTransform) const;

	// default use skew warp target name
	virtual bool GetWarpTargetName(FName& OutName) const;

	TOptional<FTransform> CachedWarpTargetTransform;
	
	void UpdateWarpTargetFromTransform(const FTransform& TargetTransform);
	
	void ClearWarpTarget();
	
	FSimpleMulticastDelegate UpdateWarpTargetDelegate;

#if WITH_EDITOR
	virtual void OnPreviewEditorTick(const FZzActionNodePreviewContext& PreviewContext) const override;
#endif
protected:
	FTransform LastOwnerTransform;
	float MovingDist = 0.f;
};


UCLASS()
class ZZACTIONNODES_API UZzActionNE_MW_SimpleTransform : public UZzActionNE_MW_TransformBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	bool bUseWarpTargetName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target", meta=(EditCondition="!bUseWarpTargetName"))
	FName CustomDataName;

	virtual bool GetBaseTransform(FTransform& OutTransform) const override;
};