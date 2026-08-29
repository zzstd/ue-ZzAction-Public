// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzNotifyNode_Loop.h"
#include "ZzActionNodes.h"

#include "CanvasTypes.h"
#include "ZzActionInstance.h"
#include "Timeline/ZzActionTimeline.h"

UZzNotifyNode_LoopBase::UZzNotifyNode_LoopBase()
{
	InstancingPolicy = EZzActionNodeInstancingPolicy::InstancedPerExecution;
}

void UZzNotifyNode_LoopBase::OnTick(float DeltaTime)
{
	Super::OnTick(DeltaTime);

	if (bGotoToEndOnBreak && !ShouldLoop())
	{
		// bHasBreak = true;
		GetNodeContext().GetActionInstance()->SetTime(GetEndTime(), true);
	}
}

void UZzNotifyNode_LoopBase::OnCede()
{
	Super::OnCede();

	LoopCounter++;
	if (ShouldLoop())
	{
		GetNodeContext().GetActionInstance()->SetTime(GetBeginTime(), true);
		OnBeginLoop();
	} // else wait OnEnd() call OnBreakLoop
}

void UZzNotifyNode_LoopBase::OnEnd(bool Abort)
{
	Super::OnEnd(Abort);
	OnStopLoop(Abort);
}

FLinearColor UZzNotifyNode_LoopBase::GetNotifyColor_Implementation() const
{
	return FLinearColor(0.153f, 0.153f, 0.361f, 1.0f);
}

void UZzNotifyNode_LoopBase::OnBeginLoop_Implementation()
{
	//UE_LOG(LogTemp, Display, TEXT("ZzAction[%s] begin loop [%d]"), *GetNodeContext().GetAssetName(), LoopCounter)
}

void UZzNotifyNode_LoopBase::OnStopLoop_Implementation(bool Abort)
{
	//UE_LOG(LogTemp, Display, TEXT("ZzAction[%s] break loop, break[%d] abort[%d]"), *GetNodeContext().GetAssetName(), bHasBreak, Abort)
}

#if WITH_EDITOR
void UZzNotifyNode_LoopBase::OnPreviewEditorDrawCanvas(class FCanvas& Canvas, FSceneView& View) const
{
	const FText MsgText = FText::FromString(FString::Printf(TEXT("Current Loop Count: %d"), LoopCounter));
	constexpr int32 MessageX = 20;
	constexpr int32 MessageY = 95;
	Canvas.DrawShadowedText(MessageX, MessageY, MsgText, GEngine->GetSmallFont(), FColor::White);
}
#endif


bool UZzNotifyNode_LoopBase::ShouldLoop() const
{
	checkNoEntry()
	return false;
}

bool UZzNotifyNode_SimpleLoop::ShouldLoop() const
{
	return BP_ShouldLoop();
}

void UZzNotifyNode_SimpleLoop::StopLoop_Implementation() const
{
	if (auto ActioInst = GetNodeContext().GetActionInstance())
	{
		ActioInst->ActionData.SetFloatAsBool(CheckLoopDataName, !bDefaultLoop);
	}
}

bool UZzNotifyNode_SimpleLoop::StopActionLoop(UZzActionInstance* InActionInst)
{
	if (auto Timeline = InActionInst ? InActionInst->GetTimeline() : nullptr)
	{
		if (auto SimpleLoopNode = Timeline->FindNode<UZzNotifyNode_SimpleLoop>())
		{
			SimpleLoopNode->StopLoop();
			SimpleLoopNode->bHasBreak = true;
			return true;
		}
	}

	return false;
}

bool UZzNotifyNode_SimpleLoop::BP_ShouldLoop_Implementation() const
{
	if (GetNodeContext().IsPreview())
	{
		return LoopCounter < PreviewLoopCount;
	}

	const bool Result = GetNodeContext().GetActionData().GetFloatAsBool(CheckLoopDataName, false);
	//UE_LOG(LogTemp, Display, TEXT("ZzAction[%s] data [%d] break [%d]"), *GetNodeContext().GetAssetName(), Result, bHasBreak);
	if (bDefaultLoop && !bHasBreak)
	{
		return !Result;
	}
	return Result;
}
