// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZzActionSplineActor.generated.h"

class UZzActionInstance;
class UNiagaraComponent;

UCLASS(Hidden)
class ZZACTIONNODES_API AZzActionSplineActor : public AActor
{
	GENERATED_BODY()
public:
	AZzActionSplineActor();
	
	UPROPERTY()
	class USplineComponent* Spline;

	void AddActionInst(UZzActionInstance* InActionInstance);
	
	void AddNiagaraInst(UNiagaraComponent* PSystem);
private:
	int32 ActivatedCounter = 0;
	
	UFUNCTION()
	void OnActionEnd(UZzActionInstance* InInstance);
	
	UFUNCTION()
	void OnSystemFinished(UNiagaraComponent* PSystem);
};
