#include "Engine/DebugTools/LoggerSystem/LoggerSystem.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"

#include <stdarg.h>



LoggerSystem* g_LoggerSystem = nullptr;



LogMessage::LogMessage(const char* messageString) :
m_SimpleForm(true),
m_CurrentCallStack(nullptr)
{
	strncpy_s(m_MessageString, messageString, 2048);
}



LogMessage::LogMessage(const char* messageString, const TimeStampData& messageTimeStamp, const LogLevel& logLevel, const char* messageTag, CallStack* currentCallStack /*= nullptr*/) :
m_SimpleForm(false),
m_MessageTimeStamp(messageTimeStamp),
m_LogLevel(logLevel),
m_CurrentCallStack(currentCallStack)
{
	strncpy_s(m_MessageString, messageString, 2048);
	strncpy_s(m_MessageTag, messageTag, 2048);
}



std::string LogMessage::GetLogMessageText()
{
	std::string messageText;
	
	if (m_SimpleForm)
	{
		messageText = Stringf("%s", m_MessageString);
	}
	else
	{
		const char* logLevelString;
		switch (m_LogLevel)
		{
		case LOG_NONE:
			logLevelString = "None";
			break;

		case LOG_DEFAULT:
			logLevelString = "Default";
			break;

		case LOG_RECOVERABLE:
			logLevelString = "Recoverable";
			break;

		case LOG_SEVERE:
			logLevelString = "Severe";
			break;

		default:
			logLevelString = "";
			break;
		}

		messageText = Stringf("%04i-%02i-%02i %02i.%02i.%02i: %s (Tag: %s, Level: %s)\n",
			m_MessageTimeStamp.m_Year,
			m_MessageTimeStamp.m_Month,
			m_MessageTimeStamp.m_Day,
			m_MessageTimeStamp.m_Hour,
			m_MessageTimeStamp.m_Minute,
			m_MessageTimeStamp.m_Second,
			m_MessageString,
			m_MessageTag,
			logLevelString);
	}

	CallStackLine* currentCallStackLines = nullptr;
	if (m_CurrentCallStack != nullptr)
	{
		currentCallStackLines = GetCallStackLines(m_CurrentCallStack);
	}

	if (currentCallStackLines != nullptr)
	{
		messageText += "\n";
		
		for (size_t callStackLineIndex = 0; callStackLineIndex < m_CurrentCallStack->m_NumberOfCallStackFrames; ++callStackLineIndex)
		{
			CallStackLine currentLine = currentCallStackLines[callStackLineIndex];
			messageText += Stringf("\t%s(%u): %s\n", currentLine.m_FileName, currentLine.m_LineNumber, currentLine.m_FunctionName);
		}

		messageText += "\n";
	}

	return messageText;
}



LoggerSystem::LoggerSystem() :
m_LoggerThread(nullptr),
m_IsRunning(false),
m_FlushLogs(false)
{
	
}



void LoggerSystem::InitializeLoggerSystem(const char* loggerName)
{
	g_LoggerSystem = new LoggerSystem();

	g_LoggerSystem->m_LoggerName = loggerName;
	g_LoggerSystem->m_IsRunning = true;
	g_LoggerSystem->m_LoggerThread = Thread::CreateNewThread(MessageLoggingThread, g_LoggerSystem);
}



void LoggerSystem::UninitializeLoggerSystem()
{
	g_LoggerSystem->m_IsRunning = false;
	g_LoggerSystem->m_LoggerThread->JoinThread();

	Thread::DestroyThread(g_LoggerSystem->m_LoggerThread);

	delete g_LoggerSystem;
	g_LoggerSystem = nullptr;
}



void LoggerSystem::FlushLogger()
{
	if (!m_FlushLogs)
	{
		m_FlushLogs = true;

		while (m_FlushLogs)
		{
			Thread::YieldThread();
		}
	}
}



