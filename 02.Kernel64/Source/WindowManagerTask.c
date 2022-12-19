#include "WindowManagerTask.h"
#include "Mouse.h"
#include "MultiProcessor.h"
#include "Task.h"
#include "Types.h"
#include "Utility.h"
#include "VBE.h"
#include "Window.h"

void kStartWindowManager(void) {
  VBEMODEINFOBLOCK *pstVBEMode;
  int iRelativeX, iRelativeY;
  int iMouseX, iMouseY;
  BYTE bButton;
  QWORD qwWindowID;
  TCB *pstTask;
  char vcTempTitle[WINDOW_TITLEMAXLENGTH];
  int iWindowCount = 0;

  pstTask = kGetRunningTask(kGetAPICID());

  kInitializeGUISystem();

  kGetCursorPosition(&iMouseX, &iMouseY);
  kMoveCursor(iMouseX, iMouseY);

  while (1) {

    if (kGetMouseDataFromMouseQueue(&bButton, &iRelativeX, &iRelativeY) ==
        FALSE) {
      kSleep(0);
      continue;
    }

    kGetCursorPosition(&iMouseX, &iMouseY);

    iMouseX += iRelativeX;
    iMouseY += iRelativeY;

    if (bButton & MOUSE_LBUTTONDOWN) {

      kSPrintf(vcTempTitle, "Test Window %d", iWindowCount++);
      qwWindowID = kCreateWindow(
          iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2, 400, 200,
          WINDOW_FLAGS_DRAWFRAME | WINDOW_FLAGS_DRAWTITLE, vcTempTitle);

      kDrawText(qwWindowID, 10, WINDOW_TITLEBAR_HEIGHT + 10, RGB(0, 0, 0),
                WINDOW_COLOR_BACKGROUND, "TEXT AREA", 9);
      kDrawText(qwWindowID, 10, WINDOW_TITLEBAR_HEIGHT + 30, RGB(0, 0, 0),
                WINDOW_COLOR_BACKGROUND, "TEXT AREA", 9);
      kShowWindow(qwWindowID, TRUE);
    }

    else if (bButton & MOUSE_RBUTTONDOWN) {

      kDeleteAllWindowInTaskID(pstTask->stLink.qwID);
      iWindowCount = 0;
    }

    kMoveCursor(iMouseX, iMouseY);
  }
}
