#pragma once

#include "Types.h"

void kMemSet(void* pvDestination, BYTE bData, int iSize);
int kMemCpy(void* pvDestination, const void* pbSource, int iSize);
int kMemCmp(const void* pvDestination, const void* pvSource, int iSize);
BOOL kSetInterruptFlag(BOOL bEnableInterrupt);