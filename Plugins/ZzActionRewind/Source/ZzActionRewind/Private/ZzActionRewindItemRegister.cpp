// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionRewindItemRegister.h"

#include "ZzActionComponent.h"
#include "ZzRewindItem.h"
#include "RewindItem_ZzActionComponent.h"

void UZzActionRewindItemRegister::OnPostComponentCreated(TSharedRef<ZZ::Rewind::FRewindItem> Item, UActorComponent* Component)
{
	using namespace ZZ::Rewind;
	
	if (Component->IsA<UZzActionComponent>())
	{
		Item->AddChildItem<FRewindItem_ZzActionComponent>(Component);
	}
}