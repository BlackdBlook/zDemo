// Copyright Epic Games, Inc. All Rights Reserved.

#include "MatReplace.h"
#include "MatReplaceStyle.h"
#include "MatReplaceCommands.h"
#include "SMRDetailView.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName MatReplaceTabName("MatReplace");

#define LOCTEXT_NAMESPACE "FMatReplaceModule"

TSharedRef<SDockTab> FMatReplaceModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SMRDetailView)
			]
		];
}

void FMatReplaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMatReplaceStyle::Initialize();
	FMatReplaceStyle::ReloadTextures();

	FMatReplaceCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);
	
	PluginCommands->MapAction(
		FMatReplaceCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMatReplaceModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMatReplaceModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MatReplaceTabName, FOnSpawnTab::CreateRaw(this, &FMatReplaceModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("FMatReplaceModule", "MatReplace"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMatReplaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMatReplaceStyle::Shutdown();

	FMatReplaceCommands::Unregister();
}

void FMatReplaceModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MatReplaceTabName);
}

void FMatReplaceModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMatReplaceCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMatReplaceCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMatReplaceModule, MatReplace)