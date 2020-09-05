#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>
#include "Engine/Time/Time.hpp"



double InitializeTime(LARGE_INTEGER& outInitialTime)
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency(&countsPerSecond);
	QueryPerformanceCounter(&outInitialTime);
	
	return (1.0 / static_cast<double>(countsPerSecond.QuadPart));
}



double GetCurrentTimeInSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime(initialTime);
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;
	double currentSeconds = static_cast<double>(elapsedCountsSinceInitialTime) * secondsPerCount;

	return currentSeconds;
}



double GetCurrentTimeInMilliseconds()
{
	return GetCurrentTimeInSeconds() * 1000.0;
}



TimeStampData GetTimeStampForCurrentTime()
{
	tm* currentTime = new tm();
	time_t timeHandle = time(NULL);
	errno_t success = localtime_s(currentTime, &timeHandle);

	TimeStampData currentTimeStamp = TimeStampData();

	if (success == 0)
	{
		currentTimeStamp.m_Year = currentTime->tm_year + 1900;
		currentTimeStamp.m_Month = currentTime->tm_mon + 1;
		currentTimeStamp.m_Day = currentTime->tm_mday;

		currentTimeStamp.m_Hour = currentTime->tm_hour;
		currentTimeStamp.m_Minute = currentTime->tm_min;
		currentTimeStamp.m_Second = currentTime->tm_sec;
	}

	delete currentTime;

	return currentTimeStamp;
}