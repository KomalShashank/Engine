#include "Engine/JobSystem/JobSystem.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



const size_t MAXIMUM_NUMBER_OF_JOBS = 1024;

JobSystem* g_JobSystem = nullptr;
bool g_JobSystemIsRunning = false;



Job::Job() :
m_NumberOfReferences(1),
m_CurrentReadIndex(0),
m_CurrentWriteIndex(0)
{

}



Job::~Job()
{

}



Job* Job::CreateJob(size_t jobCategory, JobCallback* jobCallback)
{
	Job* newJob = JobSystem::SingletonInstance()->m_JobPool.AllocateObjectFromPool();
	newJob->m_JobCategory = jobCategory;
	newJob->m_JobCallback = jobCallback;

	return newJob;
}



void Job::DispatchJob(Job* currentJob)
{
	size_t categoryIndex = GetPowerOfTwo(currentJob->m_JobCategory);
	JobSystem::SingletonInstance()->m_JobQueues[categoryIndex]->Enqueue(currentJob);
	++currentJob->m_NumberOfReferences;
}



void Job::DetachJob(Job* currentJob)
{
	Job::ReleaseJob(currentJob);
}



void Job::ReleaseJob(Job* currentJob)
{
	--currentJob->m_NumberOfReferences;
	if (currentJob->m_NumberOfReferences == 0)
	{
		JobSystem::SingletonInstance()->m_JobPool.DeallocateObjectToPool(currentJob);
	}
}



void Job::WaitJob(Job* currentJob)
{
	while (currentJob->m_NumberOfReferences == 2)
	{
		if (!JobSystem::SingletonInstance()->ConsumeGenericJob())
		{
			Thread::YieldThread();
		}
	}

	Job::ReleaseJob(currentJob);
}



void Job::RunJob(Job* currentJob)
{
	currentJob->m_JobCallback(currentJob);
	Job::ReleaseJob(currentJob);
}



JobConsumer::JobConsumer(unsigned char jobCategory) :
m_JobCategory(jobCategory)
{
	JobQueue* currentJobQueue;
	
	if (IsBitSet(m_JobCategory, GENERIC))
	{
		currentJobQueue = JobSystem::SingletonInstance()->GetJobQueueForCategory(GetPowerOfTwo(GENERIC));
		m_JobConsumerQueues.push_back(currentJobQueue);
	}
	if (IsBitSet(m_JobCategory, GENERIC_SLOW))
	{
		currentJobQueue = JobSystem::SingletonInstance()->GetJobQueueForCategory(GetPowerOfTwo(GENERIC_SLOW));
		m_JobConsumerQueues.push_back(currentJobQueue);
	}
}



JobConsumer::~JobConsumer()
{

}



void JobConsumer::ConsumeAllJobs()
{
	while (ConsumeSingleJob())
	{
		continue;
	}
}



bool JobConsumer::ConsumeSingleJob() const
{
	Job* currentJob;

	for (JobQueue* currentJobQueue : m_JobConsumerQueues)
	{
		if (currentJobQueue->Dequeue(currentJob))
		{
			Job::RunJob(currentJob);
			return true;
		}
	}

	return false;
}



JobSystem::JobSystem(size_t numberOfCategories, int numberOfJobThreads) :
m_NumberOfCategories(numberOfCategories),
m_NumberOfJobThreads(numberOfJobThreads)
{
	
}



JobSystem::~JobSystem()
{
	for (int threadIndex = 0; threadIndex < m_NumberOfJobThreads; ++threadIndex)
	{
		m_JobThreads[threadIndex]->JoinThread();
	}

	for (size_t categoryIndex = 0; categoryIndex < m_NumberOfCategories; ++categoryIndex)
	{
		while (!m_JobQueues[categoryIndex]->IsEmpty())
		{
			JobConsumer jobConsumer = JobConsumer(static_cast<unsigned char>(categoryIndex));
			jobConsumer.ConsumeAllJobs();
		}
	}

	delete m_GenericJobConsumer;
	m_JobPool.UninitializeObjectPool();
	delete[] m_JobThreads;
	delete[] m_JobQueues;
}



void JobSystem::InitializeJobQueues()
{
	m_JobQueues = new JobQueue*[m_NumberOfCategories];

	for (size_t categoryIndex = 0; categoryIndex < m_NumberOfCategories; ++categoryIndex)
	{
		JobQueue* jobQueue = new JobQueue();
		m_JobQueues[categoryIndex] = jobQueue;
	}
}



void JobSystem::InitializeJobThreads()
{
	if (m_NumberOfJobThreads <= 0)
	{
		m_NumberOfJobThreads += static_cast<int>(GetNumberOfSystemCores());
		if (m_NumberOfJobThreads <= 0)
		{
			m_NumberOfJobThreads = 1;
		}
	}

	m_JobThreads = new Thread*[m_NumberOfJobThreads];

	for (int threadIndex = 0; threadIndex < m_NumberOfJobThreads; ++threadIndex)
	{
		Thread* jobThread = Thread::CreateNewThread(WorkerJobThread, nullptr);
		m_JobThreads[threadIndex] = jobThread;
	}
}



void JobSystem::InitializeJobPool()
{
	m_JobPool.InitializeObjectPool(MAXIMUM_NUMBER_OF_JOBS);
}



void JobSystem::InitializeGenericJobConsumer()
{
	m_GenericJobConsumer = new JobConsumer(GENERIC);
}



void JobSystem::InitializeJobSystem(size_t numberOfCategories, int numberOfJobThreads)
{
	g_JobSystemIsRunning = true;
	
	if (g_JobSystem == nullptr)
	{
		g_JobSystem = new JobSystem(numberOfCategories, numberOfJobThreads);
	}

	g_JobSystem->InitializeJobQueues();
	g_JobSystem->InitializeJobThreads();
	g_JobSystem->InitializeJobPool();
	g_JobSystem->InitializeGenericJobConsumer();
}



void JobSystem::UninitializeJobSystem()
{
	g_JobSystemIsRunning = false;
	
	if (g_JobSystem != nullptr)
	{
		delete g_JobSystem;
		g_JobSystem = nullptr;
	}
}



JobSystem* JobSystem::SingletonInstance()
{
	ASSERT_OR_DIE(g_JobSystem != nullptr, "A valid JobSystem instance does not exist.");

	return g_JobSystem;
}



bool JobSystem::JobSystemIsRunning()
{
	return g_JobSystemIsRunning;
}



JobQueue* JobSystem::GetJobQueueForCategory(size_t categoryIndex)
{
	return m_JobQueues[categoryIndex];
}



bool JobSystem::ConsumeGenericJob() const
{
	return m_GenericJobConsumer->ConsumeSingleJob();
}



void WorkerJobThread(void*)
{
	JobConsumer jobConsumer = JobConsumer(GENERIC | GENERIC_SLOW);

	while (JobSystem::JobSystemIsRunning())
	{
		jobConsumer.ConsumeAllJobs();
		Thread::YieldThread();
	}
}



uint32_t GetNumberOfSystemCores()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return static_cast<uint32_t>(systemInfo.dwNumberOfProcessors);
}