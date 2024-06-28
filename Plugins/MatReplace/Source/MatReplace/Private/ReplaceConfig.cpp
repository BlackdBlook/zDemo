// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplaceConfig.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"

TArray<UMaterialInstanceConstant*> UReplaceTypeDefault::CreateMaterialInstance(
	UMaterialInterface* Parent, int32 Number, const FString& SavePath, const FString& MeshName)
{
	const FString DefaultSuffix = TEXT("_Inst");
	TArray<UMaterialInstanceConstant*> ObjectsToSync;

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();

	bool PathChecked = false;
	
	for(int i = 0;i < Number;i++)
	{
		auto Object = Parent;
		if ( Object )
		{
			// Determine an appropriate name
			FString Name;
			FString PackageName;
			FString ObjName;
			if(SavePath.IsEmpty())
			{
				ObjName = Object->GetPackage()->GetName();
			}else
			{
				ObjName = SavePath;
				if(!SavePath.EndsWith(TEXT("/")))
				{
					ObjName += '/';
				}

				if(PathChecked == false)
				{
					FText* out = nullptr;
				
					if(!FPaths::ValidatePath(ObjName, out))
					{
						if(out != nullptr)
						{
							const TCHAR* outChar = *(out->ToString());
							UE_LOG(LogTemp, Error, TEXT("%s"), outChar);
						}
						return {};
					}
				}

				PathChecked = true;
				
				ObjName += FString::Printf(TEXT("MI_%s_%d"), *MeshName, i);
			}
			AssetToolsModule.Get().CreateUniqueAssetName(ObjName,
				DefaultSuffix, PackageName, Name);

			// Create the factory used to generate the asset
			Factory->InitialParent = Object;

			UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackageName), UMaterialInstanceConstant::StaticClass(), Factory);

			if ( NewAsset )
			{
				ObjectsToSync.Add((UMaterialInstanceConstant*)NewAsset);
			}
		}
	}
	

	if ( ObjectsToSync.Num() > 0 )
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(reinterpret_cast<TArray<UObject*>&>(ObjectsToSync));
	}

	return ObjectsToSync;
}

TArray<FString> UReplaceTypeDefault::GenMatInstanceName(const FString& MeshName, int32 Num)
{
	TArray<FString> ans;

	for(int32 i = 0;i < Num;i++)
	{
		FString name = FString::Printf(TEXT("MI_%s_%d"), *MeshName, Num);
		ans.Add(MoveTemp(name));
	}
	
	return ans;
}

void UReplaceTypeDefault::SetMaterialInstanceStaticSwitchParameterValue(UMaterialInstanceConstant* Mat, FName Name,
	bool NewValue)
{
	TArray<FGuid> Guids;
	TArray<FMaterialParameterInfo> OutParameterInfo;
	Mat->GetAllStaticSwitchParameterInfo(OutParameterInfo, Guids);
	FStaticParameterSet StaticParameters = Mat->GetStaticParameters();

	for (int32 ParameterIdx = 0; ParameterIdx < OutParameterInfo.Num(); ParameterIdx++)
	{
		const FMaterialParameterInfo& ParameterInfo = OutParameterInfo[ParameterIdx];
		const FGuid ExpressionId = Guids[ParameterIdx];
		if (ParameterInfo.Name == Name)
		{
			FStaticSwitchParameter* NewParameter =new (StaticParameters.StaticSwitchParameters)
				FStaticSwitchParameter(ParameterInfo, NewValue, true, ExpressionId);
			break;
		}
	}
	Mat->UpdateStaticPermutation(StaticParameters);
}

FString UReplaceTypeDefault::GenTextureSearchPath(const FString& RootSearchPath, const FString& FileName)
{
	FString ans;

	FString root;
	if(RootSearchPath.EndsWith("/"))
	{
		root = RootSearchPath.Mid(0, RootSearchPath.Len() - 1);
	}else
	{
		root = RootSearchPath;
	}
	
	ans = FString::Printf(TEXT("Texture2D'%s/%s.%s'"),
		*root, *FileName, *FileName);
	
	return ans;
}

void UReplaceTypeDefault::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(PropertyChangedEvent.Property == nullptr)
	{
		return;
	}
	
	if(PropertyChangedEvent.Property->GetFName() == FName("TargetStaticMesh"))
	{
		OnChangeTargerMesh();
	}

	PostPropertyEdit(PropertyChangedEvent.MemberProperty->GetFName());

	if(PropertyChangedEvent.Property->HasAnyPropertyFlags(CPF_Config | CPF_GlobalConfig))
	{
		SaveConfig();
	}
}

void UReplaceConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	check(PropertyChangedEvent.Property);

	if(PropertyChangedEvent.Property->GetName() == "DataType")
	{
		OnChangeReplaceType.Execute(DataType);
	}
	
	if(PropertyChangedEvent.Property->HasAnyPropertyFlags(
		EPropertyFlags::CPF_Config | EPropertyFlags::CPF_GlobalConfig))
	{
		SaveConfig();
	}
}
