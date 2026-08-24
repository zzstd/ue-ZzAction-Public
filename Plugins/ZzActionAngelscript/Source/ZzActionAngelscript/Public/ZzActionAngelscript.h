// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FZzActionAngelscriptModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
