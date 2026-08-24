// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "as_scriptengine.h"

#include "AngelscriptBinds.h"
#include "AngelscriptDocs.h"
#include "AngelscriptManager.h"
#include "ZzActionData.h"

namespace
{
	const UScriptStruct* ResolveScriptStruct(int TypeId)
	{
		const UStruct* Struct = FAngelscriptManager::Get().GetUnrealStructFromAngelscriptTypeId(TypeId);

		const UScriptStruct* ScriptStruct = Cast<UScriptStruct>(Struct);
		if (!ScriptStruct)
		{
			FAngelscriptManager::Throw( "FZzActionData only supports USTRUCT values.");
		}

		return ScriptStruct;
	}
}

AS_FORCE_LINK const FAngelscriptBinds::FBind Bind_Bind_FZzActionData(FAngelscriptBinds::EOrder::Late, []
{
	auto FZzActionData_ = FAngelscriptBinds::ExistingClass("FZzActionData");
	
	FZzActionData_.Method("bool HasStruct(const UScriptStruct StructType) const", [](const FZzActionData* Self, const UScriptStruct* StructType)
	{
		return StructType && Self->HasStruct(StructType);
	});
	
	FZzActionData_.Method("void SetStruct(const FInstancedStruct& Value)", [](FZzActionData* Self, const FInstancedStruct& Value)
	{
		Self->SetStruct(Value);
	});
	FZzActionData_.Method("void SetStruct(const ?&in Value)", [](FZzActionData* Self, const uint8* Value, int TypeId)
	{
		if (const UScriptStruct* StructType = ResolveScriptStruct(TypeId))
		{
			FInstancedStruct Instance;
			Instance.InitializeAs(StructType, Value);
			Self->SetStruct(Instance);
		}
	});
	
	FZzActionData_.Method("bool TryGetStruct(?&out OutValue) const", [](const FZzActionData* Self, uint8* OutValue, int TypeId)
	{
		if (const UScriptStruct* StructType = ResolveScriptStruct(TypeId))
		{
			if (auto Inst = Self->FindStructPtr(StructType))
			{
				StructType->CopyScriptStruct(OutValue, Inst->GetMemory());
				return true;
			}
		}

		return false;
	});
	
	FZzActionData_.Method("bool RemoveStruct(const UScriptStruct StructType)", [](FZzActionData* Self, const UScriptStruct* StructType)
	{
		return StructType && Self->RemoveStruct(StructType);
	});
	
	FZzActionData_.Method("UObject GetObject(FName Name, const TSubclassOf<UObject>& Class = nullptr) const", [](const FZzActionData* Self, FName Name, const TSubclassOf<UObject>& Class)
	{
		return Self->GetObject(Name, Class);
	});
	SCRIPT_BIND_DOCUMENTATION("Get the Object by Name, And treat it as my Class. otherwise, return nullptr.")
	FAngelscriptBinds::SetPreviousBindArgumentDeterminesOutputType(1);
	
	FZzActionData_.Method("bool GetFloatAsBool(FName Name, bool NotFoundValue = false) const", METHOD_TRIVIAL(FZzActionData, GetFloatAsBool));
	FZzActionData_.Method("void SetFloatAsBool(FName Name, bool NewValue)", METHOD_TRIVIAL(FZzActionData, SetFloatAsBool));
	FZzActionData_.Method("void Append(const FZzActionData& OtherData)", METHOD_TRIVIAL(FZzActionData, Append));
});
