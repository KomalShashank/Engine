#pragma once



struct TimeStampData
{
	int m_Year;
	int m_Month;
	int m_Day;
	
	int m_Hour;
	int m_Minute;
	int m_Second;
};



double GetCurrentTimeInSeconds();
double GetCurrentTimeInMilliseconds();
TimeStampData GetTimeStampForCurrentTime();