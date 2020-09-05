#pragma once

#ifndef __DEBUG_CALLSTACK__
#define __DEBUG_CALLSTACK__

#include <stdint.h>

struct CallStack
{
	void** m_CallStackFrames;
	size_t m_NumberOfCallStackFrames;
};

struct CallStackLine
{
	char m_FileName[1024];
	char m_FunctionName[1024];
	uint32_t m_LineNumber;
	uint32_t m_Offset;
};

bool InitializeCallStackSystem();
void UninitializeCallStackSystem();

CallStack* RetrieveCallStack(size_t skippedFramesOffset);
void DiscardCallStack(CallStack* callStack);

CallStackLine* GetCallStackLines(CallStack* callStack);

#endif // __DEBUG_CALLSTACK__