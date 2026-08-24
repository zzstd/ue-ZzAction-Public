// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BakedTransformsData.generated.h"

USTRUCT(BlueprintType)
struct ZZACTIONNODES_API FZzBakedTransformsData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bake", meta=(ClampMin=10))
	int32 BakeFPS = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bake")
	FName Socket = NAME_None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Bake", meta=(NoResetToDefault))
	FVector2D BakedTimeRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Bake", meta=(NoResetToDefault))
	TArray<FTransform> BakedTransformsWithRootMotion;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Bake", meta=(NoResetToDefault))
	TArray<FTransform> BakedRootTransforms;

	void Bake(class UAnimMontage* InAnimMontage, double InBeginTime, double InEndTime);
	void Bake(const class UZzActionTimeline* InTimeline, double InBeginTime, double InEndTime);

	bool HasBakedTransforms() const;
	
	// InCurrentTime is the current montage time, not time relative to the notify interval.
	FTransform GetBakedTransform(float InCurrentTime, float InSubstepTime) const;
	
	void DrawDebug(const class UWorld* World) const;
};