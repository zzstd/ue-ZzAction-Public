// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "RewindItem_ZzActionInstance.h"

#include "ZzActionBlueprint.h"
#include "ZzActionInstance.h"
#include "Timeline/ZzActionTimeline.h"
#include "Widgets/SVariantValueView.h"


namespace ZZ::Rewind
{

FRewindItem_ZzActionInstance::FRewindItem_ZzActionInstance()
{
	bPaintFrame = true;
}

void FRewindItem_ZzActionInstance::OnSetup(UObject* Owner)
{
	FRewindFrameItem::OnSetup(Owner);

	auto Asset = Cast<UZzActionBlueprint>(Owner);
	check(Asset);
	ActionAsset = Asset;
	CachedActionAssetLengthFrame = Asset->GetLengthFrame();
}

FText FRewindItem_ZzActionInstance::GetDisplayName() const
{
	return FText::FromString(ActionAsset.ObjectName);
}

void FRewindItem_ZzActionInstance::OnPaintFrame(int32 Frame, FPaintFrameData& OutPaint) const
{
	if (auto p = CachedActionInst.Find(Frame))
	{
		OutPaint.Valid = true;
		OutPaint.Color = FColor(0, 170, 170);
		
		int32 CurFrame = p->List.IsEmpty() ? 0 :  p->List[0].Value.Frame;
		OutPaint.bDrawCurve = true;
		OutPaint.CurveColor = FColor::White;
		OutPaint.CurveValue = static_cast<float>(CurFrame) / CachedActionAssetLengthFrame;
	}
}

TSharedRef<SWidget> FRewindItem_ZzActionInstance::GenerateInspector(int32 Frame)
{
	if (!InspectorWidget)
	{
		SAssignNew(InspectorWidget, SVariantValueView);
	}
	
	auto InspectorNode = InspectorWidget->GetRoot()->Begin("Action", ActionAsset);
	
	FActionInfoResult FindData;
	if (FindValidActionData(Frame, &FindData))
	{
		if (FindData.Infos.Num() == 1)
		{
			FullInspectorData(InspectorNode, FindData.Infos[0], ActionAsset.WeakObject.Get());
		}
		else
		{
			int32 Index = 0;
			for (auto& Info : FindData.Infos)
			{
				FullInspectorData(InspectorNode->Begin(FString::Printf(TEXT("[%d]"), Index++))
					, Info, ActionAsset.WeakObject.Get());
			}
		}
		InspectorNode->Expand();
		InspectorNode->ShowChildren();
	}
	else
	{
		InspectorNode->HiddenChildren();
	}
	
	return InspectorWidget.ToSharedRef();
}

void FRewindItem_ZzActionInstance::OnRecordActionInst(int32 Frame, const FActionInstanceList& List)
{
	for (auto Inst : List)
	{
		auto p = SharedInstMap.Find(Inst->ActionUniqueID);
		if (ensure(p != nullptr))
		{
			if (p->RecordStartFrame < 0)
			{
				p->RecordStartFrame = Frame;
			}
			p->RecordEndFrame = Frame;
		}
	}
	
	CachedActionInst.Append(Frame, List);
	CachedActionData.Append(Frame, List);
}

void FRewindItem_ZzActionInstance::FullInspectorData(TSharedRef<FVariantValueNode> RootNode,
	const FActionInfoResult::FPackedInfo& Info, const UZzActionBlueprint* ActionAsset)
{
	int32 AssetLengthFrame = 0;
	float AssetLength = 1.f;
	if (ActionAsset)
	{
		if (auto Timeline = ActionAsset->GetTimeline())
		{
			AssetLengthFrame = Timeline->GetLengthFrame();
			AssetLength = Timeline->GetLength();
		}
	}
	
	RootNode->Show()
	->AddValue("Current Frame", FString::Printf(TEXT("%d / %d"), Info.Inst.Frame, AssetLengthFrame))
	->AddValue("Current Time", FString::Printf(TEXT("%.3f / %.1f%%"), Info.Inst.Time, (Info.Inst.Time / AssetLength) * 100.f))
	->AddValue("Play Rate", Info.Inst.PlayRate)
	->AddValue("Play Rate Internal", Info.Inst.PlayRateInternal)
	
	->AddValue("Call Object", Info.Info.BeginCallObject)
	->AddValue("Stop Call Object", Info.Info.StopCallObject)
	->AddValue("Action Unique ID", Info.Info.ActionUniqueID)
	
	->Begin("Action Data")
		->AddValue("Payload Object", Info.Data.PayloadObject)
		->SetValue_ArrayWithName("Floats", Info.Data.Floats, true)
		->SetValue_ArrayWithName("Ints", Info.Data.Ints, true)
		->SetValue_ArrayWithName("Vectors", Info.Data.Vectors, true)
		->SetValue_ArrayWithName("Transforms", Info.Data.Transforms, true)
		->SetValue_ArrayWithName("Objects", Info.Data.Objects, true);
}

bool FRewindItem_ZzActionInstance::FindValidActionData(int32 Frame, FActionInfoResult* OutResult) const
{
	const auto* Inst = CachedActionInst.Find(Frame);
	if (!Inst)
	{
		return false;
	}

	if (OutResult)
	{
		OutResult->ActionAsset = ActionAsset;
		OutResult->WeakActionItem = ConstCastSharedRef<FRewindItem_ZzActionInstance>(SharedThis(this));
		
		auto& DataList = *CachedActionData.Find(Frame);
		check(Inst->List.Num() == DataList.List.Num());
		
		for (int32 i = 0; i < Inst->List.Num(); ++i)
		{
			const int32 ActionUniqueID = Inst->List[i].Key;
			
			auto& Packed = OutResult->Infos.AddDefaulted_GetRef();
			Packed.Inst = Inst->List[i].Value;	
			Packed.Data = DataList.List[i].Value;

			auto InfoPtr = SharedInstMap.Find(ActionUniqueID);
			if (ensure(InfoPtr))
			{
				Packed.Info = *InfoPtr;
			}

			ensure(ActionUniqueID == Packed.Info.ActionUniqueID);
			ensure(ActionUniqueID == DataList.List[i].Key);
		}
	}
	
	return true;
}

void FRewindItem_ZzActionInstance::SetEndCallObject(int32 ActionID, UObject* EndCallObj)
{
	SharedInstMap.FindOrAdd(ActionID).StopCallObject = EndCallObj;
}

}
