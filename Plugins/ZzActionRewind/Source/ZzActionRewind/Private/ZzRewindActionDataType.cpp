// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzRewindActionDataType.h"

#include "ZzActionInstance.h"


namespace ZZ::Rewind
{

template<typename Key, typename Value, typename Value2>
void CopyMapToArray(TArray<TTuple<Key, Value>>& Des, const TMap<Key, Value2>& Src)
{
	Des.Empty(Src.Num());
	
	for (const auto& Pair : Src)
	{
		Des.Add(Pair);
	}
}

FActionDataInfo::FActionDataInfo(const UZzActionInstance* Inst)
{
	PayloadObject = Inst->GetActionData().PayloadObject;
	
	Structs			= Inst->GetActionData().Structs;
	
	Floats			= Inst->GetActionData().Floats.Array();
	Ints			= Inst->GetActionData().Ints.Array();
	Vectors			= Inst->GetActionData().Vectors.Array();
	Transforms		= Inst->GetActionData().Transforms.Array();
	
	CopyMapToArray(Objects, Inst->GetActionData().Objects);
}

bool FActionDataInfo::operator==(const FActionDataInfo& Other) const
{
	if (PayloadObject == Other.PayloadObject
		&& Structs == Other.Structs
		&& Floats == Other.Floats
		&& Ints == Other.Ints
		&& Vectors == Other.Vectors
		&& Objects == Other.Objects)
	{
		if (Transforms.Num() != Other.Transforms.Num())
		{
			return false;
		}
		
		for (int32 i = 0; i < Transforms.Num(); i++)
		{
			if (Transforms[i].Key != Other.Transforms[i].Key)
			{
				return false;
			}
			
			if (!Transforms[i].Value.Equals(Other.Transforms[i].Value))
			{
				return false;
			}
		}
		
		return true;
	}

	return false;
}

FActionInstInfo::FActionInstInfo(const UZzActionInstance* Inst)
{
	Frame				= Inst->GetCurrentFrame();
	Time				= Inst->GetCurrentTime();
	PlayRate			= Inst->GetFinalPlayRate();
	PlayRateInternal	= Inst->GetInternalPlayRate();
}

bool FActionInstInfo::operator==(const FActionInstInfo& Other) const
{
	return
		Frame == Other.Frame &&
		Time == Other.Time &&
		PlayRate == Other.PlayRate &&
		PlayRateInternal == Other.PlayRateInternal;
}

FActionDataInfoList::FActionDataInfoList(const FActionInstanceList& InList)
{
	List.Reserve(InList.Num());
	for (auto Inst : InList)
	{
		List.Add({Inst->ActionUniqueID, Inst});
	}
}
bool FActionDataInfoList::operator==(const FActionDataInfoList& Other) const
{
	return List == Other.List;
}
	
FActionInstInfoList::FActionInstInfoList(const FActionInstanceList& InList)
{
	List.Reserve(InList.Num());
	for (auto Inst : InList)
	{
		List.Add({Inst->ActionUniqueID, Inst});
	}
}

bool FActionInstInfoList::operator==(const FActionInstInfoList& Other) const
{
	return List == Other.List;
}
	
}
