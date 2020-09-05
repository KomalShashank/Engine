#include "Engine/DebugTools/ProfilerSystem/ProfilerSystem.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/DebugTools/LoggerSystem/LoggerSystem.hpp"
#include "Engine/DeveloperConsole/DeveloperConsole.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



const int MAXIMUM_NUMBER_OF_SAMPLES = 1024;

ProfilerSystem* g_ProfilerSystem = nullptr;



ProfileSample::ProfileSample() :
m_SampleTag(nullptr),
m_StartTime(0),
m_EndTime(0),
m_NextSample(nullptr),
m_FirstChildSample(nullptr),
m_ParentSample(nullptr)
{

}



void ProfileSample::AddChildSample(ProfileSample* childSample)
{
	if (m_FirstChildSample == nullptr)
	{
		m_FirstChildSample = childSample;
		return;
	}

	ProfileSample* lastChildSample = m_FirstChildSample;
	while (lastChildSample->m_NextSample != nullptr)
	{
		lastChildSample = lastChildSample->m_NextSample;
	}

	lastChildSample->m_NextSample = childSample;
}



uint64_t ProfileSample::GetSampleTime() const
{
	return (m_EndTime - m_StartTime);
}



uint64_t ProfileSample::GetSampleSelfTime() const
{
	uint64_t totalChildrenTime = 0;
	
	ProfileSample* currentChildSample = m_FirstChildSample;
	if (currentChildSample != nullptr)
	{
		totalChildrenTime += currentChildSample->GetSampleTime();
		
		while (currentChildSample->m_NextSample != nullptr)
		{
			currentChildSample = currentChildSample->m_NextSample;
			totalChildrenTime += currentChildSample->GetSampleTime();
		}
	}

	return (GetSampleTime() - totalChildrenTime);
}



double ProfileSample::GetSampleTimeInSeconds() const
{
	return ConvertPerformanceCountToSeconds(GetSampleTime());
}



double ProfileSample::GetSampleTimeInMilliseconds() const
{
	return (GetSampleTimeInSeconds() * 1000.0);
}



double ProfileSample::GetSampleTimeInMicroseconds() const
{
	return (GetSampleTimeInMilliseconds() * 1000.0);
}



double ProfileSample::GetSampleSelfTimeInSeconds() const
{
	return ConvertPerformanceCountToSeconds(GetSampleSelfTime());
}



double ProfileSample::GetSampleSelfTimeInMilliseconds() const
{
	return (GetSampleSelfTimeInSeconds() * 1000.0);
}



double ProfileSample::GetSampleSelfTimeInMicroseconds() const
{
	return (GetSampleSelfTimeInMilliseconds() * 1000.0);
}



void ProfileSample::StartSample()
{
	m_StartTime = GetCurrentPerformanceCount();
}



void ProfileSample::EndSample()
{
	m_EndTime = GetCurrentPerformanceCount();
}



LogScopedProfile::LogScopedProfile(const char* sampleTag)
{
	m_Sample.m_SampleTag = sampleTag;
	m_Sample.StartSample();
}



LogScopedProfile::~LogScopedProfile()
{
	m_Sample.EndSample();
	PrintToLogWithTag("Profiler", "%s took %.3f milliseconds\n", m_Sample.m_SampleTag, m_Sample.GetSampleTimeInMilliseconds());
}



ProfileSampleInfo::ProfileSampleInfo(ProfileSample* currentProfileSample, uint32_t treeDepth /*= 0*/) :
m_SampleTag(currentProfileSample->m_SampleTag),
m_NumberOfCalls(1),
m_TreeDepth(treeDepth),
m_SampleTime(currentProfileSample->GetSampleTime()),
m_SampleSelfTime(currentProfileSample->GetSampleSelfTime()),
m_ParentSample(currentProfileSample->m_ParentSample)
{

}



