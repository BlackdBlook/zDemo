// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomSettingBase.generated.h"


UCLASS(DefaultConfig, Abstract)
class CUSTOMSETTINGBASEMODULE_API UCustomSettingBase : public UDeveloperSettings
{
	GENERATED_BODY()

	virtual FName GetCategoryName()  const override
	{
		return "CustomSetting";
	}

	virtual FName GetContainerName() const override
	{
		return "Custom";
	}
};

UCLASS(Config="CustomConfig", DefaultConfig)
class CUSTOMSETTINGBASEMODULE_API UCustomSettingConfig : public UCustomSettingBase
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
	FString SampleString;
	
	// C++获取实例
	static UCustomSettingConfig* GetInstance()
	{
		return GetMutableDefault<UCustomSettingConfig>();
	}

	// 蓝图获取实例
	UFUNCTION(BlueprintPure, BlueprintCallable, DisplayName="GetCustomSettingConfigInstance")
	static UCustomSettingConfig* GetInstanceBP()
	{
		return GetInstance();
	}

};

