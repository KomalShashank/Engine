#pragma once

#include <queue>
#include <mutex>



template <typename data_type>
class TS_Queue : protected std::queue<data_type>
{
public:
	TS_Queue()
	{
		
	}

	void Enqueue(const data_type& nodeData)
	{
		m_MutexLock.lock();
		
		push(nodeData);
		
		m_MutexLock.unlock();
	}

	bool Dequeue(data_type& dequeuedNodeData)
	{
		bool dequeueSuccessful = false;

		m_MutexLock.lock();

		if (QueueSize() > 0)
		{
			dequeuedNodeData = front();
			pop();

			dequeueSuccessful = true;
		}
		
		m_MutexLock.unlock();

		return dequeueSuccessful;
	}

	size_t QueueSize()
	{
		return size();
	}

	bool IsEmpty()
	{
		return empty();
	}

public:
	std::mutex m_MutexLock;
};