void ProfileSampleInfo::AddOrUpdateListSampleInfo(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos, uint32_t treeDepth)
{
	bool sampleAlreadyExists = false;

	for (ProfileSampleInfo& currentSampleInfo : sampleInfos)
	{
		if (currentProfileSample->m_ParentSample == currentSampleInfo.m_ParentSample)
		{
			if (currentProfileSample->m_SampleTag == currentSampleInfo.m_SampleTag)
			{
				++currentSampleInfo.m_NumberOfCalls;
				currentSampleInfo.m_SampleTime += currentProfileSample->GetSampleTime();
				currentSampleInfo.m_SampleSelfTime += currentProfileSample->GetSampleSelfTime();

				sampleAlreadyExists = true;

				break;
			}
		}
	}

	if (!sampleAlreadyExists)
	{
		ProfileSampleInfo newSampleInfo = ProfileSampleInfo(currentProfileSample, treeDepth);
		sampleInfos.push_back(newSampleInfo);
	}
}



void ProfileSampleInfo::AddOrUpdateFlatSampleInfo(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos)
{
	bool sampleAlreadyExists = false;

	for (ProfileSampleInfo& currentSampleInfo : sampleInfos)
	{
		if (currentProfileSample->m_SampleTag == currentSampleInfo.m_SampleTag)
		{
			++currentSampleInfo.m_NumberOfCalls;
			currentSampleInfo.m_SampleTime += currentProfileSample->GetSampleTime();
			currentSampleInfo.m_SampleSelfTime += currentProfileSample->GetSampleSelfTime();

			sampleAlreadyExists = true;

			break;
		}
	}

	if (!sampleAlreadyExists)
	{
		ProfileSampleInfo newSampleInfo = ProfileSampleInfo(currentProfileSample);
		sampleInfos.push_back(newSampleInfo);
	}
}



ProfilerSystem::ProfilerSystem() :
m_IsEnabled(false),
m_ShouldBeEnabled(true),
m_CurrentSample(nullptr),
m_PreviousFrame(nullptr),
m_CurrentFrame(nullptr)
{
	m_SamplePool.InitializeObjectPool(MAXIMUM_NUMBER_OF_SAMPLES);

	DeveloperConsole::RegisterCommands("EnableProfiling", "Enables the profiler.", EnableProfilingCommand);
	DeveloperConsole::RegisterCommands("DisableProfiling", "Disables the profiler.", DisableProfilingCommand);
	DeveloperConsole::RegisterCommands("PrintLastProfileFrame", "Prints the last profile frame in list view. Takes ListView or FlatView as optional arguments.", PrintLastProfileFrameCommand);
}



ProfilerSystem::~ProfilerSystem()
{
	m_SamplePool.UninitializeObjectPool();
}



void ProfilerSystem::ProfilerSystemStartup()
{
#if (PROFILING_ENABLED)

	if (g_ProfilerSystem == nullptr)
	{
		g_ProfilerSystem = new ProfilerSystem();
	}

#endif
}



void ProfilerSystem::ProfilerSystemShutdown()
{
#if (PROFILING_ENABLED)
	
	if (g_ProfilerSystem != nullptr)
	{
		delete g_ProfilerSystem;
		g_ProfilerSystem = nullptr;
	}

#endif
}



ProfilerSystem* ProfilerSystem::SingletonInstance()
{
#if (PROFILING_ENABLED)
	
	ASSERT_OR_DIE(g_ProfilerSystem != nullptr, "A valid ProfilerSystem instance does not exist.");

	return g_ProfilerSystem;

#else

	return nullptr;

#endif
}



void ProfilerSystem::MarkProfileFrame()
{
	if (m_IsEnabled)
	{
		ASSERT_OR_DIE(m_CurrentFrame == m_CurrentSample, "Profiler Error. Sample Mismatch.");
		PopProfileSample();
		
		if (m_PreviousFrame != nullptr)
		{
			DeleteProfileSample(m_PreviousFrame);
		}

		m_PreviousFrame = m_CurrentFrame;
	}

	m_IsEnabled = m_ShouldBeEnabled;

	if (m_IsEnabled)
	{
		PushProfileSample("Frame");
		m_CurrentFrame = m_CurrentSample;
	}
}



ProfileSample* ProfilerSystem::GetLastProfileFrame()
{
	return m_PreviousFrame;
}



