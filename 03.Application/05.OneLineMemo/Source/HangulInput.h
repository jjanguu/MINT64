#pragma once
#include "Types.h"

#define TOLOWER(x) ((('A' <= (x)) && ((x) <= 'Z')) ? ((x) - 'A' + 'a') : (x))

typedef struct HangulInputTableItemStruct {

  char *pcHangul;

  char *pcInput;
} HANGULINPUTITEM;

typedef struct HangulIndexTableItemStruct {

  char cStartCharactor;

  DWORD dwIndex;
} HANGULINDEXITEM;

BOOL IsJaum(char cInput);
BOOL IsMoum(char cInput);
BOOL FindLongestHangulInTable(const char *pcInputBuffer, int iBufferCount,
                              int *piMatchIndex, int *piMatchLength);
BOOL ComposeHangul(char *pcInputBuffer, int *piInputBufferLength,
                   char *pcOutputBufferForProcessing,
                   char *pcOutputBufferForComplete);
