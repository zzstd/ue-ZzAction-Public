// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "RewindItem_ZzActionComponent.h"

#include "RewindItem_ZzActionInstance.h"
#include "ZzRewindActionDataType.h"
#include "ZzActionBlueprint.h"
#include "ZzActionComponent.h"
#include "ZzActionInstance.h"
#include "Widgets/SVariantValueView.h"

namespace ZZ::Rewind
{

FRewindItem_ZzActionComponent::FRewindItem_ZzActionComponent()
{
	bPaintFrame = true;
	bPaintItem = true;
}

void FRewindItem_ZzActionComponent::OnSetup(UObject* Owner)
{
	ActionComponent = Cast<UZzActionComponent>(Owner);
	check(ActionComponent.IsValid());

	ActionComponent->OnActionConstructedDelegate.AddSP(this, &FRewindItem_ZzActionComponent::OnActionConstructed);
	ActionComponent->OnActionEndDelegate.AddSP(this, &FRewindItem_ZzActionComponent::OnActionEnded);
	ActionComponent->OnActionCheckFailedDelegate.AddSP(this, &FRewindItem_ZzActionComponent::OnActionCheckFailed);
	
	for (auto a : ActionComponent->GetAllActivatingActions())
	{
		OnActionConstructed(a);
	}
	for (auto a : ActionComponent->GetAllDeferredActions())
	{
		OnActionConstructed(a);
	}
}

FText FRewindItem_ZzActionComponent::GetDisplayName() const
{
	return FText::FromString("Action Component");
}

void FRewindItem_ZzActionComponent::OnRecord(int32 Frame)
{
	FRewindFrameItem::OnRecord(Frame);
	
	if (ActionComponent.IsValid())
	{
		TMap<UZzActionBlueprint*, FActionInstanceList> aaa;
		for (auto ActionInst : ActionComponent->GetAllActivatingActions())
		{
			aaa.FindOrAdd(ActionInst->GetActionAsset()).Add(ActionInst);
		}
		
		for (auto& a : aaa)
		{
			FindOrCreateActionInstItem(a.Key)->OnRecordActionInst(Frame, a.Value);
		}
	}

	CurrentRecordFrame = Frame + 1;
}

TSharedRef<SWidget> FRewindItem_ZzActionComponent::GenerateInspector(int32 Frame)
{
	if (!InspectorWidget)
	{
		SAssignNew(InspectorWidget, SVariantValueView);
	}

	InspectorWidget->GetRoot()->HiddenChildren();
	
	int32 Index = 0;
	for (auto ActionInstItemPair: ActionItems)
	{
		FActionInfoResult Result;
		if (ActionInstItemPair.Value->FindValidActionData(Frame, &Result))
		{
			for (auto& Info : Result.Infos)
			{
				ActionInstItemPair.Value->FullInspectorData(
					InspectorWidget->GetRoot()->Begin(FString::Printf(TEXT("[%d]"), Index++), Result.ActionAsset)
					, Info, Result.ActionAsset.WeakObject.Get());
			}
		}
	}
	
	return InspectorWidget.ToSharedRef();
}

FText FRewindItem_ZzActionComponent::GetTooltipText(int32 Frame) const
{
	TArray<FString> Names;
	for (auto ActionInstItemPair: ActionItems)
	{
		FActionInfoResult Result;
		if (ActionInstItemPair.Value->FindValidActionData(Frame, &Result))
		{
			if (Result.Infos.Num() == 1)
			{
				Names.Add(Result.ActionAsset.ObjectName);
			}
			else
			{
				for (auto& Info : Result.Infos)
				{
					Names.Add(FString::Printf(TEXT("%s [%d]"), *Result.ActionAsset.ObjectName, Info.Info.ActionUniqueID));
				}
			}
		}
	}
	
	if (auto CheckFailedInfos = CheckFailedActions.Find(Frame))
	{
		for (auto& Info : *CheckFailedInfos)
		{
			Names.Add(FString::Printf(TEXT("Check Failed: %s  (%s"), *Info.ActionAsset.ObjectName, *Info.CallObject.ObjectName));
		}
	}

	return FText::FromString(FString::Join(Names, TEXT("\n")));
}

void FRewindItem_ZzActionComponent::OnPaintFrame(int32 Frame, FPaintFrameData& OutFrame) const
{
	auto Actions = GetValidActions(Frame);
	if (!Actions.IsEmpty())
	{
		OutFrame.Valid = Actions.Num() > 1 || Actions[0].Infos.Num() > 1;
		OutFrame.Widget = 0.3f;
		OutFrame.Color = FColor(230, 0, 170);
	}

	if (CheckFailedActions.Find(Frame))
	{
		OutFrame.Valid = true;
		OutFrame.Color = FColor::Red;
	}
}

void FRewindItem_ZzActionComponent::OnPaintItem(int32 Frame, FPaintItemData& OutItem) const
{
	auto Actions = GetValidActions(Frame);
	if (!Actions.IsEmpty())
	{
		OutItem.Valid = true;

		OutItem.StartFrame = Actions[0].Infos[0].Info.RecordStartFrame;
		OutItem.EndFrame = Actions[0].Infos[0].Info.RecordEndFrame;
		
		OutItem.Color = FColor(0, 170, 170);
		OutItem.Text = Actions[0].ActionAsset.ObjectName;
		OutItem.TextColor = FLinearColor::White;
	}
}

TSharedRef<FRewindItem_ZzActionInstance> FRewindItem_ZzActionComponent::FindOrCreateActionInstItem(
	UZzActionBlueprint* InAction)
{
	if (TSharedPtr<FRewindItem_ZzActionInstance>* ActionItem = ActionItems.Find(InAction))
	{
		return ActionItem->ToSharedRef();
	}
	
	auto NewItem = AddChildItem<FRewindItem_ZzActionInstance>(InAction);
	ActionItems.Add(InAction, NewItem);
	return NewItem;
}

TArray<FActionInfoResult> FRewindItem_ZzActionComponent::GetValidActions(int32 Frame) const
{
	TArray<FActionInfoResult> Result;
	
	for (auto ActionInstItemPair: ActionItems)
	{
		FActionInfoResult ValidData;
		if (ActionInstItemPair.Value->FindValidActionData(Frame, &ValidData))
		{
			Result.Add(ValidData);
		}
	}

	return Result;
}

void FRewindItem_ZzActionComponent::OnActionConstructed(UZzActionInstance* InActionInst)
{
	auto Item = FindOrCreateActionInstItem(InActionInst->GetActionAsset());
	ensure(!Item->SharedInstMap.Contains(InActionInst->ActionUniqueID));
	
	auto& Info = Item->SharedInstMap.FindOrAdd(InActionInst->ActionUniqueID);
	Info.ActionUniqueID = InActionInst->ActionUniqueID;
	Info.ActionInstance = InActionInst;
	Info.BeginCallObject = InActionInst->StartCallObject;
}

void FRewindItem_ZzActionComponent::OnActionEnded(UZzActionInstance* InActionInst)
{
	auto Item = FindOrCreateActionInstItem(InActionInst->GetActionAsset());
	ensure(Item->SharedInstMap.Contains(InActionInst->ActionUniqueID));
	
	Item->SharedInstMap.FindOrAdd(InActionInst->ActionUniqueID).StopCallObject = InActionInst->StopCallObject;
}


void FRewindItem_ZzActionComponent::OnActionCheckFailed(UZzActionBlueprint* InActionInst, UObject* InCallObject)
{
	auto& Ref = CheckFailedActions.FindOrAdd(CurrentRecordFrame).AddDefaulted_GetRef();
	Ref.RecordFrame = CurrentRecordFrame;
	Ref.ActionAsset = InActionInst;
	Ref.CallObject = InCallObject;
}

}
