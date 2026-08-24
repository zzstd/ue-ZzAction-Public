// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionLinkData.h"

#include "ZzActionData.h"
#include "ZzActionInstance.h"
#include "Nodes/ZzActionLinkTrack.h"

void FZzActionLinkData::Push(FZzActionData& InActionDataRef, UZzActionLinkNode* InLinkNode, const UZzActionInstance* InActionInstance)
{
	FZzActionLinkData LinkData;
	LinkData.LinkNode = InLinkNode;
	InActionDataRef.SetStruct(LinkData);
	
	if (ensure(InActionInstance))
	{
		FZzActionData TempHookData;
		TempHookData.InstanceHooks = InActionInstance->ActionData.InstanceHooks;
		InActionDataRef.Append(TempHookData);
	}
}
