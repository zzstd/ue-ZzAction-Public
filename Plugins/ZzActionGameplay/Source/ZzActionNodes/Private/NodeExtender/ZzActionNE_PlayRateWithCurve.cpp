// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "NodeExtender/ZzActionNE_PlayRateWithCurve.h"

#include "ZzActionInstance.h"
#include "Nodes/ZzActionCurveTrack.h"

void UZzActionNE_PlayRateWithCurve::OnBeginConst_Implementation(const FZzActionNodeContext& Context) const
{
    UpdatePlayRateWithCurve(Context);
}

void UZzActionNE_PlayRateWithCurve::OnUpdateConst_Implementation(const FZzActionNodeContext& Context) const
{
    UpdatePlayRateWithCurve(Context);
}

void UZzActionNE_PlayRateWithCurve::OnEndConst_Implementation(const FZzActionNodeContext& Context, bool Abort) const
{
    UpdatePlayRateWithCurve(Context);
}

void UZzActionNE_PlayRateWithCurve::UpdatePlayRateWithCurve(const FZzActionNodeContext& Context) const
{
    if (auto CurveNode = GetOwnerNode<UZzActionCurveNode>())
    {
        float NewPlayRate = CurveNode->GetValue(Context.GetCurrentTime());
        Context.GetActionInstance()->SetInternalPlayRate(NewPlayRate);
    }
}
