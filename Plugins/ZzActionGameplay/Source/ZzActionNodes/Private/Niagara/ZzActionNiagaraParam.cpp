// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Niagara/ZzActionNiagaraParam.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterface.h"
#include "NiagaraDataInterfaceSpline.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraTypes.h"
#include "Niagara/ZzActionNiagaraDataInterfaceProvider.h"
#include "Spline/ZzActionNiagaraDataInterfaceProvider_Spline.h"
#include "Spline/ZzActionSplineUserData.h"

void FZzNiagaraParam::CopyFrom(UNiagaraSystem* Niagara, const FNiagaraVariable& Var, UObject* Outer)
{
	Name = Var.GetName();
	
	if (Var.IsDataInterface())
	{
		Type = static_cast<int32>(EType::DataInterface);
		UNiagaraDataInterface* DI = Niagara->GetExposedParameters().GetDataInterface(Var);
		CopyFromDataInterface(DI, Outer);
	}
	else if (Var.IsUObject())
	{
		Type = static_cast<int32>(EType::Obj);
		// Store a direct reference to the asset.
		Object = Niagara->GetExposedParameters().GetUObject(Var);
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetFloatDef())
	{
		Type = static_cast<int32>(EType::Float);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			FMemory::Memcpy(&Float, ActualData, sizeof(float));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetBoolDef())
	{
		Type = static_cast<int32>(EType::Bool);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			FMemory::Memcpy(&Bool, ActualData, sizeof(bool));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetIntDef())
	{
		Type = static_cast<int32>(EType::Int);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			FMemory::Memcpy(&Int, ActualData, sizeof(int32));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetVec2Def())
	{
		Type = static_cast<int32>(EType::Vec2);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			Vec2 = FVector2D(*static_cast<const FVector2f*>(ActualData));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetVec3Def())
	{
		Type = static_cast<int32>(EType::Vec3);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			Vec3 = FVector(*static_cast<const FVector3f*>(ActualData));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetColorDef())
	{
		Type = static_cast<int32>(EType::Color);
		if (const void* ActualData = Niagara->GetExposedParameters().GetParameterData(Var))
		{
			FMemory::Memcpy(&Color, ActualData, sizeof(FLinearColor));
		}
	}
	else if (Var.GetType() == FNiagaraTypeDefinition::GetPositionDef())
	{
		Type = static_cast<int32>(EType::Vec3);
		if (const FVector* ActualData = Niagara->GetExposedParameters().GetPositionParameterValue(Var.GetName()))
		{
			Vec3 = *ActualData;
		}
	}
	else
	{
		Type = static_cast<int32>(EType::None);
	}
}

void FZzNiagaraParam::CopyTo(UNiagaraComponent* Comp) const
{
	switch (static_cast<EType>(Type)) 
	{
	case EType::Obj:
		Comp->SetVariableObject(Name, Object);
		break;
	case EType::DataInterface:
		CopyToDataInterface(Comp, UNiagaraFunctionLibrary::GetDataInterface(UNiagaraDataInterface::StaticClass(), Comp, Name));
		break;
	case EType::Float:
		Comp->SetVariableFloat(Name, Float);
		break;
	case EType::Bool:
		Comp->SetVariableBool(Name, Bool);
		break;
	case EType::Int:
		Comp->SetVariableInt(Name, Int);
		break;
	case EType::Vec2:
		Comp->SetVariableVec2(Name, Vec2);
		break;
	case EType::Vec3:
		Comp->SetVariableVec3(Name, Vec3);
		break;
	case EType::Color:
		Comp->SetVariableLinearColor(Name, Color);
		break;
	default: break;
	}
}

void FZzNiagaraParam::CopyFromDataInterface(const UNiagaraDataInterface* Source, UObject* Outer)
{
	DataInterface = DuplicateObject(Source, Outer);
	
	if (Source->IsA(UNiagaraDataInterfaceSpline::StaticClass()))
	{
		DataInterfaceProvider = NewObject<UZzActionNiagaraDataInterfaceProvider_Spline>(Outer);
	}
}

void FZzNiagaraParam::CopyToDataInterface(UNiagaraComponent* Comp, UNiagaraDataInterface* Des) const
{
	if (Des)
	{
		if (DataInterfaceProvider)
		{
			DataInterfaceProvider->CopyTo(Comp, DataInterface, Des);
		}
		else
		{
			DataInterface->CopyTo(Des);
		}
	}
}
