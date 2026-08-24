// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "MotionWarping/ZzRootMotionModifierInterface.h"

#include "MotionWarping/ZzNotifyNode_MotionWarping.h"


void IZzRootMotionModifierInterface::SetOwnerNode(UZzNotifyNode_MotionWarping* InOwnerNode)
{
	OwnerNode = InOwnerNode;
}
