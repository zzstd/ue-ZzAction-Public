// Copyright 2024-2026 zz studio. All Rights Reserved.


#pragma once

#include "Styling/SlateStyle.h"
#include "Templates/UniquePtr.h"

class FZzActionPactEditorStyle : public FSlateStyleSet
{
public:
	FZzActionPactEditorStyle() 
		: FSlateStyleSet("ZzActionPactEditorStyle")
	{
	}
	
	virtual ~FZzActionPactEditorStyle() override;

	static const FZzActionPactEditorStyle& Get()
	{
		if (!Inst.IsValid())
		{
			Inst = TUniquePtr<FZzActionPactEditorStyle>(new FZzActionPactEditorStyle);
			Inst->Initialize();
		}
		return *(Inst.Get());
	}
	
	void Initialize();
	static TUniquePtr<FZzActionPactEditorStyle> Inst;
};
