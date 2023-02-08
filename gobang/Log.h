#pragma once
#include<stdarg.h>
#include<string>
#include<Windows.h>
#include<mutex>
#include<map>
#include<fstream>
#include<iostream>
inline std::string Format(const char* Format, ...) {
	va_list Args = nullptr;
	va_start(Args, Format);
	int Len = _vscprintf(Format, Args) + 1;
	std::string Buffer(Len, 0);
	vsprintf_s(&Buffer[0], Len, Format, Args);
	if (*Args) va_end(Args);
	return Buffer;
}
enum class LogType :BYTE { Info, Warning, Error, Fatal };//日志级别
namespace ulog {
	static std::mutex g_mutex;
	static auto CurrentLogLevel = LogType::Info;
	void SetLogLevel(LogType minType) {
		CurrentLogLevel = minType;
	}
	inline void Log(LogType _type, const char* szFunctionName, int _Line, const char* szFormat, ...) {
		SYSTEMTIME st;
		GetLocalTime(&st);
		auto LogTime = Format("%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
		va_list marker = nullptr;
		va_start(marker, szFormat);
		auto num_of_chars = _vscprintf(szFormat, marker);
		auto szBuffer = std::make_unique<char[]>(static_cast<size_t>(num_of_chars) + 1);
		vsprintf_s(szBuffer.get(), static_cast<size_t>(num_of_chars) + 1, szFormat, marker);
		if (*marker)va_end(marker);
		static std::map<LogType, std::string>Type2Str;
		static std::once_flag flag;
		std::call_once(flag, [&]() {
			Type2Str.insert({ LogType::Info,"Info" });
		Type2Str.insert({ LogType::Warning,"Warning" });
		Type2Str.insert({ LogType::Error,"Error" });
		Type2Str.insert({ LogType::Fatal,"Fatal" });
			});
		if (_type >= CurrentLogLevel) {//过滤低level的log
			auto szLog = Format("[%s][%s][%s<%d>]%s", LogTime.c_str(), Type2Str[_type].c_str(), szFunctionName, _Line, szBuffer.get());
#if defined(_DBG_CONSOLER_PRINT)
			std::cout << szLog;
#endif
			std::lock_guard<std::mutex> guard(g_mutex);
			OutputDebugStringA(szLog.c_str());
		}
	}
#if !defined(_NO_LOG_)
#define LogOut(type,_message,...)Log(type,__FUNCTION__,__LINE__,_message,##__VA_ARGS__)
#else
#define SimpleLogOut(type,_message,...)
#endif
#define LogInfo(_message,...)		   LogOut(LogType::Info   ,_message,##__VA_ARGS__)
#define LogWarning(_message,...)	   LogOut(LogType::Warning,_message,##__VA_ARGS__)
#define LogError(_message,...)		   LogOut(LogType::Error  ,_message,##__VA_ARGS__)
#define LogFatalError(_message,...)	   LogOut(LogType::Fatal  ,_message,##__VA_ARGS__
#define LogIf(_condition,_message,...) if(_condition)LogOut(LogType::Info,_message,##__VA_ARGS__)//条件输出
#if defined(_DEBUG)
#define DbgLog(_message,...)		   SimpleLogOut(LogType::Info	,_message,##__VA_ARGS__)
#else
#define DbgLog(_msg,...)
#endif
}
using namespace ulog;

inline void InitConsoler() {
	
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$","w+t", stdout);
	freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$","r", stderr);
	
}