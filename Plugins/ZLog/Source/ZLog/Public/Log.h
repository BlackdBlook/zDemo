#pragma once

#include "CoreMinimal.h"
#include "LogManager.h"


class LogManagerHelper
{
public:
	template<typename ...T>
	static void Info(const char* NameSpace,const char* fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addInfo(UTF8_TO_TCHAR(NameSpace), UTF8_TO_TCHAR(fmt), args...);
	}

	template<typename ...T>
	static void Warning(const char* NameSpace,const char* fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addWarning(UTF8_TO_TCHAR(NameSpace), UTF8_TO_TCHAR(fmt), args...);
	}
	
	template<typename ...T>
	static void Error(const char* NameSpace,const char* fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addError(UTF8_TO_TCHAR(NameSpace), UTF8_TO_TCHAR(fmt), args...);
	}


	template<typename ...T>
	static void Info(const FString& NameSpace,const FString& fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addInfo(NameSpace, fmt, args...);
	}

	template<typename ...T>
	static void Warning(const FString& NameSpace,const FString& fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addWarning(NameSpace, fmt, args...);
	}
	
	template<typename ...T>
	static void Error(const FString& NameSpace,const FString& fmt, T... args)
	{
		if(UZLogManager::instance)
			UZLogManager::instance->addError(NameSpace, fmt, args...);
	}

};



//信息
#define LOG_Info(nameSpace,fmt,...) \
LogManagerHelper::Info(nameSpace,fmt,##__VA_ARGS__)

//警告
#define LOG_Warning(nameSpace,fmt,...) \
LogManagerHelper::Warning(nameSpace,fmt,##__VA_ARGS__)

//错误
#define LOG_Error(nameSpace,fmt,...) \
LogManagerHelper::Error(nameSpace,fmt,##__VA_ARGS__)