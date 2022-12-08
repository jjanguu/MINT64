#pragma once
#include "Types.h"

#define BOOTSTRAPPROCESSOR_FLAGADDRESS 0x7C09
#define MAXPROCESSORCOUNT 16

BOOL kStartUpApplicationProcessor();
BYTE kGetAPICID();
static BOOL kWakeUpApplicationProcessor();