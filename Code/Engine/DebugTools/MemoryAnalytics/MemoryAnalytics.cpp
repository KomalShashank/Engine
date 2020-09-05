#include "Engine/DebugTools/MemoryAnalytics/MemoryAnalytics.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"

#include <map>



size_t g_NumberOfAllocations = 0;
size_t g_MaximumNumberOfAllocations = 0;

size_t g_AllocatedMemoryAtStartup = 0;
size_t g_AllocatedMemoryInBytes = 0;
size_t g_AllocatedMemoryAtShutDown = 0;

std::map<void*, CallStack*, std::less<void*>, UntrackedAllocator<std::pair<void*, CallStack*>>> g_CallStackRegistry;



void* operator new(size_t numberOfBytes)
{
#if (ANALYTICS_MODE == 0)
	
	void* pointerToObject = malloc(numberOfBytes);
	
	return pointerToObject;

#elif (ANALYTICS_MODE == 1)
	
	size_t* pointerToObject = (size_t*)malloc(numberOfBytes + sizeof(size_t));
	
	++g_NumberOfAllocations;
	if (g_NumberOfAllocations > g_MaximumNumberOfAllocations)
	{
		g_MaximumNumberOfAllocations = g_NumberOfAllocations;
	}

	g_AllocatedMemoryInBytes += numberOfBytes;
	
	*pointerToObject = numberOfBytes;
	++pointerToObject;
	
	return (void*)pointerToObject;

#elif (ANALYTICS_MODE == 2)

	size_t* pointerToObject = (size_t*)malloc(numberOfBytes + sizeof(size_t));
	
	++g_NumberOfAllocations;
	if (g_NumberOfAllocations > g_MaximumNumberOfAllocations)
	{
		g_MaximumNumberOfAllocations = g_NumberOfAllocations;
	}

	g_AllocatedMemoryInBytes += numberOfBytes;
	
	*pointerToObject = numberOfBytes;
	++pointerToObject;

	g_CallStackRegistry[(void*)pointerToObject] = RetrieveCallStack(1);
	
	return (void*)pointerToObject;

#endif
}



void* operator new[](size_t numberOfBytes)
{
#if (ANALYTICS_MODE == 0)

	void* pointerToObject = malloc(numberOfBytes);

	return pointerToObject;

#elif (ANALYTICS_MODE == 1)

	size_t* pointerToObject = (size_t*)malloc(numberOfBytes + sizeof(size_t));
	++g_NumberOfAllocations;
	g_AllocatedMemoryInBytes += numberOfBytes;
	
	*pointerToObject = numberOfBytes;
	++pointerToObject;

	return (void*)pointerToObject;

#elif (ANALYTICS_MODE == 2)

	size_t* pointerToObject = (size_t*)malloc(numberOfBytes + sizeof(size_t));
	++g_NumberOfAllocations;
	g_AllocatedMemoryInBytes += numberOfBytes;
	
	*pointerToObject = numberOfBytes;
	++pointerToObject;
	
	g_CallStackRegistry[(void*)pointerToObject] = RetrieveCallStack(1);
	
	return (void*)pointerToObject;

#endif
}



void operator delete(void* pointerToObject)
{
	if (pointerToObject == nullptr)
	{
		return;
	}

#if (ANALYTICS_MODE == 0)

	free(pointerToObject);

#elif (ANALYTICS_MODE == 1)
	
	size_t* pointerToSize = (size_t*)pointerToObject;
	--pointerToSize;

	size_t numberOfBytes = *pointerToSize;
	--g_NumberOfAllocations;
	g_AllocatedMemoryInBytes -= numberOfBytes;

	free(pointerToSize);

#elif (ANALYTICS_MODE == 2)

	size_t* pointerToSize = (size_t*)pointerToObject;
	--pointerToSize;

	size_t numberOfBytes = *pointerToSize;
	--g_NumberOfAllocations;
	g_AllocatedMemoryInBytes -= numberOfBytes;

	free(pointerToSize);

	auto callStackIterator = g_CallStackRegistry.find(pointerToObject);
	if (callStackIterator != g_CallStackRegistry.end())
	{
		DiscardCallStack(callStackIterator->second);
		g_CallStackRegistry.erase(callStackIterator);
	}

#endif
}



