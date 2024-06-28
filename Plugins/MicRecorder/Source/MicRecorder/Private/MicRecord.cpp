// Fill out your copyright notice in the Description page of Project Settings.


#include "MicRecord.h"

void Callback(std::array <char, AUDIO_DATA_BLOCK_SIZE> audioDataBlock, RecordStatus recordStatus)
{
	if (recordStatus == RecordStatus::OpenDevice)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan
			, TEXT("回调 打开设备"));
	}
	else if (recordStatus == RecordStatus::RecordStart)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan
				, TEXT("回调 开始录音"));
	}
	else if (recordStatus == RecordStatus::RecordWriteData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan
        			, TEXT("回调 正在写入数据"));
	}
	else if (recordStatus == RecordStatus::RecordStop)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan
			, TEXT("回调 停止录音"));
	}
	else if (recordStatus == RecordStatus::CloseDevice)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan
			, TEXT("回调 关闭设备"));
	}
}

UMicRecord::UMicRecord()
{
	Recorder.RegisterCallback(Callback);
}

TArray<FString> UMicRecord::GetAllAudioInputDevices()
{
	TArray<FString> Ans;
	AudioRecorder::GetAllAudioDevice(Ans);
	return Ans;
}

void UMicRecord::StartRecord(int32 DeviceId)
{
	Recorder.OpenAudioDevice(DeviceId); 
	Recorder.StartRecord();
}

void UMicRecord::StopRecord()
{
	Recorder.StopRecord();
	
	Recorder.CloseAudioDevice();
	// Recorder.ResetRecord();	
}

void UMicRecord::BeginFlushToFile(const FString& FilePath, FOnFlushToFileFinish Callback)
{
	AsyncTask(ENamedThreads::HighTaskPriority, [this, FilePath, Callback]()
	{
		Recorder.WriteWavFile(*FilePath);
		
		AsyncTask(ENamedThreads::GameThread, [Callback]()
		{
			Callback.Execute(true);
		});
	});
}

void UMicRecord::Reset()
{
	Recorder.ResetRecord();
	Recording = false;
}

size_t UMicRecord::GetDataBuffer(uint8** Data)
{
	return 0;
}
