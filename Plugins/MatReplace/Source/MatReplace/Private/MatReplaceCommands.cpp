// Copyright Epic Games, Inc. All Rights Reserved.

#include "MatReplaceCommands.h"

#define LOCTEXT_NAMESPACE "FMatReplaceModule"

void FMatReplaceCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MatReplace", "Execute MatReplace action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
