#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



Clock* g_MasterClock = nullptr;



Clock::Clock(double currentTimeInSeconds) :
m_Paused(false),
m_ClockRate(1.0),
m_ParentClock(nullptr),
m_CurrentTimeInSeconds(currentTimeInSeconds),
m_PreviousTimeInSeconds(currentTimeInSeconds)
{

}



Clock::Clock() :
m_Paused(false),
m_ClockRate(1.0),
m_ParentClock(g_MasterClock),
m_CurrentTimeInSeconds(0.0),
m_PreviousTimeInSeconds(0.0)
{
	if (m_ParentClock != nullptr)
	{
		m_ParentClock->m_ChildClocks.insert(this);
	}
}



Clock::Clock(Clock* parentClock) :
m_Paused(false),
m_ClockRate(1.0),
m_ParentClock(parentClock),
m_CurrentTimeInSeconds(0.0),
m_PreviousTimeInSeconds(0.0)
{
	if (m_ParentClock != nullptr)
	{
		m_ParentClock->m_ChildClocks.insert(this);
	}
}



Clock::~Clock()
{
	for (auto childClockIterator = m_ChildClocks.begin(); childClockIterator != m_ChildClocks.end();)
	{
		Clock* currentChildClock = *childClockIterator;
		childClockIterator = m_ChildClocks.erase(childClockIterator);
		delete currentChildClock;
	}

	if (m_ParentClock != nullptr)
	{
		m_ParentClock->m_ChildClocks.erase(this);
	}
}



void Clock::PauseClock()
{
	if (!m_Paused)
	{
		m_Paused = true;
	}
}



void Clock::UnpauseClock()
{
	if (m_Paused)
	{
		m_Paused = false;
	}
}



void Clock::SetClockRate(double clockRate)
{
	m_ClockRate = clockRate;
}



void Clock::Update(double deltaTimeInSeconds)
{
	if (m_Paused)
	{
		return;
	}

	deltaTimeInSeconds *= m_ClockRate;
	m_PreviousTimeInSeconds = m_CurrentTimeInSeconds;
	m_CurrentTimeInSeconds += deltaTimeInSeconds;

	for (Clock* childClock : m_ChildClocks)
	{
		childClock->Update(deltaTimeInSeconds);
	}
}



double Clock::GetDeltaTimeDouble() const
{
	return (m_CurrentTimeInSeconds - m_PreviousTimeInSeconds);
}



float Clock::GetDeltaTimeFloat() const
{
	return static_cast<float>(GetDeltaTimeDouble());
}



void Clock::InitializeMasterClock()
{
	g_MasterClock = new Clock(GetCurrentTimeInSeconds());
}



void Clock::UninitializeMasterClock()
{
	delete g_MasterClock;
	g_MasterClock = nullptr;
}



void Clock::UpdateMasterClock()
{
	double deltaTimeInSeconds = GetCurrentTimeInSeconds() - g_MasterClock->m_CurrentTimeInSeconds;
	g_MasterClock->Update(deltaTimeInSeconds);
}



const Clock* Clock::MasterClock()
{
	ASSERT_OR_DIE(g_MasterClock != nullptr, "A valid MasterClock instance does not exist.");

	return g_MasterClock;
}