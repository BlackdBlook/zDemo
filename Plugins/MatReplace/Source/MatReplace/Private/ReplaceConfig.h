// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityObject.h"
#include "Editor/PropertyEditorTestObject.h"
#include "ReplaceConfig.generated.h"

UCLASS(Config=Editor,Blueprintable,BlueprintType)
class MATREPLACE_API UReplaceTypeDefault : public UEditorUtilityObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	TArray<UMaterialInstanceConstant*> CreateMaterialInstance(
		UMaterialInterface* Parent, int32 Number, const FString& SavePath, const FString& MeshName);
	
	UFUNCTION(BlueprintCallable)
	void SetMaterialInstanceStaticSwitchParameterValue(UMaterialInstanceConstant* Mat, FName Name, bool NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void OnChangeTargerMesh();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	TArray<FString> GenMatInstanceName(const FString& MeshName, int32 Num);
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FString GenTextureSearchPath(const FString& RootSearchPath, const FString& FileName);

	UFUNCTION(BlueprintImplementableEvent)
	void PostPropertyEdit(const FName& PropertyName);
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};

DECLARE_DELEGATE_OneParam(FOnChangeReplaceType, TSubclassOf<UReplaceTypeDefault>);

UCLASS(Config=Editor, DefaultConfig)
class MATREPLACE_API UReplaceConfig : public UObject
{
	GENERATED_BODY()
public:

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, DisplayName="ReplaceDataType", Config)
	TSubclassOf<UReplaceTypeDefault> DataType = UReplaceTypeDefault::StaticClass();

	FOnChangeReplaceType OnChangeReplaceType;
};


