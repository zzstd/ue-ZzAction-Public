// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class FZzActionPactLinkInfoCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FZzActionPactLinkInfoCustomization);
	}
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& Utils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& Utils) override;

private:
	TSharedPtr<SBox> FixBox;
	void Update();
	
	class UZzActionPactUserData* PactAnimationUserData = nullptr;
	int32 CurrentIndex = INDEX_NONE;
	
	struct FMsgData
	{
		// 0 info   1 warning   2 error
		int32 Level;
		FString Msg;
		FString ClickText;
		FOnClicked OnClicked;
	};
	TSharedRef<SWidget> MakeMotionWarpUtilsWidget();
	void UpdateMsg(TArray<FMsgData>& List);
};
