// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzRewindItemRegister.h"
#include "ZzActionRewindItemRegister.generated.h"

UCLASS()
class UZzActionRewindItemRegister : public UZzRewindItemRegister
{
	GENERATED_BODY()
public:
	virtual void OnPostComponentCreated(TSharedRef<ZZ::Rewind::FRewindItem> Item, UActorComponent* Component) override;
};