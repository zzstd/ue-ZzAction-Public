// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzActionBlueprintUserData.h"
#include "UObject/Object.h"
#include "Components/SplineComponent.h"
#include "ZzActionSplineUserData.generated.h"

class USplineComponent;
class UZzActionBlueprint;
class AZzActionSplineActor;

UCLASS()
class ZZACTIONNODES_API UZzActionSplineUserData : public UZzActionBlueprintUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Spline")
	FTransform RelativeTransform;
	
	UPROPERTY(EditAnywhere, Category = "Spline")
	FName SplineName;
	
	UPROPERTY(VisibleAnywhere, Category = "Spline")
	TArray<FSplinePoint> Points;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	bool bClosedLoop = false;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	FVector DefaultUpVector = FVector::UpVector;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	float Duration = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	int32 ReparamStepsPerSegment = 10;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	bool bStationaryEndpoints = false;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	bool bAllowDiscontinuousSpline = false;

	void CopyFrom(const USplineComponent* Spline);
	void CopyTo(USplineComponent* Spline) const;
	
	virtual void OnActionConstructed(UZzActionInstance* InActionInstance) override;

	static FName GetSplineDataName(FName SplineName);
	
	static UZzActionSplineUserData* GetSplineUserData(const UZzActionBlueprint* Asset, FName SplineName);
// EDITOR:
	virtual void OnPreviewEnter(UWorld* InWorld, TArray<AActor*>& OutActors) override;
	virtual void OnPreviewExit(UWorld* InWorld) override;

	void HandlePreviewSplineChanged();
	
	UPROPERTY(DuplicateTransient)
	TObjectPtr<AZzActionSplineActor> PreviewSplineActor;
	
	UPROPERTY(VisibleAnywhere, Category = "Spline", DuplicateTransient)
	TObjectPtr<USplineComponent> PreviewSplineComponent;
protected:
	mutable bool bPreviewSplineModifyScoped = false;
	
	virtual AZzActionSplineActor* SpawnSplineActor(UWorld* InWorld, const FTransform& SpawnTF, bool bApplyPoint) const;
}; 
