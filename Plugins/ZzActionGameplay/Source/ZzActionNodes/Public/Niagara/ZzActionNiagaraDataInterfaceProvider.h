// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZzActionNiagaraDataInterfaceProvider.generated.h"

class UNiagaraDataInterface;
class UNiagaraComponent;

/*
 * Supplies ZzAction-specific parameters required by Niagara data interfaces, 
 * such as the SplineActor(SoftActorPath) used by UNiagaraDataInterfaceSpline.
 * @See: UZzActionDataInterfaceProvider_Spline
 */
UCLASS(Abstract)
class ZZACTIONNODES_API UZzActionNiagaraDataInterfaceProvider : public UObject
{
	GENERATED_BODY()
public:
	
	virtual void CopyTo(UNiagaraComponent* Comp, UNiagaraDataInterface* InSource, UNiagaraDataInterface* InDes) {}
	
	// Allows Niagara data-interface parameters to be hidden in the details panel.
	virtual bool ShouldHiddenDIProperty(FName PropName) const { return false; }
};
