// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionNE_PactRunLinkAction.h"

#include "Pact/ZzActionPactLibrary.h"
#include "Nodes/ZzActionLinkTrack.h"

void UZzActionNE_PactRunLinkAction::GetLinkActionData(FZzActionData& OutLinkActionData) const
{
	UZzActionPactLibrary::PushPactData(OutLinkActionData, GetOwnerNode<UZzActionLinkNode>(), GetNodeContext().GetActionInstance());
}
