// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionNodesFunctionLibrary.h"

#include "ZzActionLinkData.h"

void UZzActionNodesFunctionLibrary::PushLinkData(FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance)
{
	FZzActionLinkData::Push(InActionDataRef, InLinkNode, InActionInstance);
}
