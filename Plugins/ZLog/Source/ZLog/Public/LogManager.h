

#pragma once

#include "CoreMinimal.h"
#include "LogStringBuilder.h"
#include "LogManager.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(HYS_LOG, Log, All)



/**
 * 
 */
UCLASS()
class ZLOG_API UZLogManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	enum LogType : uint8
	{
		Info,
		Warning,
		Error
	};
	
	
public:
	
	static UZLogManager* instance;

	// 添加日志 - 信息
	template<typename... T>
	void addInfo(const FString& nameSpace, const FString& fmt, T... args)
	{
		this->addLog(LogType::Info, nameSpace, fmt, args...);
	}

	// 添加日志 - 警告 
	template<typename... T>
	void addWarning(const FString& nameSpace, const FString& fmt, T... args)
	{
		this->addLog(LogType::Warning, nameSpace, fmt, args...);
	}

	// 添加日志 - 错误 
	template<typename... T>
	void addError(const FString& nameSpace, const FString& fmt, T... args)
	{
		this->addLog(LogType::Error, nameSpace, fmt, args...);
	}
	
	template<typename... T>
	void addLog(LogType type, const FString& nameSpace, const FString& fmt, T... args)
	{
		FString logStr;
		logStr += getTitleByType(type);
		logStr += " <" + nameSpace + "> ";
		logStr += makeString(fmt, args...);

		if (!IsInGameThread())
		{
			AsyncTask(ENamedThreads::GameThread, [this, type, logStr]() {
				LogPrintf(type, logStr); 
			});
		}
		else
		{
			LogPrintf(type, logStr);
		}
		
	}
private:
	
	FString getTitleByType(LogType type);

	FColor getColorByType(LogType log);
	
	void LogPrintf(LogType type, FString logStr);

	template<typename ...T>
	static FString makeString(const FString& fmt, T... args)
	{
		return LogString::makeString(fmt,args...);
	}

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};




