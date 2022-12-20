
#include "WindowManagerTask.h"
#include "Mouse.h"
#include "Task.h"
#include "Types.h"
#include "Utility.h"
#include "VBE.h"
#include "Window.h"

void kStartWindowManager(void) {
  int iMouseX, iMouseY;
  BOOL bMouseDataResult;
  BOOL bKeyDataResult;
  BOOL bEventQueueResult;

  kInitializeGUISystem();

  kGetCursorPosition(&iMouseX, &iMouseY);
  kMoveCursor(iMouseX, iMouseY);

  while (1) {

    bMouseDataResult = kProcessMouseData();

    bKeyDataResult = kProcessKeyData();

    bEventQueueResult = FALSE;
    while (kProcessEventQueueData() == TRUE) {
      bEventQueueResult = TRUE;
    }

    if ((bMouseDataResult == FALSE) && (bKeyDataResult == FALSE) &&
        (bEventQueueResult == FALSE)) {
      kSleep(0);
    }
  }
}

BOOL kProcessMouseData(void) {
  QWORD qwWindowIDUnderMouse;
  BYTE bButtonStatus;
  int iRelativeX, iRelativeY;
  int iMouseX, iMouseY;
  int iPreviousMouseX, iPreviousMouseY;
  BYTE bChangedButton;
  RECT stWindowArea;
  EVENT stEvent;
  WINDOWMANAGER *pstWindowManager;
  char vcTempTitle[WINDOW_TITLEMAXLENGTH];
  static int iWindowCount = 0;
  QWORD qwWindowID;

  if (kGetMouseDataFromMouseQueue(&bButtonStatus, &iRelativeX, &iRelativeY) ==
      FALSE)
    return FALSE;

  pstWindowManager = kGetWindowManager();

  kGetCursorPosition(&iMouseX, &iMouseY);

  iPreviousMouseX = iMouseX;
  iPreviousMouseY = iMouseY;

  iMouseX += iRelativeX;
  iMouseY += iRelativeY;

  kMoveCursor(iMouseX, iMouseY);
  kGetCursorPosition(&iMouseX, &iMouseY);

  qwWindowIDUnderMouse = kFindWindowByPoint(iMouseX, iMouseY);

  bChangedButton = pstWindowManager->bPreviousButtonStatus ^ bButtonStatus;

  if (bChangedButton & MOUSE_LBUTTONDOWN) {

    if (bButtonStatus & MOUSE_LBUTTONDOWN) {

      if (qwWindowIDUnderMouse != pstWindowManager->qwBackgoundWindowID)

        kMoveWindowToTop(qwWindowIDUnderMouse);

      if (kIsInTitleBar(qwWindowIDUnderMouse, iMouseX, iMouseY) == TRUE) {

        if (kIsInCloseButton(qwWindowIDUnderMouse, iMouseX, iMouseY) == TRUE) {

          kSetWindowEvent(qwWindowIDUnderMouse, EVENT_WINDOW_CLOSE, &stEvent);
          kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);

          kDeleteWindow(qwWindowIDUnderMouse);
        }

        else {

          pstWindowManager->bWindowMoveMode = TRUE;

          pstWindowManager->qwMovingWindowID = qwWindowIDUnderMouse;
        }
      }

      else {

        kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_LBUTTONDOWN, iMouseX,
                       iMouseY, bButtonStatus, &stEvent);
        kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
      }
    }

    else {

      if (pstWindowManager->bWindowMoveMode == TRUE) {

        pstWindowManager->bWindowMoveMode = FALSE;
        pstWindowManager->qwMovingWindowID = WINDOW_INVALIDID;
      }

      else {

        kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_LBUTTONUP, iMouseX,
                       iMouseY, bButtonStatus, &stEvent);
        kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
      }
    }
  }

  else if (bChangedButton & MOUSE_RBUTTONDOWN) {

    if (bButtonStatus & MOUSE_RBUTTONDOWN) {

      kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_RBUTTONDOWN, iMouseX,
                     iMouseY, bButtonStatus, &stEvent);
      kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);

      kSPrintf(vcTempTitle, "OS Test Window %d", iWindowCount++);
      qwWindowID = kCreateWindow(
          iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2, 400, 200,
          WINDOW_FLAGS_DRAWFRAME | WINDOW_FLAGS_DRAWTITLE, vcTempTitle);

      kDrawText(qwWindowID, 10, WINDOW_TITLEBAR_HEIGHT + 10, RGB(0, 0, 0),
                WINDOW_COLOR_BACKGROUND, "TEXT AREA", 9);
      kDrawText(qwWindowID, 10, WINDOW_TITLEBAR_HEIGHT + 30, RGB(0, 0, 0),
                WINDOW_COLOR_BACKGROUND, "TEXT AREA", 9);
      kShowWindow(qwWindowID, TRUE);
    } else {

      kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_RBUTTONUP, iMouseX,
                     iMouseY, bButtonStatus, &stEvent);
      kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
    }
  }

  else if (bChangedButton & MOUSE_MBUTTONDOWN) {

    if (bButtonStatus & MOUSE_MBUTTONDOWN) {

      kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_MBUTTONDOWN, iMouseX,
                     iMouseY, bButtonStatus, &stEvent);
      kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
    } else {

      kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_MBUTTONUP, iMouseX,
                     iMouseY, bButtonStatus, &stEvent);
      kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
    }
  }

  else {

    kSetMouseEvent(qwWindowIDUnderMouse, EVENT_MOUSE_MOVE, iMouseX, iMouseY,
                   bButtonStatus, &stEvent);
    kSendEventToWindow(qwWindowIDUnderMouse, &stEvent);
  }

  if (pstWindowManager->bWindowMoveMode == TRUE) {

    if (kGetWindowArea(pstWindowManager->qwMovingWindowID, &stWindowArea) ==
        TRUE) {

      kMoveWindow(pstWindowManager->qwMovingWindowID,
                  stWindowArea.iX1 + iMouseX - iPreviousMouseX,
                  stWindowArea.iY1 + iMouseY - iPreviousMouseY);
    }

    else {

      pstWindowManager->bWindowMoveMode = FALSE;
      pstWindowManager->qwMovingWindowID = WINDOW_INVALIDID;
    }
  }

  pstWindowManager->bPreviousButtonStatus = bButtonStatus;
  return TRUE;
}

