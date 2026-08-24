// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Niagara/ZzActionNiagaraDataInterfaceProvider.h"
#include "UObject/Object.h"
#include "ZzActionNiagaraDataInterfaceProvider_Spline.generated.h"

UCLASS(Hidden)
class ZZACTIONNODES_API UZzActionNiagaraDataInterfaceProvider_Spline : public UZzActionNiagaraDataInterfaceProvider
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "ZzAction", meta=(GetOptions="GetSplineUserDataOptions"))
	FName SplineName;
	
	UFUNCTION(CallInEditor, Category = "ZzAction")
	void AddSplineUserData() const;
	
	virtual void CopyTo(UNiagaraComponent* Comp, UNiagaraDataInterface* InSource, UNiagaraDataInterface* InDes) override;
	virtual bool ShouldHiddenDIProperty(FName PropName) const override;
private:
	
	UFUNCTION()
	TArray<FName> GetSplineUserDataOptions() const;
};