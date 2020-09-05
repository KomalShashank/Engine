#pragma once

#include "Engine/DataStructures/ThreadSafeQueue.hpp"
#include "Engine/DebugTools/MemoryAnalytics/CallStack.hpp"
#include "Engine/IO Utilities/BinaryFileIO.hpp"
#include "Engine/Threading/Thread.hpp"
#include "Engine/Time/Time.hpp"



enum LogLevel
{
	LOG_NONE,
	LOG_DEFAULT,
	LOG_RECOVERABLE,
	LOG_SEVERE
};



class LogMessage
{
public:
	LogMessage(const char* messageString);
	LogMessage(const char* messageString, const TimeStampData& messageTimeStamp, const LogLevel& logLevel, const char* messageTag, CallStack* currentCallStack = nullptr);

	std::string GetLogMessageText();

public:
	bool m_SimpleForm;
	char m_MessageString[2048];
	TimeStampData m_MessageTimeStamp;

	LogLevel m_LogLevel;
	char m_MessageTag[2048];

	CallStack* m_CurrentCallStack;
};



class LoggerSystem
{
private:
	LoggerSystem();

public:
	static void InitializeLoggerSystem(const char* loggerName);
	static void UninitializeLoggerSystem();

	void FlushLogger();

public:
	TS_Queue<LogMessage*> m_AllLogMessages;
	Thread* m_LoggerThread;

	const char* m_LoggerName;
	bool m_IsRunning;
	bool m_FlushLogs;
};

extern LoggerSystem* g_LoggerSystem;



void MessageLoggingThread(void* messageLogger);
void HandleMessagesInQueue(LoggerSystem* loggerSystem, BinaryFileWriter& fileWriter);
void MakeDefaultFileCopy(LoggerSystem* loggerSystem, const char* fileName);

void PrintToLog(const char* messageFormat, ...);
void PrintToLogSimple(const char* messageFormat, ...);
void PrintToLogWithTag(const char* messageTag, const char* messageFormat, ...);
void PrintToLogWithLogLevel(const LogLevel& logLevel, const char* messageFormat, ...);
void PrintToLogWithCallStack(CallStack* currentCallStack, const char* messageFormat, ...);
void PrintToLogWithEverything(const char* messageTag, const LogLevel& logLevel, CallStack* currentCallStack, const char* messageFormat, ...);