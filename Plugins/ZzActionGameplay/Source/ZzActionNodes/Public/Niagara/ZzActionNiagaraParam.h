// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZzActionNiagaraParam.generated.h"

class UNiagaraDataInterface;
class UNiagaraComponent;

class UZzActionNiagaraDataInterfaceProvider;

USTRUCT(BlueprintType)
struct ZZACTIONNODES_API FZzNiagaraParam
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	FName Name;
	
	enum class EType : uint8
	{
		None = 0,
		Obj,
		DataInterface,
		Float,
		Bool,
		Int,
		Vec2,
		Vec3,
		Color,
	};

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	int32 Type;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<UObject> Object;
	
	UPROPERTY(EditAnywhere, Category = "Niagara", Instanced)
	TObjectPtr<UNiagaraDataInterface> DataInterface;
	
	// Save together with DataInterface
	UPROPERTY(EditAnywhere, Category = "Niagara", Instanced)
	TObjectPtr<UZzActionNiagaraDataInterfaceProvider> DataInterfaceProvider;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	float Float;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	bool Bool;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	int32 Int;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	FVector2D Vec2;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	FVector Vec3;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	FLinearColor Color;
	
	void CopyFrom(class UNiagaraSystem* Niagara, const struct FNiagaraVariable& Var, UObject* Outer);
	void CopyTo(UNiagaraComponent* Comp) const;
private:
	void CopyFromDataInterface(const UNiagaraDataInterface* Source, UObject* Outer);
	void CopyToDataInterface(UNiagaraComponent* Comp, UNiagaraDataInterface* Des) const;
};
