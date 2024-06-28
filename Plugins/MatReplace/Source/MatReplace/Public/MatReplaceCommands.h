// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MatReplaceStyle.h"

class FMatReplaceCommands : public TCommands<FMatReplaceCommands>
{
public:

	FMatReplaceCommands()
		: TCommands<FMatReplaceCommands>(TEXT("MatReplace"), NSLOCTEXT("Contexts", "MatReplace", "MatReplace Plugin"), NAME_None, FMatReplaceStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