void ProfilerSystem::PushProfileSample(const char* sampleTag)
{
	ProfileSample* newSample = m_SamplePool.AllocateObjectFromPool();
	newSample->m_SampleTag = sampleTag;
	newSample->m_ParentSample = m_CurrentSample;
	
	if (m_CurrentSample != nullptr)
	{
		m_CurrentSample->AddChildSample(newSample);
	}

	m_CurrentSample = newSample;
	m_CurrentSample->StartSample();
}



void ProfilerSystem::PopProfileSample()
{
	ASSERT_OR_DIE(m_CurrentSample != nullptr, "No Sample Exists.");
	m_CurrentSample->EndSample();
	m_CurrentSample = m_CurrentSample->m_ParentSample;
}



void ProfilerSystem::DeleteProfileSample(ProfileSample* currentProfileSample)
{
	ProfileSample* currentChildSample = currentProfileSample->m_FirstChildSample;
	if (currentChildSample != nullptr)
	{
		while (currentChildSample->m_NextSample != nullptr)
		{
			ProfileSample* nextChildSample = currentChildSample->m_NextSample;
			DeleteProfileSample(currentChildSample);
			currentChildSample = nextChildSample;
		}

		if (currentChildSample->m_NextSample == nullptr)
		{
			DeleteProfileSample(currentChildSample);
		}
	}

	m_SamplePool.DeallocateObjectToPool(currentProfileSample);
}



void ProfilerSystem::SetProfilingEnabled(bool profilingEnabled)
{
	m_ShouldBeEnabled = profilingEnabled;
}



void ProfilerSystem::PrintLastProfileFrame(const ProfileViewFormat& viewFormat, uint64_t frameNumber /*= 0U*/, uint32_t runNumber /*= 0U*/)
{
	ProfileSample* lastProfileFrame = GetLastProfileFrame();

	if (lastProfileFrame != nullptr)
	{
		switch (viewFormat)
		{
		case LIST_VIEW:
			PrintFrameInListView(lastProfileFrame);
			break;

		case FLAT_VIEW:
			PrintFrameInFlatView(lastProfileFrame);
			break;

		case CSV_VIEW:
			PrintFrameInCSVView(lastProfileFrame, frameNumber, runNumber);
			break;

		default:
			break;
		}
	}
}



void ProfilerSystem::PrintFrameInListView(ProfileSample* currentFrame)
{
	double totalFrameTimeInMilliseconds = currentFrame->GetSampleTimeInMilliseconds();
	std::vector<ProfileSampleInfo> allSampleInfos;

	uint32_t treeDepth = 0;

	ProfileSampleInfo::AddOrUpdateListSampleInfo(currentFrame, allSampleInfos, treeDepth);
	PopulateChildListSampleInfos(currentFrame, allSampleInfos, treeDepth + 1);

	PrintToLogSimple("TAG\t\t\t\t\t\t\t\tCALLS\t\t\t\t\t\t\t\tTIME\t\t\t\t\t\t\t\tSELF TIME\t\t\t\t\t\t\t\tFRAMETIME PERCENT\n");

	for (const ProfileSampleInfo& currentSampleInfo : allSampleInfos)
	{
		double sampleTimeInMilliseconds = ConvertPerformanceCountToSeconds(currentSampleInfo.m_SampleTime) * 1000.0;
		double sampleSelfTimeInMilliseconds = ConvertPerformanceCountToSeconds(currentSampleInfo.m_SampleSelfTime) * 1000.0;

		double frameTimePercentage = (sampleTimeInMilliseconds / totalFrameTimeInMilliseconds) * 100.0;
		
		PrintToLogSimple("%.*s%s\t\t\t\t\t\t\t\t%i\t\t\t\t\t\t\t\t%6llu cycles (%.3f ms)\t\t\t\t\t\t\t\t%6llu cycles (%.3f ms)\t\t\t\t\t\t\t\t%.2f%%\n",
			currentSampleInfo.m_TreeDepth, "----------------",
			currentSampleInfo.m_SampleTag,
			currentSampleInfo.m_NumberOfCalls,
			currentSampleInfo.m_SampleTime,
			sampleTimeInMilliseconds,
			currentSampleInfo.m_SampleSelfTime,
			sampleSelfTimeInMilliseconds,
			frameTimePercentage);
	}
}



