#pragma once

#include <set>



class Clock
{
private:
	Clock(double currentTimeInSeconds);

public:
	Clock();
	Clock(const Clock&) = delete;
	Clock(Clock* parentClock);
	~Clock();

	void PauseClock();
	void UnpauseClock();

	void SetClockRate(double clockRate);
	void Update(double deltaTimeInSeconds);

	double GetDeltaTimeDouble() const;
	float GetDeltaTimeFloat() const;

	static void InitializeMasterClock();
	static void UninitializeMasterClock();
	static void UpdateMasterClock();

	static const Clock* MasterClock();

private:
	bool m_Paused;
	double m_ClockRate;

	Clock* m_ParentClock;
	std::set<Clock*> m_ChildClocks;

	double m_CurrentTimeInSeconds;
	double m_PreviousTimeInSeconds;
};