#include "Engine/Threading/Thread.hpp"



Thread::Thread()
{

}



Thread::Thread(ThreadedFunction* threadFunction, void* threadArgument)
{
	m_Thread = std::thread(threadFunction, threadArgument);
}



Thread::~Thread()
{
	m_Thread.~thread();
}



Thread* Thread::CreateNewThread(ThreadedFunction* threadFunction, void* threadArgument)
{
	return new Thread(threadFunction, threadArgument);
}



void Thread::DestroyThread(Thread*& currentThread)
{
	delete currentThread;
	currentThread = nullptr;
}



void Thread::DetachThread()
{
	m_Thread.detach();
}



void Thread::JoinThread()
{
	m_Thread.join();
}



void Thread::SleepThreadForTime(float timeInSeconds)
{
	uint32_t timeInMilliseconds = static_cast<uint32_t>(timeInSeconds * 1000.0f);
	std::this_thread::sleep_for(std::chrono::milliseconds(timeInMilliseconds));
}



void Thread::YieldThread()
{
	std::this_thread::yield();
}