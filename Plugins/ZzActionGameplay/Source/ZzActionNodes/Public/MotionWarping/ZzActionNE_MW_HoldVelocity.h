// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzNotifyNode_MotionWarping.h"
#include "ZzActionNE_MW_HoldVelocity.generated.h"

class UZzActionNE_MW_TransformBase;

UCLASS()
class ZZACTIONNODES_API UZzActionNE_MW_HoldVelocity : public UZzActionNodeExtender_MotionWarping
{
	GENERATED_BODY()
public:
	virtual void OnWarpBegin(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const override;
	virtual void OnWarpUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const override;
	virtual void OnWarpEnd(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const override;

	virtual void OnTick_Implementation(float DeltaTime) override;
	virtual void OnEnd_Implementation(bool Abort) override;
	
	// if > 0 to enable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Velocity")
	float DesiredSpeed = 0.f;

	/*
	 * By default, ignore Z when the warp endpoints are on the same horizontal plane. 
	 * Closed this for motion that requires vertical displacement.
	 * 
	 * For example, a jumping skill that is parabolic in shape does not need to take into account the Z-axis, 
	 * but if it is a linear displacement that includes the Z-axis, then the Z-axis needs to be considered
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Velocity")
	bool bIgnoreWarpTargetZAxis = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Velocity")
	FVector2D ClampPlayRate = {0.5, 1.5f};
	
	UFUNCTION(CallInEditor)
	void UpdateDesiredSpeed();

private:
	float ComputePlayRate(const FAnimMontageInstance* MontageInst, const FVector& WarpTarget) const;
	
	FAnimMontageInstance* GetActionMontageInstance() const;

	void UpdateState(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier) const;
	void ClearState() const;
};
