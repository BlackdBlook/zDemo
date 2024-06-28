#include "SMRDetailView.h"

#include "FStaticMeshMatReplaceDetails.h"
#include "ReplaceConfig.h"
#include "Widgets/Layout/SScrollBox.h"

void SMRDetailView::Construct(const FArguments& Args)
{
	auto& prEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	RepType = UReplaceConfig::StaticClass()->GetDefaultObject<UReplaceConfig>();

	DefaultType = RepType->DataType->GetDefaultObject<UReplaceTypeDefault>();

	RepType->OnChangeReplaceType.BindLambda([this](TSubclassOf<UReplaceTypeDefault> newType)
	{
		ChangeType(newType);
	});
	
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;
	
	{
		TypePanel = prEditor.CreateDetailView(DetailsViewArgs);
		TypePanel->SetObject(
			RepType,
			true);
	}

	{
		ConfigPanel = prEditor.CreateDetailView(DetailsViewArgs);
		ConfigPanel->SetObject(
			RepType->DataType->GetDefaultObject<UReplaceTypeDefault>(),
			true);
		
		ConfigPanel->SetOnObjectArrayChanged(FOnObjectArrayChanged::CreateLambda(
			[this](const FString& str, const TArray<UObject*>& objs)
			{
				if(objs.Num() == 0)
				{
					DefaultType = nullptr;
				}
				auto temp = Cast<UReplaceTypeDefault>(objs[0]);
				DefaultType = temp;
			}));
	}

	

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				TypePanel.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				ConfigPanel.ToSharedRef()
			]
			
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Bottom)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("确认")))
				.OnClicked_Lambda([this]()
				{
					DefaultType->Run();
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("刷新")))
				.OnClicked(FOnClicked::CreateLambda([this]()->FReply
				{
					// ChangeType(RepType->DataType);
					DefaultType->OnChangeTargerMesh();
					return FReply::Handled();
				}))
			]
		]
	];

	// FPropertyEditorModule& PropertyEditorModule =
	// FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//
	// //注册绑定
	// PropertyEditorModule.RegisterCustomClassLayout("ReplaceTypeDefault",
	// 	FOnGetDetailCustomizationInstance::CreateStatic(
	// 		&FStaticMeshMatReplaceDetails::MakeInstance
	// 	));
	//
	// //通知自定义模块修改完成
	// PropertyEditorModule.NotifyCustomizationModuleChanged();

	// //模块卸载时，记得Unregister
	// PropertyEditorModule.UnregisterCustomPropertyTypeLayout("CustObject");
}

void SMRDetailView::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(RepType);
	Collector.AddReferencedObject(DefaultType);
}


void SMRDetailView::ChangeType(TSubclassOf<UReplaceTypeDefault> NewType)
{
	if(NewType == nullptr)
	{
		return;
	}
	
	auto temp = NewType->GetDefaultObject<UReplaceTypeDefault>();
	ConfigPanel->SetObject(temp,true);
}

FString SMRDetailView::GetReferencerName() const
{
	return "SMRDetailView";
}
