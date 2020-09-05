#pragma once

#include <thread>



typedef void (ThreadedFunction)(void* threadArgument);



class Thread
{
private:
	Thread();
	Thread(ThreadedFunction* threadFunction, void* threadArgument);
	~Thread();

public:
	static Thread* CreateNewThread(ThreadedFunction* threadFunction, void* threadArgument);
	static void DestroyThread(Thread*& currentThread);
	
	void DetachThread();
	void JoinThread();

	static void SleepThreadForTime(float timeInSeconds);
	static void YieldThread();

private:
	std::thread m_Thread;
};