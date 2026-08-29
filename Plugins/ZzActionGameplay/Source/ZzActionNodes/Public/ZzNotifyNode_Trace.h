// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BakedTransformsData.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_Trace.generated.h"


UCLASS(Abstract)
class ZZACTIONNODES_API UZzNotifyNode_BakedBase : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bake Transform")
	FZzBakedTransformsData BakedTransforms;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bake Transform")
	bool bDrawDebugBakedTransforms = true;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Bake Transform")
	virtual void Bake();

	UFUNCTION(BlueprintImplementableEvent, Category="Bake Transform")
	void BP_PreBake();
	
#if WITH_EDITOR
	virtual void OnPreviewEditorTick(float NewPos, bool bPlaying) const override;
#endif
};

USTRUCT(BlueprintType)
struct ZZACTIONNODES_API FZzNotifyNode_TraceOnHitsParam
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TraceOnHitsParam")
	TArray<FHitResult> Hits;
};


UCLASS()
class ZZACTIONNODES_API UZzNotifyNode_Trace : public UZzNotifyNode_BakedBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	FVector StartOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	FVector EndOffset = FVector(100, 0, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep", meta=(ClampMin=2))
	int32 TraceNum = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep", meta=(ClampMin=0.0))
	float TraceRadius = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	bool bUseBakedSubstepTick = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sweep")
	bool bDrawDebugTrace = false;
	
	virtual void OnBegin() override;
	virtual void OnTick(float DeltaTime) override;
	virtual void OnEnd(bool Abort) override;
	
	/*
	 * Advance accumulated time and interpolate baked transforms for substep execution.
	 * 
	 * BaseTransform is the mesh component's current transform and converts baked transforms into world space. 
	 * Time is the current montage time.
	 * 
	 * ExecuteFun may perform multiple trace evaluations per call when bUseBakedSubstepTick is enabled.
	 */
	void UpdateAndExecute(const FTransform& BaseTransform, double Time);
	
	void UpdateAndExecuteWithContext();

	//  Allows trace parameters, such as StartOffset, to be overridden by weapon configuration.
	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	void OnPreExecute();
	
	virtual void OnExecute(const FTransform& InTF);
	
	/*
	 * Provides the default node-extender integration. Implement Execute for the trace and return hits through OnHits
	 * 
	 * duplicate hits are not removed automatically.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Trace")
	void OnHits(const TArray<FHitResult>& InHits);
	
	UFUNCTION(BlueprintPure, Category="Trace")
	static TArray<FHitResult> GetTraceHits(const UZzActionInstance* ActionInst);
	
#if WITH_EDITOR
	virtual void OnPreviewEditorTick(float NewPos, bool bPlaying) const override;
#endif
private:
	TOptional<double> CurrentSubstepTime;
	TOptional<FTransform> LastExecuteTransform;
};

UCLASS()
class ZZACTIONNODES_API UZzNotifyNode_Overlap : public UZzNotifyNode_BakedBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlap")
	FVector LocationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlap")
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlap")
	int32 Shape;
	
	virtual void OnTick(float DeltaTime) override;
	
#if WITH_EDITOR
	virtual void OnPreviewEditorTick(float NewPos, bool bPlaying) const override;
#endif
};