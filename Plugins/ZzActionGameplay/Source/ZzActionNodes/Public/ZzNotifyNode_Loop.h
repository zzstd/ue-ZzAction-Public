// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzActionInstance.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_Loop.generated.h"

 
UCLASS(Abstract)
class ZZACTIONNODES_API UZzNotifyNode_LoopBase : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UZzNotifyNode_LoopBase();

	virtual void OnTick(float DeltaTime) override;
	virtual void OnCede() override;
	virtual void OnEnd(bool Abort) override;
	virtual FLinearColor GetNotifyColor_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, Category = "Loop")
	void OnBeginLoop();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Loop")
	void OnStopLoop(bool Abort);
	
#if WITH_EDITOR
	virtual void OnPreviewEditorDrawCanvas(class FCanvas& Canvas, FSceneView& View) const override;
#endif

	virtual bool ShouldLoop() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loop")
	bool bGotoToEndOnBreak = true;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Loop")
	int32 LoopCounter = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Loop")
	bool bHasBreak = false;
};


UCLASS(DisplayName="Loop (Simple)")
class ZZACTIONNODES_API UZzNotifyNode_SimpleLoop : public UZzNotifyNode_LoopBase
{
	GENERATED_BODY()
public:
	virtual bool ShouldLoop() const override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SimpleLoop")
	bool BP_ShouldLoop() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SimpleLoop")
	void StopLoop() const;

	UFUNCTION(BlueprintCallable, Category = "SimpleLoop")
	static bool StopActionLoop(UZzActionInstance* InActionInst);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleLoop")
	FName CheckLoopDataName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleLoop")
	bool bDefaultLoop = true;

	UPROPERTY(EditAnywhere, Category = "SimpleLoop|Preview")
	int32 PreviewLoopCount = 0;
};