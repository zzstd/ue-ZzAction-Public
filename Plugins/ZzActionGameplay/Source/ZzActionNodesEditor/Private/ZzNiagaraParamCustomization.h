// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once


class FZzNiagaraParamCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FZzNiagaraParamCustomization);
	}
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& Utils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& Utils) override;
	
	bool bIsDataInterface = false;
	TSharedPtr<IPropertyHandle> ValuePropHandle;
	TSharedPtr<IPropertyHandle> DataInterfaceProviderPropHandle;
};
