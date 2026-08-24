// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionPactUserDataCustomization.h"

#include "Pact/ZzActionPactUserData.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

void FZzActionPactUserDataCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
                                                          IPropertyTypeCustomizationUtils& Utils)
{
	TArray<UObject*> Objects;
	PropertyHandle->GetOuterObjects(Objects);
	for (auto Obj : Objects)
	{
		if (Obj && !Obj->HasAnyFlags(RF_ArchetypeObject))
		{
			SelectedObjectsList.Add(Obj);
		}
	}
	
	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		PropertyHandle->CreatePropertyValueWidget()
	];
}

void FZzActionPactUserDataCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& Utils)
{
	if (auto a = PropertyHandle->GetChildHandle(0))
	{
		TArray<UObject*> Objects;
		a->GetOuterObjects(Objects);
		UZzActionPactUserData* Object = Objects.IsEmpty() ? nullptr : Cast<UZzActionPactUserData>(Objects[0]);
		if (Object)
		{
			ChildBuilder.AddCustomRow(INVTEXT(""))
			.WholeRowContent()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Lambda([Object, PropertyHandle]()
					{
						Object->Modify();
						Object->UpdateLinks();
						PropertyHandle->RequestRebuildChildren();
						return FReply::Handled();
					})
					[
						// TODO：Add an action that assigns the target and enables debug visualization
						SNew(STextBlock).Text(INVTEXT("Update Links"))
					]
				]
			];
		}
		
		uint32 NumberOfChild;
		if (a->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
		{
			for (uint32 Index = 0; Index < NumberOfChild; ++Index)
			{
				if (auto Child = a->GetChildHandle(Index))
				{
					ChildBuilder.AddProperty(Child.ToSharedRef());
				}
			}
		}
	}
}

TArray<TWeakObjectPtr<UObject>> FZzActionPactUserDataCustomization::GetFunctionCallExecutionContext(TWeakObjectPtr<UFunction> InWeakFunction) const
{
	return SelectedObjectsList;
}