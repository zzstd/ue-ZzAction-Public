// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZzRootMotionModifierInterface.generated.h"

class UZzNotifyNode_MotionWarping;

UINTERFACE()
class UZzRootMotionModifierInterface : public UInterface
{
	GENERATED_BODY()
};

/*
 *  Allows a URootMotionModifier to resolve its owning ZzAction context.
 */
class ZZACTIONNODES_API IZzRootMotionModifierInterface
{
	GENERATED_BODY()
public:
	virtual void SetOwnerNode(UZzNotifyNode_MotionWarping* InOwnerNode);
	
	TWeakObjectPtr<UZzNotifyNode_MotionWarping> OwnerNode;
};
