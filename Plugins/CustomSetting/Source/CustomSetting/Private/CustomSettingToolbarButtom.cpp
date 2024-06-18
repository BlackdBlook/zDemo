#include "CustomSettingToolbarButtom.h"

#define LOCTEXT_NAMESPACE "FCustomSettingButtonCommands"
void FCustomSettingButtonCommands::RegisterCommands()
{
	UI_COMMAND(
		PluginAction, "CustomSetting",
		"CustomSetting",
		EUserInterfaceActionType::Button, FInputChord());
}
#undef LOCTEXT_NAMESPACE