void operator delete[](void* pointerToObject)
{
	if (pointerToObject == nullptr)
	{
		return;
	}

#if (ANALYTICS_MODE == 0)

	free(pointerToObject);

#elif (ANALYTICS_MODE == 1)

	size_t* pointerToSize = (size_t*)pointerToObject;
	--pointerToSize;

	size_t numberOfBytes = *pointerToSize;
	--g_NumberOfAllocations;
	g_AllocatedMemoryInBytes -= numberOfBytes;

	free(pointerToSize);

#elif (ANALYTICS_MODE == 2)

	size_t* pointerToSize = (size_t*)pointerToObject;
	--pointerToSize;

	size_t numberOfBytes = *pointerToSize;
	--g_NumberOfAllocations;
	g_AllocatedMemoryInBytes -= numberOfBytes;

	free(pointerToSize);

	auto callStackIterator = g_CallStackRegistry.find(pointerToObject);
	if (callStackIterator != g_CallStackRegistry.end())
	{
		DiscardCallStack(callStackIterator->second);
		g_CallStackRegistry.erase(callStackIterator);
	}

#endif
}



void MemoryAnalyticsStartup()
{
#if (ANALYTICS_MODE == 1 || ANALYTICS_MODE == 2)
	
	g_AllocatedMemoryAtStartup = g_AllocatedMemoryInBytes;
	DebuggerPrintf("\n\n\n\nAllocated Memory at Startup: %u bytes\n\n\n\n\n", g_AllocatedMemoryAtStartup);

#endif
}



void MemoryAnalyticsShutdown()
{
#if (ANALYTICS_MODE == 1)
	
	g_AllocatedMemoryAtShutDown = g_AllocatedMemoryInBytes;
	DebuggerPrintf("\n\n\n\nAllocated Memory at Shutdown: %u bytes\n\n", g_AllocatedMemoryAtShutDown);

	if (g_AllocatedMemoryAtShutDown > g_AllocatedMemoryAtStartup)
	{
		DebuggerPrintf("Memory Leaks: %u bytes\n\n\n\n\n", g_AllocatedMemoryAtShutDown - g_AllocatedMemoryAtStartup);
}
	else
	{
		DebuggerPrintf("Memory Leaks: -%u bytes\n\n\n\n\n", g_AllocatedMemoryAtStartup - g_AllocatedMemoryAtShutDown);
	}

#elif (ANALYTICS_MODE == 2)

	g_AllocatedMemoryAtShutDown = g_AllocatedMemoryInBytes;
	DebuggerPrintf("\n\n\n\nAllocated Memory at Shutdown: %u bytes\n\n", g_AllocatedMemoryAtShutDown);

	if (g_AllocatedMemoryAtShutDown > g_AllocatedMemoryAtStartup)
	{
		DebuggerPrintf("Memory Leaks: %u bytes\n\n\n\n\n", g_AllocatedMemoryAtShutDown - g_AllocatedMemoryAtStartup);
	}
	else
	{
		DebuggerPrintf("Memory Leaks: -%u bytes\n\n\n\n\n", g_AllocatedMemoryAtStartup - g_AllocatedMemoryAtShutDown);
	}

	for (const auto& currentCallStack : g_CallStackRegistry)
	{
		CallStackLine* currentCallStackLines = GetCallStackLines(currentCallStack.second);

		for (size_t callStackLineIndex = 0; callStackLineIndex < currentCallStack.second->m_NumberOfCallStackFrames; ++callStackLineIndex)
		{
			CallStackLine currentLine = currentCallStackLines[callStackLineIndex];
			DebuggerPrintf("%s(%u): %s\n", currentLine.m_FileName, currentLine.m_LineNumber, currentLine.m_FunctionName);
		}

		DebuggerPrintf("\n");
	}

#endif
}