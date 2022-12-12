#pragma once

#include "Task.h"
#include "Types.h"

BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);
WORD kInPortWord(WORD wPort);
void kOutPortWord(WORD wPort, WORD wData);
void kLoadGDTR(QWORD qwGDTRAddress);
void kLoadTR(WORD wTSSSegmentOffset);
void kLoadIDTR(QWORD qwIDTRAddress);
void kEnableInterrupt();
void kDisableInterrupt();
QWORD kReadRFLAGS();
QWORD kReadTSC();
void kSwitchContext(CONTEXT *pstCurrentContext, CONTEXT *pstNextContext);
void kHlt();
BOOL kTestAndSet(volatile BYTE *pbDestination, BYTE bCompare, BYTE bSource);
void kInitializeFPU();
void kSaveFPUContext(void *pvFPUContext);
void kLoadFPUContext(void *pvFPUContext);
void kSetTS();
void kClearTS();
void kEnableGlobalLocalAPIC();
void kPause();