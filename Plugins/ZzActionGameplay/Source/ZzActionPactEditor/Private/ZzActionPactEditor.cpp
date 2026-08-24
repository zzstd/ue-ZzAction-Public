// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "ZzActionPactEditor.h"

#include "ZzActionBlueprint.h"
#include "Nodes/ZzActionLinkTrack.h"
#include "ZzActionBlueprintEditorToolkit.h"
#include "ZzActionEditorToolMenuContext.h"
#include "ZzActionPactEditorStyle.h"
#include "ZzActionPactLinkInfoCustomization.h"
#include "ZzActionPactUserDataCustomization.h"
#include "Pact/ZzActionPactUserData.h"
#include "Timeline/ZzActionTimeline.h"

#define LOCTEXT_NAMESPACE "FZzActionPactEditorModule"

void FZzActionPactEditorModule::StartupModule()
{
	FZzActionPactEditorStyle::Get();
	
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("ZzActionPactUserData"),
	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FZzActionPactUserDataCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("ZzActionPactLinkInfo"),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FZzActionPactLinkInfoCustomization::MakeInstance));

	{
		FToolMenuOwnerScoped OwnerScoped(this);
	
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.ActionBlueprintEditorToolkit.ToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection(NAME_None);
			{
				Section.AddDynamicEntry("QuickCreatePact", FNewToolMenuSectionDelegate::CreateLambda(
				[](FToolMenuSection& Section)
				{
					if (auto* Context = Section.FindContext<UZzActionEditorToolMenuContext>())
					{
						auto WeakEditor = Context->ActionEditor;
						
						const FUIAction Action = FUIAction(
							FExecuteAction::CreateLambda([WeakEditor]()
							{
								if (auto ActionBP = WeakEditor.IsValid() ? WeakEditor.Pin()->GetActionObject() : nullptr)
								{
									FScopedTransaction Transaction(LOCTEXT("QuickCreatePact", "Quick Create Pact"));
									ActionBP->Modify();
									
									auto PactUserData = ActionBP->FindUserDataByClass<UZzActionPactUserData>();
									if (!PactUserData)
									{
										PactUserData = NewObject<UZzActionPactUserData>(ActionBP, UZzActionPactUserData::StaticClass(), NAME_None, RF_Transactional);
										ActionBP->AddUserData(PactUserData);
									}
									check(PactUserData)
									PactUserData->UpdateLinks();
								}
							}),
							FCanExecuteAction::CreateLambda([WeakEditor]()
							{
								if (auto ActionBP = WeakEditor.IsValid() ? WeakEditor.Pin()->GetActionObject() : nullptr)
								{
									if (ActionBP->FindUserDataByClass<UZzActionPactUserData>())
									{
										return false;
									}
									
									if (ActionBP->GetTimeline()->FindNode<UZzActionLinkNode>())
									{
										return true;
									}
								}
								return false;
							})
						);
						
						Section.AddEntry(FToolMenuEntry::InitToolBarButton(
							"QuickCreatePact",
							Action,
							INVTEXT("Create Pact"),
							INVTEXT("Quickly create a counterpoint performance, including Link tracks"),
							FSlateIcon(),
							EUserInterfaceActionType::Button
						));
					}
				}));
			}
		}
	}

}

void FZzActionPactEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FZzActionPactEditorModule, ZzActionPactEditor)