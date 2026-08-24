// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzNotifyNode_NiagaraEffect.h"
#include "ZzActionNiagaraParam.h"
#include "ZzNotifyNode_NiagaraEffectWithParams.generated.h"



UCLASS(DisplayName="Niagara Effect With Params")
class ZZACTIONNODES_API UZzNotifyNode_NiagaraEffectWithParams : public UZzNotifyNode_NiagaraEffect
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Parameters", EditFixedSize)
	TArray<FZzNiagaraParam> UserParameters;
	
	UFUNCTION(CallInEditor, Category = "User Parameters")
	void UpdateParams();
	
	virtual UNiagaraComponent* SpawnEffect(USkeletalMeshComponent* MeshComp) const override;
};
