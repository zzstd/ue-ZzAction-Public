// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Node/ZzActionNodeExtender.h"
#include "ZzActionNE_PlayRateWithCurve.generated.h"


UCLASS()
class ZZACTIONNODES_API UZzActionNE_PlayRateWithCurve : public UZzActionNodeExtender
{
	GENERATED_BODY()
public:
	void OnBeginConst_Implementation(const FZzActionNodeContext& Context) const override;
	void OnUpdateConst_Implementation(const FZzActionNodeContext& Context) const override;
	void OnEndConst_Implementation(const FZzActionNodeContext& Context, bool Abort) const override;

	void UpdatePlayRateWithCurve(const FZzActionNodeContext& Context) const;
};