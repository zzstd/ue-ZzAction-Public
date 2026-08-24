// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NodeExtender/ZzActionNE_RunLinkAction.h"
#include "UObject/Object.h"
#include "ZzActionNE_PactRunLinkAction.generated.h"

/**
 * 
 */
UCLASS()
class ZZACTIONPACT_API UZzActionNE_PactRunLinkAction : public UZzActionNE_RunLinkAction
{
	GENERATED_BODY()
public:
	virtual void GetLinkActionData(FZzActionData& OutLinkActionData) const override;
};
