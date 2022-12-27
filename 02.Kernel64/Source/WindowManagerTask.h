#pragma once
#include "Types.h"

#define WINDOWMANAGER_DATAACCUMULATECOUNT 20

void kStartWindowManager();
BOOL kProcessMouseData();
BOOL kProcessKeyData();
BOOL kProcessEventQueueData();