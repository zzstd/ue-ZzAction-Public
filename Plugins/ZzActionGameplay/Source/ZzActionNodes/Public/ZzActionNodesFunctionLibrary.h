// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ZzActionNodesFunctionLibrary.generated.h"

struct FZzActionData;

UCLASS()
class ZZACTIONNODES_API UZzActionNodesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Link")
	static void PushLinkData(UPARAM(Ref) FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance);
};
