// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Spline/ZzActionNiagaraSplineActor.h"

#include "NiagaraComponent.h"
#include "ZzActionInstance.h"
#include "Components/SplineComponent.h"


AZzActionNiagaraSplineActor::AZzActionNiagaraSplineActor()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AZzActionNiagaraSplineActor::AddActionInst(UZzActionInstance* InActionInstance)
{
	check(InActionInstance);
	if (InActionInstance->IsEnded())
	{
		return;
	}
	
	ActivatedCounter++;
	InActionInstance->OnEndedDelegate.AddDynamic(this, &AZzActionNiagaraSplineActor::OnActionEnd);
}

void AZzActionNiagaraSplineActor::AddNiagaraInst(UNiagaraComponent* PSystem)
{
	check(PSystem);
	if (!PSystem->IsActive())
	{
		return;
	}
	
	ActivatedCounter++;
	PSystem->OnSystemFinished.AddDynamic(this, &AZzActionNiagaraSplineActor::OnSystemFinished);
}

void AZzActionNiagaraSplineActor::OnActionEnd(UZzActionInstance* InInstance)
{
	ActivatedCounter--;
	if (ActivatedCounter <= 0)
	{
		Destroy();
	}
}

void AZzActionNiagaraSplineActor::OnSystemFinished(class UNiagaraComponent* PSystem)
{
	ActivatedCounter--;
	if (ActivatedCounter <= 0)
	{
		Destroy();
	}
}