void MessageLoggingThread(void* messageLogger)
{
	LoggerSystem* loggerSystem = (LoggerSystem*)messageLogger;
	TimeStampData currentTimeStamp = GetTimeStampForCurrentTime();

	char fileName[128];
	sprintf_s(fileName, "Logs/%s__%04i-%02i-%02i__%02i.%02i.%02i.log",
			loggerSystem->m_LoggerName,
			currentTimeStamp.m_Year,
			currentTimeStamp.m_Month,
			currentTimeStamp.m_Day,
			currentTimeStamp.m_Hour,
			currentTimeStamp.m_Minute,
			currentTimeStamp.m_Second);

	BinaryFileWriter fileWriter;
	fileWriter.OpenBinaryFile(fileName);

	while (loggerSystem->m_IsRunning)
	{
		HandleMessagesInQueue(loggerSystem, fileWriter);
		Thread::YieldThread();
	}

	HandleMessagesInQueue(loggerSystem, fileWriter);

	fileWriter.CloseBinaryFile();

	MakeDefaultFileCopy(loggerSystem, fileName);
}



void HandleMessagesInQueue(LoggerSystem* loggerSystem, BinaryFileWriter& fileWriter)
{
	LogMessage* currentLogMessage = nullptr;
	while (loggerSystem->m_AllLogMessages.Dequeue(currentLogMessage))
	{
		std::string logMessageText = currentLogMessage->GetLogMessageText();
		fileWriter.WriteTextString(logMessageText);
		delete currentLogMessage;
	}

	if (loggerSystem->m_FlushLogs)
	{
		fileWriter.FlushBinaryFile();
		loggerSystem->m_FlushLogs = false;
	}
}



void MakeDefaultFileCopy(LoggerSystem* loggerSystem, const char* fileName)
{
	unsigned char* fileBuffer = nullptr;

	BinaryFileReader fileReader;
	fileReader.OpenBinaryFile(fileName);
	
	size_t fileBufferSize = fileReader.GetBinaryFileSize();
	fileBuffer = new unsigned char[fileBufferSize];

	fileReader.ReadBytes(fileBuffer, fileBufferSize);
	fileReader.CloseBinaryFile();

	char copiedFileName[128];
	sprintf_s(copiedFileName, "Logs/%s.log", loggerSystem->m_LoggerName);

	BinaryFileWriter fileWriter;
	fileWriter.OpenBinaryFile(copiedFileName);

	fileWriter.WriteBytes(fileBuffer, fileBufferSize);
	fileWriter.CloseBinaryFile();

	delete[] fileBuffer;
}



void PrintToLog(const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];
	
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral, GetTimeStampForCurrentTime(), LOG_DEFAULT, "Default");
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}



void PrintToLogSimple(const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];

	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral);
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}



void PrintToLogWithTag(const char* messageTag, const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];

	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral, GetTimeStampForCurrentTime(), LOG_DEFAULT, messageTag);
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}



void PrintToLogWithLogLevel(const LogLevel& logLevel, const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];

	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral, GetTimeStampForCurrentTime(), logLevel, "Default");
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}



void PrintToLogWithCallStack(CallStack* currentCallStack, const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];

	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral, GetTimeStampForCurrentTime(), LOG_DEFAULT, "Default", currentCallStack);
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}



void PrintToLogWithEverything(const char* messageTag, const LogLevel& logLevel, CallStack* currentCallStack, const char* messageFormat, ...)
{
	const int MAXIMUM_MESSAGE_LENGTH = 2048;
	char messageLiteral[MAXIMUM_MESSAGE_LENGTH];

	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MAXIMUM_MESSAGE_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);

	messageLiteral[MAXIMUM_MESSAGE_LENGTH - 1] = '\0';

	LogMessage* newLogMessage = new LogMessage(messageLiteral, GetTimeStampForCurrentTime(), logLevel, messageTag, currentCallStack);
	g_LoggerSystem->m_AllLogMessages.Enqueue(newLogMessage);
}