// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/ZzActionNotifyTrack.h"
#include "ZzNotifyNode_DisableRootMotion.generated.h"


UCLASS(DisplayName="Disable Root Motion")
class ZZACTIONNODES_API UZzNotifyNode_DisableRootMotion : public UZzActionNotifyNode
{
	GENERATED_BODY()
public:
	UZzNotifyNode_DisableRootMotion();
	
	virtual void OnBegin() override;
	virtual void OnEnd(bool Abort) override;
private:
	struct FAnimMontageInstance* GetAnimMontageInstance() const;
};
