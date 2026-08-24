// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "ZzActionSplineUserDataCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Editor/ComponentVisualizers/Public/SplineDetailsProvider.h"
#include "Spline/ZzActionSplineUserData.h"
#include "Subsystems/EditorActorSubsystem.h"

void FZzActionSplineUserDataCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
														   IPropertyTypeCustomizationUtils& Utils)
{
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

void FZzActionSplineUserDataCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& Utils)
{
	SplinePropertyHandle = PropertyHandle->GetChildHandle(0);
	check(SplinePropertyHandle);
	
	ChildBuilder.AddCustomRow(FText()).WholeRowContent()
	[
		SNew(SHorizontalBox)
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.Text(INVTEXT("Select all points"))
			.OnClicked(this, &FZzActionSplineUserDataCustomization::HandleSelectAllSplinePoints)
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.Text(INVTEXT("Export to level"))
			.ToolTipText(INVTEXT("Need to select split actor in the level editor"))
			.IsEnabled(this, &FZzActionSplineUserDataCustomization::HasSelectedSpline)
			.OnClicked(this, &FZzActionSplineUserDataCustomization::ExportToLevel)
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.Text(INVTEXT("Import from level"))
			.ToolTipText(INVTEXT("Need to select split actor in the level editor"))
			.IsEnabled(this, &FZzActionSplineUserDataCustomization::HasSelectedSpline)
			.OnClicked(this, &FZzActionSplineUserDataCustomization::ImportFromLevel)
		]
	];
	
	uint32 NumberOfChild;
	if (SplinePropertyHandle->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
	{
		for (uint32 Index = 0; Index < NumberOfChild; ++Index)
		{
			if (auto Child = SplinePropertyHandle->GetChildHandle(Index))
			{
				ChildBuilder.AddProperty(Child.ToSharedRef());
			}
		}
	}
}

UZzActionSplineUserData* FZzActionSplineUserDataCustomization::GetSplineUserData() const
{
	if (SplinePropertyHandle)
	{
		TArray<UObject*> Objects;
		SplinePropertyHandle->GetOuterObjects(Objects);
		if (Objects.Num() == 1)
		{
			return Cast<UZzActionSplineUserData>(Objects[0]);
		}
	}
	
	return nullptr;
}

USplineComponent* FZzActionSplineUserDataCustomization::GetLevelSelectedSplineComponent() const
{
	if (auto EditorActorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UEditorActorSubsystem>() : nullptr)
	{
		auto Actors = EditorActorSubsystem->GetSelectedLevelActors();
		if (Actors.Num() == 1)
		{
			return Actors[0]->FindComponentByClass<USplineComponent>();
		}
	}
	
	return nullptr;
}

bool FZzActionSplineUserDataCustomization::HasSelectedSpline() const
{
	return GetLevelSelectedSplineComponent() != nullptr;
}

FReply FZzActionSplineUserDataCustomization::ExportToLevel()
{
	if (auto SplineUserData = GetSplineUserData())
	{
		if (auto SplineComp = GetLevelSelectedSplineComponent())
		{
			SplineComp->Modify();
			SplineUserData->CopyTo(SplineComp);
		}
	}
	
	return FReply::Handled();
}

FReply FZzActionSplineUserDataCustomization::ImportFromLevel()
{
	if (auto SplineUserData = GetSplineUserData())
	{
		if (auto SplineComp = GetLevelSelectedSplineComponent())
		{
			SplineUserData->Modify();
			SplineUserData->CopyFrom(SplineComp);
			if (SplineUserData->PreviewSplineComponent)
			{
				SplineUserData->CopyTo(SplineUserData->PreviewSplineComponent);
			}
		}
	}

	return FReply::Handled();
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS // disable use ISplineDetailsProvider warning
FReply FZzActionSplineUserDataCustomization::HandleSelectAllSplinePoints()
{
	if (auto SplineUserData = GetSplineUserData())
	{
		if (SplineUserData->PreviewSplineComponent)
		{
			if (auto SplineInterface = GetSplineDetailsProvider(SplineUserData->PreviewSplineComponent))
			{
				SplineInterface->HandleSelectAllSplinePoints(SplineUserData->PreviewSplineComponent);
				SplineInterface->ActivateVisualization();
			}
		}
	}
	
	return FReply::Handled();
}

ISplineDetailsProvider* FZzActionSplineUserDataCustomization::GetSplineDetailsProvider(const USplineComponent* SplineComponent)
{
	for (ISplineDetailsProvider* Interface : IModularFeatures::Get().GetModularFeatureImplementations<ISplineDetailsProvider>(ISplineDetailsProvider::GetModularFeatureName()))
	{
		if (Interface && Interface->ShouldUseForSpline(SplineComponent))
		{
			return Interface;
		}
	}
	
	return nullptr;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS