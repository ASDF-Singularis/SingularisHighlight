#pragma once

#include "Modules/ModuleManager.h"

class FSingularisHighlightModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
