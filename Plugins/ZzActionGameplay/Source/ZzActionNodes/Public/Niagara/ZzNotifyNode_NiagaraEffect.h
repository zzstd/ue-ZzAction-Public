// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_NiagaraEffect.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(DisplayName="Niagara Effect")
class ZZACTIONNODES_API UZzNotifyNode_NiagaraEffect : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UZzNotifyNode_NiagaraEffect();

	virtual FLinearColor GetNotifyColor_Implementation() const override;
	virtual FText GetNotifyName_Implementation() const override;
	virtual void OnBegin() override;
	virtual void OnEnd(bool Abort) override;
	virtual bool IsPoint() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem", meta = (DisplayName = "Niagara System", ToolTip = "The niagara system to spawn for the notify state"))
	TObjectPtr<UNiagaraSystem> Template;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem")
	bool bTimedEffect = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem", meta = (AnimNotifyBoneName = "true", ToolTip = "The socket or bone to attach the system to"))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem", meta = (MakeEditWidget, ZzMakeEditWidgetTransform = "SocketName", ToolTip = "Offset from the socket or bone to place the Niagara system"))
	FTransform Offset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem")
	bool bAttached = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem", meta = (DisplayName = "Destroy Immediately", ToolTip = "Whether the Niagara system should be immediately destroyed at the end of the notify state or be allowed to finish"))
	bool bDestroyAtEnd = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraSystem", meta = (DisplayName = "Destroy Point Effect at Aborted", EditCondition="!bTimedEffect"))
	bool bDestroyPointEffectAtAborted = false;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "NiagaraSystem")
	bool bAbsoluteScale;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "NiagaraSystem")
	bool bEditorPreview = true;

#if WITH_EDITOR
	virtual void OnPreviewEditorTick(float NewPos, bool bPlaying) const override;
	virtual void OnPreviewEditorCleanup() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
private:
#if WITH_EDITORONLY_DATA
	/*
	 * This node is InstancedPerExecution, so we directly store the editor properties on the instance
	 */
	UPROPERTY(Transient, NonTransactional)
	mutable TObjectPtr<UNiagaraComponent> EditorPreviewFxComp;

	mutable FTransform EditorPreviewCachedSpawnOffset;
#endif
protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "NiagaraSystem")
	TObjectPtr<UNiagaraComponent> FXComponent;
	
	virtual UNiagaraComponent* SpawnEffect(USkeletalMeshComponent* MeshComp) const;
	
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "NiagaraSystem")
	FTransform GetSpawnEffectOffset() const;
	
	bool ValidateParameters(USkeletalMeshComponent* MeshComp, FString* ErrMsg = nullptr) const;
};
