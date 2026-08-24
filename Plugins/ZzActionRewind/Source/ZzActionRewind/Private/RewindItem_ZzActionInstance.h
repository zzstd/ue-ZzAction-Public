// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZzRewindFrameItem.h"
#include "RewindItem_ZzActionComponent.h"
#include "Utils/FrameData.h"
#include "Utils/ObjectInfo.h"

class UZzActionBlueprint;
class UZzActionInstance;

namespace ZZ::Rewind
{

class FVariantValueNode;
class SVariantValueView;

class FRewindItem_ZzActionInstance : public FRewindFrameItem
{
public:
	FRewindItem_ZzActionInstance();
	
	virtual void OnSetup(UObject* Owner) override;
	virtual FText GetDisplayName() const override;
	virtual void OnPaintFrame(int32 Frame, FPaintFrameData& OutPaint) const override;
	virtual TSharedRef<SWidget> GenerateInspector(int32 Frame) override;

	TObjectInfo<UZzActionBlueprint> ActionAsset;
	int32 CachedActionAssetLengthFrame = 0;
	
	void OnRecordActionInst(int32 Frame, const FActionInstanceList& List);

	static void FullInspectorData(TSharedRef<FVariantValueNode> RootNode, const FActionInfoResult::FPackedInfo& Info, const UZzActionBlueprint* ActionAsset);
	
	bool FindValidActionData(int32 Frame, FActionInfoResult* OutResult = nullptr) const;

	void SetEndCallObject(int32 ActionID, UObject* EndCallObj);
	TMap<int32, FActionInstSharedInfo> SharedInstMap;
private:
	TSharedPtr<SVariantValueView> InspectorWidget;
	
	TFrameData<FActionInstInfoList> CachedActionInst;
	TFrameData<FActionDataInfoList> CachedActionData;
};

}