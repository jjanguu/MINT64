#pragma once

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 300
#define CONSOLESHELL_PROMPTMESSAGE         "jjangu> "

typedef void (* CommandFunction)(const char* pcParameter);

#pragma pack(push, 1)

typedef struct kShellCommandEntryStruct
{
    char* pcCommand;
    char* pcHelp;
    CommandFunction pfFunction;
}SHELLCOMMANDENTRY;

typedef struct kParameterListStruct
{
    const char* pcBuffer;
    int iLength;
    int iCurrentPosition;
}PARAMETERLIST;

#pragma pack(pop)
void kStartConsoleShell();
void kExecuteCommand(const char* pcCommandBuffer);
void kInitializeParameter(PARAMETERLIST* pstList, const char* pcParameter);
int kGetNextParameter(PARAMETERLIST* pstList, char* pcParameter);
void kHelp(const char* pcParameterBuffer);
void kClear(const char* pcParameterBuffer);
void kShowTotalRAMSize(const char* pcParameterBuffer);
void kStringToDecimalHexTest(const char* pcParameterBuffer);
void kShutdown(const char* pcParamegerBuffer);