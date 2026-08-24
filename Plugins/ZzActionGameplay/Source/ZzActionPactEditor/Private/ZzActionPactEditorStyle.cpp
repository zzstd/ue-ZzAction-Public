// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionPactEditorStyle.h"

#include "Styling/StyleColors.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

TUniquePtr<FZzActionPactEditorStyle> FZzActionPactEditorStyle::Inst(nullptr);

FZzActionPactEditorStyle::~FZzActionPactEditorStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

void FZzActionPactEditorStyle::Initialize()
{
	const FVector2D Icon16x16(16.0f, 16.0f);
		
	SetContentRoot(IPluginManager::Get().FindPlugin("ZzActionGameplay")->GetBaseDir() / TEXT("Resources"));
	Set("PactEditor.Debug", new IMAGE_BRUSH_SVG("Debug", Icon16x16));
	
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}
