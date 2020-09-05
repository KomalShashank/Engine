#pragma once

#include "Engine/DebugTools/MemoryAnalytics/CallStack.hpp"
#include "Engine/DeveloperConsole/Command.hpp"

#define ANALYTICS_MODE 0



extern size_t g_NumberOfAllocations;
extern size_t g_MaximumNumberOfAllocations;

extern size_t g_AllocatedMemoryAtStartup;
extern size_t g_AllocatedMemoryInBytes;
extern size_t g_AllocatedMemoryAtShutDown;

void MemoryAnalyticsStartup();
void MemoryAnalyticsShutdown();