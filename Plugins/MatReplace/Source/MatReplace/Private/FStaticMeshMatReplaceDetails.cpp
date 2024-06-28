#include "FStaticMeshMatReplaceDetails.h"

#include "Modules/ModuleManager.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SComboButton.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "LevelEditor.h"
#include "LevelEditorActions.h"

TSharedRef<IDetailCustomization> FStaticMeshMatReplaceDetails::MakeInstance()
{
	return MakeShared<FStaticMeshMatReplaceDetails>();
}

void FStaticMeshMatReplaceDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//返回指定目录的构造对象。第一个参数用于查找指定目录（必要时也可以作为目录名），
	//第二个参数为显示目录名（可选），
	//第三个参数为优先级，下面设置的是Important，所以会在顶层显示此目录。
	IDetailCategoryBuilder& CustCategory = DetailBuilder.EditCategory(
		FName("CustCategory"),
		FText::FromString("CustCategory"),
		ECategoryPriority::Important);
	//为此目录添加一行记录。第一个参数为用于过滤（搜索）的文本，第二个参数是是否为高级项。
	CustCategory
	.AddCustomRow(FText::FromString("CustRow"), false)
	.NameContent()	//NameContent是属性名的显示内容
	[
		SNew(STextBlock).Text(FText::FromString("TextBlock"))
	]
	.ValueContent()
	[	//ValueContent是值的显示内容。如果不想区分Key和Value分割的话，可以直接使用WholeRowContent，下面有效果。
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.AutoHeight()
		[
			SNew(SCheckBox)
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.AutoHeight()
		[
			SNew(SEditableTextBox)
		]
	];
}
