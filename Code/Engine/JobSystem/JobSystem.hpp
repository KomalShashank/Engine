#pragma once

#include <vector>
#include "Engine/Threading/Thread.hpp"
#include "Engine/DataStructures/ThreadSafeQueue.hpp"
#include "Engine/DataStructures/ObjectPool.hpp"



#define GENERIC			0x01
#define GENERIC_SLOW	0x02



class Job;
typedef TS_Queue<Job*> JobQueue;
typedef void (JobCallback)(Job* currentJob);



const size_t MAXIMUM_CALLBACK_DATA_SIZE = 256;



class Job
{
public:
	Job();
	~Job();

	static Job* CreateJob(size_t jobCategory, JobCallback* jobCallback);
	static void DispatchJob(Job* currentJob);
	static void DetachJob(Job* currentJob);
	static void ReleaseJob(Job* currentJob);
	static void WaitJob(Job* currentJob);
	static void RunJob(Job* currentJob);

	template <typename data_type>
	data_type ReadFromJobData()
	{
		size_t dataSize = sizeof(data_type);
		ASSERT_OR_DIE(m_CurrentReadIndex + dataSize <= MAXIMUM_CALLBACK_DATA_SIZE, "Exceeded maximum buffer size.");

		data_type readData;
		memcpy(&readData, m_CallBackData + m_CurrentReadIndex, dataSize);
		m_CurrentReadIndex += dataSize;

		return readData;
	}

	template <typename data_type>
	void WriteToJobData(data_type jobData)
	{
		size_t dataSize = sizeof(data_type);
		ASSERT_OR_DIE(m_CurrentWriteIndex + dataSize <= MAXIMUM_CALLBACK_DATA_SIZE, "Exceeded maximum buffer size.");
		
		memcpy(m_CallBackData + m_CurrentWriteIndex, &jobData, dataSize);
		m_CurrentWriteIndex += dataSize;
	}

public:
	size_t m_JobCategory;
	uint32_t m_NumberOfReferences;

	size_t m_CurrentReadIndex;
	size_t m_CurrentWriteIndex;

	JobCallback* m_JobCallback;
	unsigned char m_CallBackData[MAXIMUM_CALLBACK_DATA_SIZE];
};



class JobConsumer
{
public:
	JobConsumer(unsigned char jobCategory);
	~JobConsumer();

	void ConsumeAllJobs();
	bool ConsumeSingleJob() const;

public:
	unsigned char m_JobCategory;
	std::vector<JobQueue*> m_JobConsumerQueues;
};



class JobSystem
{
private:
	JobSystem(size_t numberOfCategories, int numberOfJobThreads);
	~JobSystem();

	void InitializeJobQueues();
	void InitializeJobThreads();
	void InitializeJobPool();
	void InitializeGenericJobConsumer();

public:
	static void InitializeJobSystem(size_t numberOfCategories, int numberOfJobThreads);
	static void UninitializeJobSystem();

	static JobSystem* SingletonInstance();
	static bool JobSystemIsRunning();

	JobQueue* GetJobQueueForCategory(size_t categoryIndex);
	bool ConsumeGenericJob() const;

public:
	size_t m_NumberOfCategories;
	int m_NumberOfJobThreads;

	JobQueue** m_JobQueues;
	Thread** m_JobThreads;
	ObjectPool<Job> m_JobPool;

private:
	JobConsumer* m_GenericJobConsumer;
};



void WorkerJobThread(void*);
uint32_t GetNumberOfSystemCores();