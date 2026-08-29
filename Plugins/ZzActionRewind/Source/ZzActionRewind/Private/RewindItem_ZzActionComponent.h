// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzRewindFrameItem.h"
#include "ZzRewindActionDataType.h"

class UZzActionComponent;
class UZzActionBlueprint;

namespace ZZ::Rewind
{

class FRewindItem_ZzActionInstance;

class FRewindItem_ZzActionComponent : public FRewindFrameItem
{
public:
	FRewindItem_ZzActionComponent();
	
	virtual void OnSetup(UObject* Owner) override;
	virtual FText GetDisplayName() const override;
	virtual void OnRecord(int32 Frame) override;
	virtual TSharedRef<SWidget> GenerateInspector(int32 Frame) override;
	virtual FText GetTooltipText(int32 Frame) const override;
	
	virtual void OnPaintItem(int32 Frame, FPaintItemData& OutItem) const override;
	virtual void OnPaintFrame(int32 Frame, FPaintFrameData& OutFrame) const override;
	
	TWeakObjectPtr<UZzActionComponent> ActionComponent;
	
	TMap<TWeakObjectPtr<UZzActionBlueprint>, TSharedPtr<FRewindItem_ZzActionInstance>> ActionItems;
	TSharedRef<FRewindItem_ZzActionInstance> FindOrCreateActionInstItem(UZzActionBlueprint* InAction);

	TArray<FActionInfoResult> GetValidActions(int32 Frame) const;

	TSharedPtr<class SVariantValueView> InspectorWidget;

	void OnActionConstructed(UZzActionInstance* InActionInst);
	void OnActionEnded(UZzActionInstance* InActionInst);
	
	void OnActionCheckFailed(UZzActionBlueprint* InActionInst, UObject* InCallObject);
	
	struct FCheckFailedInfo
	{
		int32 RecordFrame;
		TObjectInfo<UZzActionBlueprint> ActionAsset;
		TObjectInfo<UObject> CallObject;
	};
	
	int32 CurrentRecordFrame = 0;
	TMap<int32, TArray<FCheckFailedInfo>> CheckFailedActions;
};

}
