// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomSettingToolbarButtom.h"
#include "..\Public\CustomSettingStyle.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "ISettingsViewer.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "GameMapsSettings.h"
#include "ISettingsEditorModel.h"
#include "ISettingsEditorModule.h"
#include "GameFramework/InputSettings.h"
#include "Runtime/Launch/Resources/Version.h"


#define LOCTEXT_NAMESPACE "FCustomSettingModule"


static  FName s_CustomSettingTabName("CustomSetting");
static  FName s_containerName = TEXT("Custom");

class FCustomSettingViewerModule
	: public IModuleInterface
	, public ISettingsViewer
{
public:

	// ISettingsViewer interface
	virtual void ShowSettings(const FName& CategoryName, const FName& SectionName)override
	{
		FGlobalTabmanager::Get()->TryInvokeTab(s_CustomSettingTabName);
		ISettingsEditorModelPtr SettingsEditorModel = m_settingsEditorModel.Pin();

		if (SettingsEditorModel.IsValid())
		{
			ISettingsCategoryPtr Category = SettingsEditorModel->GetSettingsContainer()->GetCategory(CategoryName);

			if (Category.IsValid())
			{
				SettingsEditorModel->SelectSection(Category->GetSection(SectionName));
			}
		}
	}


public:
	void RegisteToolbarButton()
	{
		
		
		
	}

	// IModuleInterface interface

	virtual void StartupModule() override
	{
		FCustomSettingStyle::Initialize();
		FCustomSettingStyle::ReloadTextures();
		
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([this]() {
			auto OpenView = []( FName ContainerName, FName CategoryName, FName SectionName )
			{
				FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").
				ShowViewer(ContainerName, CategoryName, SectionName);
			};
			FToolMenuOwnerScoped OwnerScoped(this);
			{
				UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Edit");
				{
					FToolMenuSection& Section = Menu->FindOrAddSection("EditLocalTabSpawners");
				
					Section.AddMenuEntry(
						s_CustomSettingTabName,
						FText::FromName(s_CustomSettingTabName),
						{},
						FSlateIcon(FCustomSettingStyle::GetStyleSetName(), "CustomSetting.Icon"),
						FUIAction(FExecuteAction::CreateStatic(OpenView,
							s_containerName, FName("Project"), FName("Maps")))
					);
				}

#if (ENGINE_MAJOR_VERSION) == 5
				UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
				{
					//寻找一个叫做CustomSetting的ToolMenuSection。如果找不到则创建一个
					FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("CustomSetting");
					{
						Section.AddEntry(
								FToolMenuEntry::InitToolBarButton(
								s_CustomSettingTabName,
								FUIAction(FExecuteAction::CreateStatic(
								OpenView,
								s_containerName, FName("Project"), FName("Maps"))),
								FText::FromName(s_CustomSettingTabName),
								{},
								FSlateIcon(FCustomSettingStyle::GetStyleSetName(), "CustomSetting.Icon")
							)
						);
					}
				}
#elif (ENGINE_MAJOR_VERSION) == 4
				FCustomSettingButtonCommands::Register();
				PluginCommands = MakeShareable(new FUICommandList);

				PluginCommands->MapAction(FCustomSettingButtonCommands::Get().PluginAction,
					FExecuteAction::CreateStatic(OpenView, s_containerName, FName("Project"), FName("Maps")),
					FCanExecuteAction());


				FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

						{
							TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
							ToolbarExtender->AddToolBarExtension("Settings",
								EExtensionHook::After,
								PluginCommands,
								FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& Builder)
									{
										Builder.AddToolBarButton(FCustomSettingButtonCommands::Get().PluginAction);
									}	
								)
							);

							LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
						}

			
#endif
			}
		}));

		// 
		ISettingsModule* settingModlule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (settingModlule)
		{
			settingModlule->RegisterSettings(s_containerName, "Project", "Maps",
				FText::FromString("Maps & Modes"),
				FText::FromString("Default maps, game modes and other map related settings."),
				GetMutableDefault<UGameMapsSettings>()
			);

			settingModlule->RegisterViewer(s_containerName, *this);
		}

		auto _handleSpawnTab = [this](const FSpawnTabArgs& SpawnTabArgs)->TSharedRef<SDockTab> {

			TSharedRef<SWidget> SettingsEditor = SNullWidget::NullWidget;
			ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

			if (SettingsModule)
			{
				ISettingsEditorModule& SettingsEditorModule = FModuleManager::GetModuleChecked<ISettingsEditorModule>("SettingsEditor");
				ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer(s_containerName);
				if (SettingsContainer.IsValid())
				{
					ISettingsEditorModelRef SettingsEditorModel = SettingsEditorModule.CreateModel(SettingsContainer.ToSharedRef());
					m_settingsEditorModel = SettingsEditorModel;
					SettingsEditor = SettingsEditorModule.CreateEditor(SettingsEditorModel);
				}
			}

			return SNew(SDockTab)
				.TabRole(ETabRole::NomadTab)
				[
					SettingsEditor
				];
		};

		// 注册edit列表的
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			s_CustomSettingTabName,
			FOnSpawnTab::CreateLambda(_handleSpawnTab)
		)
			.SetDisplayName(FText::FromString(TEXT("自定义配置")))
			.SetMenuType(ETabSpawnerMenuType::Hidden)
			.SetIcon(FSlateIcon(FCustomSettingStyle::GetStyleSetName(), "CustomSetting.TitleIcon"));

		// 注册Toolbar的
		
	}

	virtual void ShutdownModule() override
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(s_CustomSettingTabName);
	}


	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

private:

	TWeakPtr<ISettingsEditorModel> m_settingsEditorModel;

	TSharedPtr<class FUICommandList> PluginCommands;

	
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomSettingViewerModule, CustomSetting)