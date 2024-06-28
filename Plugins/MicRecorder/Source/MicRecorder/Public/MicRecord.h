// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#ifdef PLATFORM_WINDOWS
#include "MicRecorder/Windows/AudioRecordWindows.h"
using AudioRecorder = AudioRecordSpace::AudioRecordWindows;
#elif PLATFORM_LINUX
static_assert(false, TEXT("Linux平台重载有待开发"));
#endif

#include "MicRecord.generated.h"



DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFlushToFileFinish, bool, Success);
/**
 * 
 */
UCLASS(BlueprintType)
class MICRECORDER_API UMicRecord : public UObject
{
	GENERATED_BODY()
public:
	UMicRecord();
	
	UFUNCTION(BlueprintCallable)
	static TArray<FString> GetAllAudioInputDevices();
	
	UFUNCTION(BlueprintCallable)
	void StartRecord(int32 DeviceId); 

	UFUNCTION(BlueprintCallable)
	void StopRecord();

	UFUNCTION(BlueprintCallable)
	void BeginFlushToFile(const FString& FilePath,FOnFlushToFileFinish Callback);

	UFUNCTION(BlueprintCallable)
	void Reset();
	
	using DataBlock = std::array <char,AUDIO_DATA_BLOCK_SIZE>;
	DECLARE_MULTICAST_DELEGATE_TwoParams(FRecoderCallback, DataBlock, RecordStatus);
	FRecoderCallback RecoderCallback;

	size_t GetDataBuffer(uint8** Data);
	
private:

	AudioRecorder Recorder;
	
	bool Recording = false;
};
