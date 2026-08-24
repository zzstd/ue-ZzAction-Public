// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZzActionLinkData.generated.h"

class UZzActionInstance;
class UZzActionLinkNode;

/*
 *  Link Node used ActionData.
 *  
 *  use FZzActionLinkData::Push() to push link data into ActionData before running the linked action. 
 *  @See: UZzActionNE_RunLinkAction.h
 *  
 *  Use GetActionData().TryGetStruct<FZzActionLinkData>() to get the data.
 */
USTRUCT(BlueprintType)
struct ZZACTIONNODES_API FZzActionLinkData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UZzActionLinkNode> LinkNode;
	
	static void Push(struct FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance);
};
