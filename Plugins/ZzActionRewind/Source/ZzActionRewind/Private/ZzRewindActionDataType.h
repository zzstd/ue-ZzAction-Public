// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Utils/ObjectInfo.h"

class UZzActionBlueprint;
class UZzActionInstance;

namespace ZZ::Rewind
{
	
class FVariantValueNode;
class FRewindItem_ZzActionInstance;

using FActionInstanceList = TArray<UZzActionInstance*, TInlineAllocator<1>>;
	
/*
 * copy from FZzActionData, TMap -> TArray
 */
struct FActionDataInfo
{
	TObjectInfo<UObject> PayloadObject;
	
	TArray<FInstancedStruct> Structs;
	
	TArray<TTuple<FName, float>> Floats;
	TArray<TTuple<FName, int32>> Ints;
	TArray<TTuple<FName, FVector>> Vectors;
	TArray<TTuple<FName, FTransform>> Transforms;
	TArray<TTuple<FName, TObjectInfo<UObject>>> Objects;

	FActionDataInfo() {}
	FActionDataInfo(const UZzActionInstance* Inst);
		
	bool operator==(const FActionDataInfo& Other) const;
};

struct FActionInstInfo
{
	int32 Frame = -1;
	float Time = -1;
	float PlayRate = 0;
	float PlayRateInternal = 0;
	
	FActionInstInfo() {}
	FActionInstInfo(const UZzActionInstance* Inst); 
	bool operator==(const FActionInstInfo& Other) const;
};
	
struct FActionInstSharedInfo
{
	int32 ActionUniqueID = -1;
	
	int32 RecordStartFrame = -1;
	
	int32 RecordEndFrame = -1;
	
	TObjectInfo<UObject> ActionInstance;
	
	TObjectInfo<UObject> BeginCallObject;
	
	TObjectInfo<UObject> StopCallObject;
	
	// @TODO: check failed msg
};
	
struct FActionDataInfoList
{
	TArray<TTuple<int32, FActionDataInfo>, TInlineAllocator<1>> List;

	FActionDataInfoList(const FActionInstanceList& InList);
	bool operator==(const FActionDataInfoList& Other) const;
};

struct FActionInstInfoList
{
	TArray<TTuple<int32, FActionInstInfo>, TInlineAllocator<1>> List;

	FActionInstInfoList(const FActionInstanceList& InList);
	bool operator==(const FActionInstInfoList& Other) const;
};
	
struct FActionInfoResult
{
	TObjectInfo<UZzActionBlueprint> ActionAsset;
	TWeakPtr<FRewindItem_ZzActionInstance> WeakActionItem;

	struct FPackedInfo
	{
		FActionInstSharedInfo Info;
		FActionInstInfo Inst;
		FActionDataInfo	Data;
	};
	
	TArray<FPackedInfo, TInlineAllocator<1>> Infos;
};
	
	
}