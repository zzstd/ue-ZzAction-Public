// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "ZzNiagaraParamCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Niagara/ZzNotifyNode_NiagaraEffectWithParams.h"
#include "Spline/ZzActionNiagaraDataInterfaceProvider_Spline.h"
#include "Spline/ZzActionSplineUserData.h"

void FZzNiagaraParamCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
												   IPropertyTypeCustomizationUtils& Utils)
{
	auto NamePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Name));
	auto TypePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Type));
	
	FName ParamName;
	NamePropHandle->GetValue(ParamName);
	
	int32 TypeValue = 0;
	TypePropHandle->GetValue(TypeValue);
	FZzNiagaraParam::EType Type = static_cast<FZzNiagaraParam::EType>(TypeValue);
	
	switch (Type)
	{
	case FZzNiagaraParam::EType::None:
		break;
	case FZzNiagaraParam::EType::Obj:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Object));
		break;
	case FZzNiagaraParam::EType::DataInterface:
		bIsDataInterface = true;
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, DataInterface));
		DataInterfaceProviderPropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, DataInterfaceProvider));
		break;
	case FZzNiagaraParam::EType::Float:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Float));
		break;
	case FZzNiagaraParam::EType::Bool:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Bool));
		break;
	case FZzNiagaraParam::EType::Int:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Int));
		break;
	case FZzNiagaraParam::EType::Vec2:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Vec2));
		break;
	case FZzNiagaraParam::EType::Vec3:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Vec3));
		break;
	case FZzNiagaraParam::EType::Color:
		ValuePropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FZzNiagaraParam, Color));
		break;
	}
	
	HeaderRow
	.NameContent()
	[
		NamePropHandle->CreatePropertyValueWidget()
	];
	
	if (ValuePropHandle)
	{
		HeaderRow.ValueContent()
		[
			ValuePropHandle->CreatePropertyValueWidgetWithCustomization(nullptr)
		];
	}
}

void FZzNiagaraParamCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& Utils)
{
	if (!ValuePropHandle)
		return;
	
	UZzActionNiagaraDataInterfaceProvider_Spline* SplineProviderObj = nullptr;
	if (DataInterfaceProviderPropHandle)
	{
		UObject* ProviderObj;
		DataInterfaceProviderPropHandle->GetValue(ProviderObj);
		SplineProviderObj = Cast<UZzActionNiagaraDataInterfaceProvider_Spline>(ProviderObj);

		if (auto Prop = DataInterfaceProviderPropHandle->GetChildHandle(0))
		{

			uint32 NumberOfChild;
			if (Prop->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
			{
				for (uint32 Index = 0; Index < NumberOfChild; ++Index)
				{
					if (auto Child = Prop->GetChildHandle(Index))
					{
						ChildBuilder.AddProperty(Child.ToSharedRef());
					}
				}
			}
		}
	}
	
	{
		auto Prop = bIsDataInterface ? ValuePropHandle->GetChildHandle(0) : ValuePropHandle;
		uint32 NumberOfChild;
		if (Prop->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
		{
			for (uint32 Index = 0; Index < NumberOfChild; ++Index)
			{
				if (auto Child = Prop->GetChildHandle(Index))
				{
					if (SplineProviderObj)
					{
						if (auto ChildProp = Child->GetProperty())
						{
							if (SplineProviderObj->ShouldHiddenDIProperty(ChildProp->GetFName()))
							{
								continue;
							}
						}
					}
					ChildBuilder.AddProperty(Child.ToSharedRef());
				}
			}
		}
	}
	
}
