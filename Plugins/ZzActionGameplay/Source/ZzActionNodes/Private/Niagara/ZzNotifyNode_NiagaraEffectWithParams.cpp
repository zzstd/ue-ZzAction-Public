// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Niagara/ZzNotifyNode_NiagaraEffectWithParams.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraTypes.h"
#include "Niagara/ZzActionNiagaraParam.h"

void UZzNotifyNode_NiagaraEffectWithParams::UpdateParams()
{
	UserParameters.Empty();
	
	if (!Template)
		return;
	
	TArray<FNiagaraVariable> OutParameters;
	Template->GetExposedParameters().GetUserParameters(OutParameters);
	
	for (auto Var : OutParameters)
	{
		auto& MyVar = UserParameters.AddDefaulted_GetRef();
		MyVar.CopyFrom(Template, Var, this);
	}
}

UNiagaraComponent* UZzNotifyNode_NiagaraEffectWithParams::SpawnEffect(USkeletalMeshComponent* MeshComp) const
{
	auto Comp = Super::SpawnEffect(MeshComp);
	if (Comp)
	{
		for (auto& MyVar : UserParameters)
		{
			MyVar.CopyTo(Comp);
		}
	
		Comp->GetSystemInstanceController().Reset();
	}
	return Comp;
}
