#pragma once
#include "CustomSettingStyle.h"
#include "Framework/Commands/Commands.h"

class FCustomSettingButtonCommands : public TCommands<FCustomSettingButtonCommands>
{
public:
	FCustomSettingButtonCommands()
		: TCommands<FCustomSettingButtonCommands>(
			  TEXT("CustomSetting"),
			  FText::FromString(TEXT("CustomSetting")),
			  NAME_None,
			  FCustomSettingStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> PluginAction;
};
