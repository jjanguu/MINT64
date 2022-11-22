#pragma once

#include "Types.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kCommonIntteruptHandler(int iVectorNumber);
void kKeyboardHandler(int iVectorNumber);
void kTimerHandler(int iVectorNumber);
void kDeviceNotAvailableHandler(int iVectorNumber); 