
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <string>
#include <typeindex>

namespace Log
{
	//检查是否有 toString 这个成员函数
	template<typename T>
	class CheckEnable_toString
	{
	private:
		template<typename U> static auto checkFunc(int) ->
			decltype(std::declval<U>().toString(), std::true_type());

		template<typename U> static std::false_type checkFunc(...);
	public:
		enum {
			value = std::is_same<decltype(checkFunc<T>(0)),
			std::true_type>::value
		};
	};

	//检查是否有 ToString 这个成员函数 Ue4 命名风格  ....
	template<typename T>
	class CheckEnable_ToString
	{
	private:
		template<typename U> static auto checkFunc(int) ->
			decltype(std::declval<U>().ToString(), std::true_type());

		template<typename U> static std::false_type checkFunc(...);
	public:
		enum {
			value = std::is_same<decltype(checkFunc<T>(0)),
			std::true_type>::value
		};
	};


	using namespace std;

	template<typename T, typename enable = void>
	struct ValueConvert
	{
		static FString convert(const T& v)
		{
			return "None";
		}
	};

	//非bool 的整数
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_integral<T>::value && !is_same<T, bool>::value>::type>
	{
		static FString convert(const T& v)
		{
			return FString::FromInt(v);
		}
	};

	//bool
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_same<T, bool>::value>::type>
	{
		static FString convert(const T& v)
		{
			return v ? "true" : "false";
		}
	};

	//浮点
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_floating_point<T>::value>::type>
	{
		static FString convert(const T& v)
		{
			float value = (float)v;
			return FString::SanitizeFloat(value);
		}
	};

	//c-style 字符串
	using CStyleStr = const char*;
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_same<T, CStyleStr>::value>::type>
	{
		static FString convert(const T& v)
		{
			return FString(UTF8_TO_TCHAR(v));
		}
	};

	//std::string
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_same<T, std::string>::value>::type>
	{
		static FString convert(const T& v)
		{
			return FString(UTF8_TO_TCHAR(v.c_str()));
		}
	};

	//FString
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_same<T, FString>::value>::type>
	{
		static FString convert(const T& v)
		{
			return v;
		}
	};

	//TChar*
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_same<T, FString::ElementType*>::value>::type>
	{
		static FString convert(const T& v)
		{
			return FString(v);
		}
	};

	//支持转换的类对象 ToString
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_class<T>::value&&CheckEnable_ToString<T>::value>::type>
	{
		static FString convert(T& v) { return v.ToString(); }
	};

	//支持转换的类对象 toString
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_class<T>::value&&CheckEnable_toString<T>::value>::type>
	{
		static FString convert(T& v) { return v.toString(); }
	};

	//指针类型
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_pointer<T>::value && !is_same<T, FString::ElementType*>::value && !is_same<T, CStyleStr>::value>::type>
	{
		static FString convert(T& v)
		{
			return ValueConvert<typename remove_pointer<T>::type>::convert(*v);
		}
	};

	//枚举
	template<typename T>
	struct ValueConvert<T, typename enable_if<is_enum<T>::value>::type>
	{
		static FString convert(T& v)
		{
			return FString::FromInt((int32)v);
		}
	};

	template<typename T>
	FString convert(const T& v)
	{
		using type = T;
		return ValueConvert<T>::convert(const_cast<T&>(v));
	}
};

struct LogString
{
	template<typename ...T>
	static FString makeString(const FString& fmt, T... args)
	{

		TArray<FStringFormatArg> argStr; 
		(void)std::initializer_list<FString>
		{
			([&args,&argStr] {

				argStr.Add(Log::convert(args));
			}(), fmt)...
		};

		return FString::Format(*fmt, argStr);
	}
};