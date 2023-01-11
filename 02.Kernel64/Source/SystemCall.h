#pragma once
#include "Types.h"

#define SYSTEMCALLL_MAXPARAMETERCOUNT 20

#pragma pack(push, 1)

typedef struct kSystemCallParameterTableStruct {
  QWORD vqwValue[SYSTEMCALLL_MAXPARAMETERCOUNT];
} PARAMETERTABLE;

#pragma pack(pop)

#define PARAM(x) (pstParameter->vqwValue[(x)])

void kInitializeSystemCall();
void kSystemCallEntryPoint(QWORD qwServiceNumber, PARAMETERTABLE *pstParameter);
QWORD kProcessSystemCall(QWORD qwServiceNumber, PARAMETERTABLE *pstParameter);
void kSystemCallTestTask();