BOOL kProcessKeyData(void) {
  KEYDATA stKeyData;
  EVENT stEvent;
  QWORD qwAcitveWindowID;

  if (kGetKeyFromKeyQueue(&stKeyData) == FALSE)
    return FALSE;

  qwAcitveWindowID = kGetTopWindowID();
  kSetKeyEvent(qwAcitveWindowID, &stKeyData, &stEvent);
  return kSendEventToWindow(qwAcitveWindowID, &stEvent);
}

BOOL kProcessEventQueueData(void) {
  EVENT stEvent;
  WINDOWEVENT *pstWindowEvent;
  QWORD qwWindowID;
  RECT stArea;

  if (kReceiveEventFromWindowManagerQueue(&stEvent) == FALSE)
    return FALSE;

  pstWindowEvent = &(stEvent.stWindowEvent);

  switch (stEvent.qwType) {

  case EVENT_WINDOWMANAGER_UPDATESCREENBYID:
    if (kGetWindowArea(pstWindowEvent->qwWindowID, &stArea) == TRUE) {
      kRedrawWindowByArea(&stArea);
    }
    break;

  case EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA:

    if (kConvertRectClientToScreen(pstWindowEvent->qwWindowID,
                                   &(pstWindowEvent->stArea),
                                   &stArea) == TRUE) {
      kRedrawWindowByArea(&stArea);
    }
    break;

  case EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA:
    kRedrawWindowByArea(&(pstWindowEvent->stArea));
    break;

  default:
    break;
  }

  return TRUE;
}