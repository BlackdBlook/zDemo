#include "LogManager.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(HYS_LOG)

UZLogManager* UZLogManager::instance = nullptr;

FString UZLogManager::getTitleByType(LogType type)
{
	switch (type)
	{
	case Info:
		return "[Info]";
	case Warning:
		return "[Warning]";
	case Error:
		return "[Error]";
	default:
		break;
	}
	return "";
}

FColor UZLogManager::getColorByType(LogType log)
{
	switch (log)
	{
	case Info:
		return FColor::Green;
	case Warning:
		return FColor::Yellow;
	case Error:
		return FColor::Red;
	default:
		break;
	}

	return FColor::Black;
}

void UZLogManager::LogPrintf(LogType type, FString logStr)
{
	if (GEngine)
	{	
		GEngine->AddOnScreenDebugMessage(-1, 10, this->getColorByType(type), *logStr);
	}

	if (type == LogType::Info)
	{
		UE_LOG(HYS_LOG, Display, TEXT("%s"), *logStr);
	}
	else if (type == LogType::Error)
	{
		UE_LOG(HYS_LOG, Error, TEXT("%s"), *logStr);
	}
	else if (type == LogType::Warning)
	{
		UE_LOG(HYS_LOG, Warning, TEXT("%s"), *logStr);
	}
}

void UZLogManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	instance = this;
}
