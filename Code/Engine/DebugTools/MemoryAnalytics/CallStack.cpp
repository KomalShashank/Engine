#include "Engine/DebugTools/MemoryAnalytics/CallStack.hpp"

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#define UNREFERENCED(X) (X)

#pragma warning(disable : 4091)
#include <Windows.h>
#include <DbgHelp.h>
#include <stdlib.h>



const size_t MAXIMUM_SYMBOLNAME_LENGTH = 256;
const size_t MAXIMUM_FILENAME_LENGTH = 1024;
const size_t MAXIMUM_CALLSTACK_DEPTH = 256;



typedef BOOL(__stdcall *SymbolInitialize)(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef BOOL(__stdcall *SymbolCleanup)(IN HANDLE hProcess);
typedef BOOL(__stdcall *SymbolFromAddress)(IN HANDLE hProcess, IN DWORD64 Addr, OUT PDWORD64 Displacement, OUT PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall *SymbolGetLine)(IN HANDLE hProcess, IN DWORD64 Addr, OUT PDWORD Displacement, OUT PIMAGEHLP_LINE64 Symbol);



static HMODULE s_DebugHelp;
static HANDLE s_Process;
static SYMBOL_INFO* s_Symbol;

static char s_FileName[MAXIMUM_FILENAME_LENGTH];
static CallStackLine s_CallStackBuffer[MAXIMUM_CALLSTACK_DEPTH];

static SymbolInitialize s_SymbolInitialize;
static SymbolCleanup s_SymbolCleanup;
static SymbolFromAddress s_SymbolFromAddress;
static SymbolGetLine s_SymbolGetLine;



bool InitializeCallStackSystem()
{
	s_DebugHelp = LoadLibraryA("dbghelp.dll");
	if (s_DebugHelp == nullptr)
	{
		return false;
	}

	s_SymbolInitialize = (SymbolInitialize)GetProcAddress(s_DebugHelp, "SymInitialize");
	s_SymbolCleanup = (SymbolCleanup)GetProcAddress(s_DebugHelp, "SymCleanup");
	s_SymbolFromAddress = (SymbolFromAddress)GetProcAddress(s_DebugHelp, "SymFromAddr");
	s_SymbolGetLine = (SymbolGetLine)GetProcAddress(s_DebugHelp, "SymGetLineFromAddr64");

	s_Process = GetCurrentProcess();
	s_SymbolInitialize(s_Process, NULL, TRUE);

	s_Symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + (MAXIMUM_FILENAME_LENGTH * sizeof(char)));
	s_Symbol->MaxNameLen = MAXIMUM_FILENAME_LENGTH;
	s_Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	
	return true;
}



void UninitializeCallStackSystem()
{
	s_SymbolCleanup(s_Process);
	
	FreeLibrary(s_DebugHelp);
	s_DebugHelp = NULL;
}



CallStack* RetrieveCallStack(size_t skippedFramesOffset)
{
	void* callStackList[MAXIMUM_CALLSTACK_DEPTH];
	uint32_t numberOfCallStackFrames = CaptureStackBackTrace(1 + skippedFramesOffset, MAXIMUM_CALLSTACK_DEPTH, callStackList, NULL);

	size_t callStackBufferSize = sizeof(CallStack) + sizeof(void*) * numberOfCallStackFrames;
	void* callStackBufferData = malloc(callStackBufferSize);

	CallStack* callStack = (CallStack*)callStackBufferData;
	void* frontOfCallStackFrames = (void*)(((size_t)callStackBufferData) + sizeof(CallStack));

	callStack->m_CallStackFrames = (void**)frontOfCallStackFrames;
	callStack->m_NumberOfCallStackFrames = numberOfCallStackFrames;

	memcpy(callStack->m_CallStackFrames, callStackList, sizeof(void*) * numberOfCallStackFrames);

	return callStack;
}



void DiscardCallStack(CallStack* callStack)
{
	free(callStack);
}



CallStackLine* GetCallStackLines(CallStack* callStack)
{
	IMAGEHLP_LINE64 lineInfo;
	DWORD lineDisplacement = 0;
	lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	size_t numberOfFrames = callStack->m_NumberOfCallStackFrames;
	for (size_t frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex)
	{
		CallStackLine* callStackLine = &(s_CallStackBuffer[frameIndex]);
		DWORD64 pointerToFrame = (DWORD64)(callStack->m_CallStackFrames[frameIndex]);
		s_SymbolFromAddress(s_Process, pointerToFrame, 0, s_Symbol);

		strcpy_s(callStackLine->m_FunctionName, 1024, s_Symbol->Name);

		BOOL returnedLine = s_SymbolGetLine(GetCurrentProcess(), pointerToFrame, &lineDisplacement, &lineInfo);
		if (returnedLine)
		{
			callStackLine->m_LineNumber = lineInfo.LineNumber;
			const char* fileName = lineInfo.FileName;
			strcpy_s(callStackLine->m_FileName, 1024, fileName);
			callStackLine->m_Offset = lineDisplacement;
		}
		else
		{
			callStackLine->m_LineNumber = 0;
			callStackLine->m_Offset = 0;
			strcpy_s(callStackLine->m_FileName, 1024, "N/A");
		}
	}

	return s_CallStackBuffer;
}