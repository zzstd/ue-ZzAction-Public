// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "ZzActionNodesEditor.h"

#include "ZzActionEditor.h"
#include "ZzActionSectionsTrack.h"
#include "ZzActionSplineUserDataCustomization.h"
#include "ZzActionTrackModel_Sections.h"
#include "ZzNiagaraParamCustomization.h"

#define LOCTEXT_NAMESPACE "FZzActionNodesEditorModule"

void FZzActionNodesEditorModule::StartupModule()
{
	FZzActionEditorModule::Get().RegisterTrackEditor<UZzActionSectionsTrack>(
		FOnMakeTrackEditorDelegate::CreateStatic(&FZzActionTrackModel_Sections::MakeInstance));

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("ZzNiagaraParam"),
	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FZzNiagaraParamCustomization::MakeInstance));
	
	PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("ZzActionSplineUserData"),
	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FZzActionSplineUserDataCustomization::MakeInstance));
}

void FZzActionNodesEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FZzActionNodesEditorModule, ZzActionNodesEditor)