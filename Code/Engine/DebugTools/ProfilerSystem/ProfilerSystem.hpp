#pragma once
#include "Engine/DataStructures/ObjectPool.hpp"
#include "Engine/DeveloperConsole/Command.hpp"

#define PROFILING_ENABLED true



enum ProfileViewFormat
{
	LIST_VIEW,
	FLAT_VIEW,
	CSV_VIEW
};



class ProfileSample
{
public:
	ProfileSample();

	void AddChildSample(ProfileSample* childSample);
	
	uint64_t GetSampleTime() const;
	uint64_t GetSampleSelfTime() const;

	double GetSampleTimeInSeconds() const;
	double GetSampleTimeInMilliseconds() const;
	double GetSampleTimeInMicroseconds() const;

	double GetSampleSelfTimeInSeconds() const;
	double GetSampleSelfTimeInMilliseconds() const;
	double GetSampleSelfTimeInMicroseconds() const;

	void StartSample();
	void EndSample();

public:
	const char* m_SampleTag;

	uint64_t m_StartTime;
	uint64_t m_EndTime;

	ProfileSample* m_NextSample;
	ProfileSample* m_FirstChildSample;
	ProfileSample* m_ParentSample;
};



#define LOG_SCOPED_PROFILE(sampleTag) LogScopedProfile _S_##__LINE__##(sampleTag)



class LogScopedProfile
{
public:
	LogScopedProfile(const char* sampleTag);
	~LogScopedProfile();

private:
	ProfileSample m_Sample;
};



struct ProfileSampleInfo
{
public:
	ProfileSampleInfo(ProfileSample* currentProfileSample, uint32_t treeDepth = 0);
	static void AddOrUpdateListSampleInfo(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos, uint32_t treeDepth);
	static void AddOrUpdateFlatSampleInfo(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos);

public:
	const char* m_SampleTag;
	uint32_t m_NumberOfCalls;
	uint32_t m_TreeDepth;
	uint64_t m_SampleTime;
	uint64_t m_SampleSelfTime;

	ProfileSample* m_ParentSample;
};



class ProfilerSystem
{
private:
	ProfilerSystem();
	~ProfilerSystem();

public:
	static void ProfilerSystemStartup();
	static void ProfilerSystemShutdown();

	static ProfilerSystem* SingletonInstance();

	void MarkProfileFrame();
	ProfileSample* GetLastProfileFrame();

	void PushProfileSample(const char* sampleTag);
	void PopProfileSample();

	void DeleteProfileSample(ProfileSample* currentProfileSample);

	void SetProfilingEnabled(bool profilingEnabled);
	void PrintLastProfileFrame(const ProfileViewFormat& viewFormat, uint64_t frameNumber = 0U, uint32_t runNumber = 0U);

private:
	void PrintFrameInListView(ProfileSample* currentFrame);
	void PrintFrameInFlatView(ProfileSample* currentFrame);
	void PrintFrameInCSVView(ProfileSample* currentFrame, uint64_t frameNumber, uint32_t runNumber);

	void PopulateChildListSampleInfos(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos, uint32_t treeDepth);
	void PopulateChildFlatSampleInfos(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos);

private:
	bool m_IsEnabled;
	bool m_ShouldBeEnabled;

	ProfileSample* m_CurrentSample;
	ProfileSample* m_PreviousFrame;
	ProfileSample* m_CurrentFrame;

	ObjectPool<ProfileSample> m_SamplePool;
};



double GetPerformanceFrequency();
uint64_t GetCurrentPerformanceCount();
double ConvertPerformanceCountToSeconds(uint64_t currentPerformanceCount);



void EnableProfilingCommand(Command& currentCommand);
void DisableProfilingCommand(Command& currentCommand);

void PrintLastProfileFrameCommand(Command& currentCommand);