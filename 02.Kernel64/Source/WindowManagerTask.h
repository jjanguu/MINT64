#pragma once
#include "2DGraphics.h"
#include "Types.h"

#define WINDOWMANAGER_DATAACCUMULATECOUNT 20
#define WINDOWMANAGER_RESIZEMARKERSIZE 20
#define WINDOWMANAGER_COLOR_RESIZEMARKER RGB(210, 20, 20)
#define WINDOWMANAGER_THICK_RESIZEMARKER 4

void kStartWindowManager();
BOOL kProcessMouseData();
BOOL kProcessKeyData();
BOOL kProcessEventQueueData();
void kDrawResizeMarker(const RECT *pstArea, BOOL bShowMarker);