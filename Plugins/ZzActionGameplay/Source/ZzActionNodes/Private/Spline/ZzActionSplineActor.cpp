// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Spline/ZzActionSplineActor.h"

#include "NiagaraComponent.h"
#include "ZzActionInstance.h"
#include "Components/SplineComponent.h"


AZzActionSplineActor::AZzActionSplineActor()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AZzActionSplineActor::AddActionInst(UZzActionInstance* InActionInstance)
{
	check(InActionInstance);
	if (InActionInstance->IsEnded())
	{
		return;
	}
	
	ActivatedCounter++;
	InActionInstance->OnEndedDelegate.AddDynamic(this, &AZzActionSplineActor::OnActionEnd);
}

void AZzActionSplineActor::AddNiagaraInst(UNiagaraComponent* PSystem)
{
	check(PSystem);
	if (!PSystem->IsActive())
	{
		return;
	}
	
	ActivatedCounter++;
	PSystem->OnSystemFinished.AddDynamic(this, &AZzActionSplineActor::OnSystemFinished);
}

void AZzActionSplineActor::OnActionEnd(UZzActionInstance* InInstance)
{
	ActivatedCounter--;
	if (ActivatedCounter <= 0)
	{
		Destroy();
	}
}

void AZzActionSplineActor::OnSystemFinished(class UNiagaraComponent* PSystem)
{
	ActivatedCounter--;
	if (ActivatedCounter <= 0)
	{
		Destroy();
	}
}