void ProfilerSystem::PrintFrameInFlatView(ProfileSample* currentFrame)
{
	double totalFrameTimeInMilliseconds = currentFrame->GetSampleTimeInMilliseconds();
	std::vector<ProfileSampleInfo> allSampleInfos;

	ProfileSampleInfo::AddOrUpdateFlatSampleInfo(currentFrame, allSampleInfos);
	PopulateChildFlatSampleInfos(currentFrame, allSampleInfos);

	PrintToLogSimple("TAG\t\t\t\t\t\t\t\tCALLS\t\t\t\t\t\t\t\tTIME\t\t\t\t\t\t\t\tSELF TIME\t\t\t\t\t\t\t\tFRAMETIME PERCENT\n");

	for (const ProfileSampleInfo& currentSampleInfo : allSampleInfos)
	{
		double sampleTimeInMilliseconds = ConvertPerformanceCountToSeconds(currentSampleInfo.m_SampleTime) * 1000.0;
		double sampleSelfTimeInMilliseconds = ConvertPerformanceCountToSeconds(currentSampleInfo.m_SampleSelfTime) * 1000.0;

		double frameTimePercentage = (sampleTimeInMilliseconds / totalFrameTimeInMilliseconds) * 100.0;

		PrintToLogSimple("%s\t\t\t\t\t\t\t\t%i\t\t\t\t\t\t\t\t%6llu cycles (%.3f ms)\t\t\t\t\t\t\t\t%6llu cycles (%.3f ms)\t\t\t\t\t\t\t\t%.2f%%\n",
			currentSampleInfo.m_SampleTag,
			currentSampleInfo.m_NumberOfCalls,
			currentSampleInfo.m_SampleTime,
			sampleTimeInMilliseconds,
			currentSampleInfo.m_SampleSelfTime,
			sampleSelfTimeInMilliseconds,
			frameTimePercentage);
	}
}



void ProfilerSystem::PrintFrameInCSVView(ProfileSample* currentFrame, uint64_t frameNumber, uint32_t runNumber)
{
	double totalFrameTimeInMicroseconds = currentFrame->GetSampleTimeInMicroseconds();
	std::vector<ProfileSampleInfo> allSampleInfos;

	ProfileSampleInfo::AddOrUpdateFlatSampleInfo(currentFrame, allSampleInfos);
	PopulateChildFlatSampleInfos(currentFrame, allSampleInfos);

	if (frameNumber == 1U)
	{
		PrintToLogSimple("FRAME NO.,TAG,TIME (microseconds),PERCENT (%%),RUN NO.\n");
	}

	size_t totalNumberOfSampleInfos = allSampleInfos.size();
	for (size_t sampleInfoIndex = 0; sampleInfoIndex < totalNumberOfSampleInfos; ++sampleInfoIndex)
	{
		const ProfileSampleInfo& currentSampleInfo = allSampleInfos[sampleInfoIndex];
		
		double sampleTimeInMicroseconds = ConvertPerformanceCountToSeconds(currentSampleInfo.m_SampleTime) * 1000.0 * 1000.0;
		double frameTimePercentage = (sampleTimeInMicroseconds / totalFrameTimeInMicroseconds) * 100.0;

		if (sampleInfoIndex == 0U)
		{
			PrintToLogSimple("\n%llu,%s,%.3f,%.2f,%u\n",
				frameNumber,
				currentSampleInfo.m_SampleTag,
				sampleTimeInMicroseconds,
				frameTimePercentage,
				runNumber);

			continue;
		}
		
		PrintToLogSimple(",%s,%.3f,%.2f\n",
			currentSampleInfo.m_SampleTag,
			sampleTimeInMicroseconds,
			frameTimePercentage);
	}
}



void ProfilerSystem::PopulateChildListSampleInfos(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos, uint32_t treeDepth)
{
	ProfileSample* currentChildSample = currentProfileSample->m_FirstChildSample;
	if (currentChildSample != nullptr)
	{
		ProfileSampleInfo::AddOrUpdateListSampleInfo(currentChildSample, sampleInfos, treeDepth);
		PopulateChildListSampleInfos(currentChildSample, sampleInfos, treeDepth + 1);

		while (currentChildSample->m_NextSample != nullptr)
		{
			currentChildSample = currentChildSample->m_NextSample;
			ProfileSampleInfo::AddOrUpdateListSampleInfo(currentChildSample, sampleInfos, treeDepth);
			PopulateChildListSampleInfos(currentChildSample, sampleInfos, treeDepth + 1);
		}
	}
}



