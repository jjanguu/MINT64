#pragma once

#include <stdarg.h>
#include "Types.h"

void kMemSet(void* pvDestination, BYTE bData, int iSize);
int kMemCpy(void* pvDestination, const void* pbSource, int iSize);
int kMemCmp(const void* pvDestination, const void* pvSource, int iSize);
BOOL kSetInterruptFlag(BOOL bEnableInterrupt);
void kCheckTotalRAMSize();
QWORD kGetTotalRAMSize();
void kReverseString(char* pcBuffer);
long kAToI(const char* pcBuffer, int iRadix);
QWORD kHexStringToQword(const char* pcBuffer);
long kDecimalStringToLong(const char* pcBuffer);
int kIToA(long lValue, char* pcBuffer, int iRadix);
int kHexToString(QWORD qwValue, char* pcBuffer);
int kDecimalToString(long lValue, char* pcBuffer);
int kSPrintf(char* pcBuffer, const char* pcFormatString, ...);
int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap);