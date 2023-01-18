#include "2DGraphics.h"
#include "Font.h"
#include "Utility.h"
#include "VBE.h"

inline BOOL kIsInRectangle(const RECT *pstArea, int iX, int iY) {
  if ((iX < pstArea->iX1) || (pstArea->iX2 < iX) || (iY < pstArea->iY1) ||
      (pstArea->iY2 < iY))
    return FALSE;
  return TRUE;
}

inline int kGetRectangleWidth(const RECT *pstArea) {
  int iWidth;
  iWidth = pstArea->iX2 - pstArea->iX1 + 1;

  if (iWidth < 0)
    return -iWidth;
  return iWidth;
}

inline int kGetRectangleHeight(const RECT *pstArea) {
  int iHeight;

  iHeight = pstArea->iY2 - pstArea->iY1 + 1;

  if (iHeight < 0)
    return -iHeight;

  return iHeight;
}

inline BOOL kIsRectangleOverlapped(const RECT *pstArea1, const RECT *pstArea2) {
  if ((pstArea1->iX1 > pstArea2->iX2) || (pstArea1->iX2 < pstArea2->iX1) ||
      (pstArea1->iY1 > pstArea2->iY2) || pstArea1->iY2 < pstArea2->iY1)
    return FALSE;

  return TRUE;
}

inline BOOL kGetOverlappedRectangle(const RECT *pstArea1, const RECT *pstArea2,
                                    RECT *pstIntersection) {
  int iMaxX1;
  int iMinX2;
  int iMaxY1;
  int iMinY2;

  iMaxX1 = MAX(pstArea1->iX1, pstArea2->iX1);
  iMinX2 = MIN(pstArea1->iX2, pstArea2->iX2);
  if (iMinX2 < iMaxX1)
    return FALSE;

  iMaxY1 = MAX(pstArea1->iY1, pstArea2->iY1);
  iMinY2 = MIN(pstArea1->iY2, pstArea2->iY2);
  if (iMinY2 < iMaxY1)
    return FALSE;

  pstIntersection->iX1 = iMaxX1;
  pstIntersection->iY1 = iMaxY1;
  pstIntersection->iX2 = iMinX2;
  pstIntersection->iY2 = iMinY2;

  return TRUE;
}

inline void kSetRectangleData(int iX1, int iY1, int iX2, int iY2,
                              RECT *pstRect) {
  if (iX1 < iX2) {
    pstRect->iX1 = iX1;
    pstRect->iX2 = iX2;
  } else {
    pstRect->iX1 = iX2;
    pstRect->iX2 = iX1;
  }
  if (iY1 < iY2) {
    pstRect->iY1 = iY1;
    pstRect->iY2 = iY2;
  } else {
    pstRect->iY1 = iY2;
    pstRect->iY2 = iY1;
  }
}

inline void kInternalDrawPixel(const RECT *pstMemoryArea,
                               COLOR *pstMemoryAddress, int iX, int iY,
                               COLOR stColor) {
  int iWidth;

  if (!kIsInRectangle(pstMemoryArea, iX, iY)) {
    return;
  }

  iWidth = kGetRectangleWidth(pstMemoryArea);

  *(pstMemoryAddress + (iWidth * iY) + iX) = stColor;
}

void kInternalDrawLine(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                       int iX1, int iY1, int iX2, int iY2, COLOR stColor) {
  int iDeltaX, iDeltaY;
  int iError = 0;
  int iDeltaError;
  int iX, iY;
  int iStepX = 1, iStepY = 1;
  RECT stLineArea;

  kSetRectangleData(iX1, iY1, iX2, iY2, &stLineArea);
  if (!kIsRectangleOverlapped(pstMemoryArea, &stLineArea))
    return;

  iDeltaX = iX2 - iX1;
  iDeltaY = iY2 - iY1;

  if (iDeltaX < 0) {
    iDeltaX = -iDeltaX;
    iStepX = -1;
  }
  if (iDeltaY < 0) {
    iDeltaY = -iDeltaY;
    iStepY = -1;
  }

  if (iDeltaX > iDeltaY) {
    iDeltaError = iDeltaY << 1;
    iY = iY1;
    for (iX = iX1; iX != iX2; iX += iStepX) {
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iX, iY, stColor);
      iError += iDeltaError;

      if (iError >= iDeltaX) {
        iY += iStepY;
        iError -= iDeltaX << 1;
      }
    }
    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iX, iY, stColor);
  } else {
    iDeltaError = iDeltaX << 1;
    iX = iX1;
    for (iY = iY1; iY != iY2; iY += iStepY) {
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iX, iY, stColor);
      iError += iDeltaError;
      if (iError >= iDeltaY) {
        iX += iStepX;
        iError -= iDeltaY << 1;
      }
    }
    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iX, iY, stColor);
  }
}

void kInternalDrawRect(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                       int iX1, int iY1, int iX2, int iY2, COLOR stColor,
                       BOOL bFill) {
  int iWidth;
  int iTemp;
  int iY;
  int iMemoryAreaWidth;
  RECT stDrawRect;
  RECT stOverlappedArea;

  if (bFill == FALSE) {
    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, iX1, iY1, iX2, iY1,
                      stColor);
    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, iX1, iY1, iX1, iY2,
                      stColor);
    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, iX2, iY1, iX2, iY2,
                      stColor);
    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, iX1, iY2, iX2, iY2,
                      stColor);
  } else {
    kSetRectangleData(iX1, iY1, iX2, iY2, &stDrawRect);

    if (kGetOverlappedRectangle(pstMemoryArea, &stDrawRect,
                                &stOverlappedArea) == FALSE) {

      return;
    }

    iWidth = kGetRectangleWidth(&stOverlappedArea);

    iMemoryAreaWidth = kGetRectangleWidth(pstMemoryArea);
    pstMemoryAddress +=
        stOverlappedArea.iY1 * iMemoryAreaWidth + stOverlappedArea.iX1;

    for (iY = stOverlappedArea.iY1; iY < stOverlappedArea.iY2; iY++) {

      kMemSetWord(pstMemoryAddress, stColor, iWidth);

      pstMemoryAddress += iMemoryAreaWidth;
    }

    kMemSetWord(pstMemoryAddress, stColor, iWidth);
  }
}

void kInternalDrawCircle(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                         int iX, int iY, int iRadius, COLOR stColor,
                         BOOL bFill) {
  int iCircleX, iCircleY;
  int iDistance;

  if (iRadius < 0)
    return;

  iCircleY = iRadius;

  if (bFill == FALSE) {

    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, 0 + iX, iRadius + iY,
                       stColor);
    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, 0 + iX, -iRadius + iY,
                       stColor);
    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iRadius + iX, 0 + iY,
                       stColor);
    kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, -iRadius + iX, 0 + iY,
                       stColor);
  } else {

    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, 0 + iX, iRadius + iY,
                      0 + iX, -iRadius + iY, stColor);
    kInternalDrawLine(pstMemoryArea, pstMemoryAddress, iRadius + iX, 0 + iY,
                      -iRadius + iX, 0 + iY, stColor);
  }

  iDistance = -iRadius;

  for (iCircleX = 1; iCircleX <= iCircleY; iCircleX++) {

    iDistance += (iCircleX << 1) - 1;

    if (iDistance >= 0) {
      iCircleY--;

      iDistance += (-iCircleY << 1) + 2;
    }

    if (bFill == FALSE) {
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iCircleX + iX,
                         iCircleY + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iCircleX + iX,
                         -iCircleY + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, -iCircleX + iX,
                         iCircleY + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, -iCircleX + iX,
                         -iCircleY + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iCircleY + iX,
                         iCircleX + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, iCircleY + iX,
                         -iCircleX + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, -iCircleY + iX,
                         iCircleX + iY, stColor);
      kInternalDrawPixel(pstMemoryArea, pstMemoryAddress, -iCircleY + iX,
                         -iCircleX + iY, stColor);
    } else {

      kInternalDrawRect(pstMemoryArea, pstMemoryAddress, -iCircleX + iX,
                        iCircleY + iY, iCircleX + iX, iCircleY + iY, stColor,
                        TRUE);
      kInternalDrawRect(pstMemoryArea, pstMemoryAddress, -iCircleX + iX,
                        -iCircleY + iY, iCircleX + iX, -iCircleY + iY, stColor,
                        TRUE);
      kInternalDrawRect(pstMemoryArea, pstMemoryAddress, -iCircleY + iX,
                        iCircleX + iY, iCircleY + iX, iCircleX + iY, stColor,
                        TRUE);
      kInternalDrawRect(pstMemoryArea, pstMemoryAddress, -iCircleY + iX,
                        -iCircleX + iY, iCircleY + iX, -iCircleX + iY, stColor,
                        TRUE);
    }
  }
}
void kInternalDrawText(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                       int iX, int iY, COLOR stTextColor,
                       COLOR stBackgroundColor, const char *pcString,
                       int iLength) {
  int i;
  int j;

  for (i = 0; i < iLength;) {
    // 현재 문자가 한글이 아니면 영문자가 끝나는 곳을 검색
    if ((pcString[i] & 0x80) == 0) {
      // 문자열의 끝까지 검색
      for (j = i; j < iLength; j++) {
        if (pcString[j] & 0x80) {
          break;
        }
      }

      // 영문자를 출력하는 함수를 호출하고 현재 위치를 갱신
      kInternalDrawEnglishText(pstMemoryArea, pstMemoryAddress,
                               iX + (i * FONT_ENGLISHWIDTH), iY, stTextColor,
                               stBackgroundColor, pcString + i, j - i);
      i = j;
    }
    // 현재 문자가 한글이면 한글이 끝나는 곳을 검색
    else {
      // 문자열의 끝까지 검색
      for (j = i; j < iLength; j++) {
        if ((pcString[j] & 0x80) == 0) {
          break;
        }
      }

      // 영문자를 출력하는 함수를 호출하고 현재 위치를 갱신
      kInternalDrawHangulText(pstMemoryArea, pstMemoryAddress,
                              iX + i * FONT_ENGLISHWIDTH, iY, stTextColor,
                              stBackgroundColor, pcString + i, j - i);
      i = j;
    }
  }
}

/**
 *  영문자 문자 출력
 */
void kInternalDrawEnglishText(const RECT *pstMemoryArea,
                              COLOR *pstMemoryAddress, int iX, int iY,
                              COLOR stTextColor, COLOR stBackgroundColor,
                              const char *pcString, int iLength) {
  int iCurrentX, iCurrentY;
  int i, j, k;
  BYTE bBitmap;
  BYTE bCurrentBitmask;
  int iBitmapStartIndex;
  int iMemoryAreaWidth;
  RECT stFontArea;
  RECT stOverlappedArea;
  int iStartYOffset;
  int iStartXOffset;
  int iOverlappedWidth;
  int iOverlappedHeight;

  // 문자를 출력하는 X좌표
  iCurrentX = iX;

  // 메모리 영역의 너비를 계산
  iMemoryAreaWidth = kGetRectangleWidth(pstMemoryArea);

  // 문자의 개수만큼 반복
  for (k = 0; k < iLength; k++) {
    // 문자를 출력할 위치의 Y좌표를 구함
    iCurrentY = iY * iMemoryAreaWidth;

    // 현재 폰트를 표시하는 영역을 RECT 자료구조에 설정
    kSetRectangleData(iCurrentX, iY, iCurrentX + FONT_ENGLISHWIDTH - 1,
                      iY + FONT_ENGLISHHEIGHT - 1, &stFontArea);

    // 현재 그려야 할 문자가 메모리 영역과 겹치는 부분이 없으면 다음 문자로 이동
    if (kGetOverlappedRectangle(pstMemoryArea, &stFontArea,
                                &stOverlappedArea) == FALSE) {
      // 문자 하나를 뛰어넘었으므로 폰트의 너비만큼 x좌표를 이동하여 다음 문자를
      // 출력
      iCurrentX += FONT_ENGLISHWIDTH;
      continue;
    }

    // 비트맵 폰트 데이터에서 출력할 문자의 비트맵이 시작하는 위치를 계산
    // 1바이트 * FONT_HEIGHT로 구성되어 있으므로 문자의 비트맵 위치는
    // 아래와 같이 계산 가능
    iBitmapStartIndex = pcString[k] * FONT_ENGLISHHEIGHT;

    // 문자를 출력할 영역과 메모리 영역이 겹치는 부분을 이용하여 x, y오프셋과
    // 출력할 너비, 높이를 계산
    iStartXOffset = stOverlappedArea.iX1 - iCurrentX;
    iStartYOffset = stOverlappedArea.iY1 - iY;
    iOverlappedWidth = kGetRectangleWidth(&stOverlappedArea);
    iOverlappedHeight = kGetRectangleHeight(&stOverlappedArea);

    // 출력에서 제외된 y오프셋만큼 비트맵 데이터를 제외
    iBitmapStartIndex += iStartYOffset;

    // 문자 출력
    // 겹치는 영역의 y오프셋부터 높이만큼 출력
    for (j = iStartYOffset; j < iOverlappedHeight; j++) {
      // 이번 라인에서 출력할 폰트 비트맵과 비트 오프셋 계산
      bBitmap = g_vucEnglishFont[iBitmapStartIndex++];
      bCurrentBitmask = 0x01 << (FONT_ENGLISHWIDTH - 1 - iStartXOffset);

      // 겹치는 영역의 x오프셋부터 너비만큼 현재 라인에 출력
      for (i = iStartXOffset; i < iOverlappedWidth; i++) {
        // 비트가 설정되어있으면 화면에 문자색을 표시
        if (bBitmap & bCurrentBitmask) {
          pstMemoryAddress[iCurrentY + iCurrentX + i] = stTextColor;
        }
        // 비트가 설정되어있지 않으면 화면에 배경색을 표시
        else {
          pstMemoryAddress[iCurrentY + iCurrentX + i] = stBackgroundColor;
        }

        bCurrentBitmask = bCurrentBitmask >> 1;
      }

      // 다음 라인으로 이동해야 하므로, 현재 y좌표에 메모리 영역의 너비만큼
      // 더해줌
      iCurrentY += iMemoryAreaWidth;
    }

    // 문자 하나를 다 출력했으면 폰트의 넓이만큼 X 좌표를 이동하여 다음 문자를
    // 출력
    iCurrentX += FONT_ENGLISHWIDTH;
  }
}

/**
 *  한글 출력
 */
void kInternalDrawHangulText(const RECT *pstMemoryArea, COLOR *pstMemoryAddress,
                             int iX, int iY, COLOR stTextColor,
                             COLOR stBackgroundColor, const char *pcString,
                             int iLength) {
  int iCurrentX, iCurrentY;
  WORD wHangul;
  WORD wOffsetInGroup;
  WORD wGroupIndex;
  int i, j, k;
  WORD wBitmap;
  WORD wCurrentBitmask;
  int iBitmapStartIndex;
  int iMemoryAreaWidth;
  RECT stFontArea;
  RECT stOverlappedArea;
  int iStartYOffset;
  int iStartXOffset;
  int iOverlappedWidth;
  int iOverlappedHeight;

  // 문자를 출력하는 X좌표
  iCurrentX = iX;

  // 메모리 영역의 너비를 계산
  iMemoryAreaWidth = kGetRectangleWidth(pstMemoryArea);

  // 한글 문자의 개수만큼 반복
  for (k = 0; k < iLength; k += 2) {
    // 문자를 출력할 위치의 Y좌표를 구함
    iCurrentY = iY * iMemoryAreaWidth;

    // 현재 폰트를 표시하는 영역을 RECT 자료구조에 설정
    kSetRectangleData(iCurrentX, iY, iCurrentX + FONT_HANGULWIDTH - 1,
                      iY + FONT_HANGULHEIGHT - 1, &stFontArea);

    // 현재 그려야 할 문자가 메모리 영역과 겹치는 부분이 없으면 다음 문자로 이동
    if (kGetOverlappedRectangle(pstMemoryArea, &stFontArea,
                                &stOverlappedArea) == FALSE) {
      // 문자 하나를 뛰어넘었으므로 폰트의 너비만큼 x좌표를 이동하여 다음 문자를
      // 출력
      iCurrentX += FONT_HANGULWIDTH;
      continue;
    }

    // 비트맵 폰트 데이터에서 출력할 문자의 비트맵이 시작하는 위치를 계산
    // 2바이트 * FONT_HEIGHT로 구성되어 있으므로 문자의 비트맵 위치는
    // 아래와 같이 계산 가능
    // 바이트를 워드로 변환
    wHangul = ((WORD)pcString[k] << 8) | (BYTE)(pcString[k + 1]);

    // 완성형 가~힝까지이면 자/모 오프셋을 더해줌
    if ((0xB0A1 <= wHangul) && (wHangul <= 0xC8FE)) {
      wOffsetInGroup = (wHangul - 0xB0A1) & 0xFF;
      wGroupIndex = ((wHangul - 0xB0A1) >> 8) & 0xFF;
      // 그룹당 94개 문자가 있고 51개는 완성형에 없는 자모가 들어있으므로 그룹
      // 인덱스에 94를 곱한 뒤 그룹 내 오프셋에 51을 더하면 폰트 데이터에서 몇
      // 번째인지 계산할 수 있음
      wHangul = (wGroupIndex * 94) + wOffsetInGroup + 51;
    }
    // 만약 자/모이면 자음의 시작인 ㄱ을 빼서 오프셋을 구함
    else if ((0xA4A1 <= wHangul) && (wHangul <= 0xA4D3)) {
      wHangul = wHangul - 0xA4A1;
    }
    // 위의 두 가지 경우가 아니면 처리할 수 없으므로 다음 문자로 넘어감
    else {
      continue;
    }

    iBitmapStartIndex = wHangul * FONT_HANGULHEIGHT;

    // 문자를 출력할 영역과 메모리 영역이 겹치는 부분을 이용하여 x, y오프셋과
    // 출력할 너비, 높이를 계산
    iStartXOffset = stOverlappedArea.iX1 - iCurrentX;
    iStartYOffset = stOverlappedArea.iY1 - iY;
    iOverlappedWidth = kGetRectangleWidth(&stOverlappedArea);
    iOverlappedHeight = kGetRectangleHeight(&stOverlappedArea);

    // 출력에서 제외된 y오프셋만큼 비트맵 데이터를 제외
    iBitmapStartIndex += iStartYOffset;

    // 문자 출력
    // 겹치는 영역의 y오프셋부터 높이만큼 출력
    for (j = iStartYOffset; j < iOverlappedHeight; j++) {
      // 이번 라인에서 출력할 폰트 비트맵과 비트 오프셋 계산
      wBitmap = g_vusHangulFont[iBitmapStartIndex++];
      wCurrentBitmask = 0x01 << (FONT_HANGULWIDTH - 1 - iStartXOffset);

      // 겹치는 영역의 x오프셋부터 너비만큼 현재 라인에 출력
      for (i = iStartXOffset; i < iOverlappedWidth; i++) {
        // 비트가 설정되어있으면 화면에 문자색을 표시
        if (wBitmap & wCurrentBitmask) {
          pstMemoryAddress[iCurrentY + iCurrentX + i] = stTextColor;
        }
        // 비트가 설정되어있지 않으면 화면에 배경색을 표시
        else {
          pstMemoryAddress[iCurrentY + iCurrentX + i] = stBackgroundColor;
        }

        wCurrentBitmask = wCurrentBitmask >> 1;
      }

      // 다음 라인으로 이동해야 하므로, 현재 y좌표에 메모리 영역의 너비만큼
      // 더해줌
      iCurrentY += iMemoryAreaWidth;
    }

    // 문자 하나를 다 출력했으면 폰트의 넓이만큼 X 좌표를 이동하여 다음 문자를
    // 출력
    iCurrentX += FONT_HANGULWIDTH;
  }
}