void ProfilerSystem::PopulateChildFlatSampleInfos(ProfileSample* currentProfileSample, std::vector<ProfileSampleInfo>& sampleInfos)
{
	ProfileSample* currentChildSample = currentProfileSample->m_FirstChildSample;
	if (currentChildSample != nullptr)
	{
		ProfileSampleInfo::AddOrUpdateFlatSampleInfo(currentChildSample, sampleInfos);
		PopulateChildFlatSampleInfos(currentChildSample, sampleInfos);

		while (currentChildSample->m_NextSample != nullptr)
		{
			currentChildSample = currentChildSample->m_NextSample;
			ProfileSampleInfo::AddOrUpdateFlatSampleInfo(currentChildSample, sampleInfos);
			PopulateChildFlatSampleInfos(currentChildSample, sampleInfos);
		}
	}
}



double GetPerformanceFrequency()
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency(&countsPerSecond);

	return (1.0 / static_cast<double>(countsPerSecond.QuadPart));
}



uint64_t GetCurrentPerformanceCount()
{
	LARGE_INTEGER currentPerformanceCount;
	QueryPerformanceCounter(&currentPerformanceCount);

	return static_cast<uint64_t>(currentPerformanceCount.QuadPart);
}



double ConvertPerformanceCountToSeconds(uint64_t currentPerformanceCount)
{
	static double secondsPerCount = GetPerformanceFrequency();

	return (static_cast<double>(currentPerformanceCount) * secondsPerCount);
}



void EnableProfilingCommand(Command& currentCommand)
{
	ConsoleLine enabledMessage;

	if (currentCommand.HasNoArguments())
	{
		ProfilerSystem::SingletonInstance()->SetProfilingEnabled(true);
		enabledMessage = ConsoleLine("Profiling Enabled.", RGBA::GREEN);
	}
	else
	{
		enabledMessage = ConsoleLine("EnableProfiling takes no arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(enabledMessage);
}



void DisableProfilingCommand(Command& currentCommand)
{
	ConsoleLine disabledMessage;

	if (currentCommand.HasNoArguments())
	{
		ProfilerSystem::SingletonInstance()->SetProfilingEnabled(false);
		disabledMessage = ConsoleLine("Profiling Disabled.", RGBA::GREEN);
	}
	else
	{
		disabledMessage = ConsoleLine("DisableProfiling takes no arguments.", RGBA::RED);
	}

	DeveloperConsole::AddNewConsoleLine(disabledMessage);
}



void PrintLastProfileFrameCommand(Command& currentCommand)
{
	ConsoleLine printMessage;

	if (currentCommand.HasNoArguments())
	{
		ProfilerSystem::SingletonInstance()->PrintLastProfileFrame(LIST_VIEW);
		printMessage = ConsoleLine("Frame Profiling printed in List View.", RGBA::GREEN);
	}
	else
	{
		std::vector<std::string> currentCommandArguments;
		currentCommand.GetCommandArguments(currentCommandArguments);
		std::string viewFormat = currentCommandArguments[0];

		if (viewFormat == "ListView")
		{
			ProfilerSystem::SingletonInstance()->PrintLastProfileFrame(LIST_VIEW);
			printMessage = ConsoleLine("Frame Profiling printed in List View.", RGBA::GREEN);
		}
		else if (viewFormat == "FlatView")
		{
			ProfilerSystem::SingletonInstance()->PrintLastProfileFrame(FLAT_VIEW);
			printMessage = ConsoleLine("Frame Profiling printed in Flat View.", RGBA::GREEN);
		}
		else
		{
			printMessage = ConsoleLine("Invalid argument. Printing failed.", RGBA::RED);
		}
	}

	DeveloperConsole::AddNewConsoleLine(printMessage);
}