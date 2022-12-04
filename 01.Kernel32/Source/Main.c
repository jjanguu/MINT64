
#include "ModeSwitch.h"
#include "Page.h"
#include "Types.h"

#define PRINT_PASS(line) kPrintString(45, line, "Pass");
#define PRINT_FAIL(line) kPrintString(45, line, "Fail");

void kPrintString(int iX, int iY, const char *pcString);
BOOL kInitializeKernel64Area();
BOOL kIsMemoryEnough();
void kCopyKernel64ImageTo2Mbyte();

void Main(void) {
  DWORD i;
  DWORD dwEAX, dwEBX, dwECX, dwEDX;
  char vcVendorString[13] = {
      0,
  };

  kPrintString(0, 3, "Protected Mode C Language Kernel Start......[Pass]");
  kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");

  if (kIsMemoryEnough() == FALSE) {
    PRINT_FAIL(4)
    kPrintString(0, 5, "Not Enough Memory MINT64 OS Requires Over 64Mbyte Mem");
    while (1)
      ;
  } else {
    PRINT_PASS(4)
  }

  kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");

  if (kInitializeKernel64Area() == FALSE) {
    PRINT_FAIL(5)
    kPrintString(0, 6, "Kernel Area Initialization Fail");
    while (1)
      ;
  }

  PRINT_PASS(5)

  kPrintString(0, 6, "IA-32e Page Tables Initialize...............[    ]");
  kInitializePageTables();
  PRINT_PASS(6)

  kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
  *(DWORD *)vcVendorString = dwEBX;
  *((DWORD *)vcVendorString + 1) = dwEDX;
  *((DWORD *)vcVendorString + 2) = dwECX;
  kPrintString(0, 7,
               "Processor Vendor String.....................[            ]");
  kPrintString(45, 7, vcVendorString);

  kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
  kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
  if (dwEDX & (1 << 29)) {
    PRINT_PASS(8)
  } else {
    PRINT_FAIL(8)
    kPrintString(0, 9, "This processor does not support 64bit mode");
    while (1)
      ;
  }

  kPrintString(0, 9, "Copy IA-32e Kernel To 2M Address............[    ]");
  kCopyKernel64ImageTo2Mbyte();
  PRINT_PASS(9)

  kPrintString(0, 9, "Switch To IA-32e Mode");
  kSwitchAndExecute64bitKernel();

  while (1)
    ;
}

void kPrintString(int iX, int iY, const char *pcString) {
  CHARACTER *pstScreen = (CHARACTER *)0xB8000;
  int i;

  pstScreen += (iY * 80) + iX;
  for (i = 0; pcString[i] != 0; i++) {
    pstScreen[i].bCharactor = pcString[i];
  }
}

BOOL kInitializeKernel64Area() {
  DWORD *pdwCurrentAddress;

  pdwCurrentAddress = (DWORD *)0x100000;

  while ((DWORD)pdwCurrentAddress < 0x600000) {
    *pdwCurrentAddress = 0x00;

    if (*pdwCurrentAddress != 0) {
      return FALSE;
    }

    pdwCurrentAddress++;
  }
  return TRUE;
}

BOOL kIsMemoryEnough() {
  DWORD *pdwCurrentAddress;

  pdwCurrentAddress = (DWORD *)0x100000;

  while ((DWORD)pdwCurrentAddress < 0x4000000) {
    *pdwCurrentAddress = 0x12345678;

    if (*pdwCurrentAddress != 0x12345678) {
      return FALSE;
    }

    pdwCurrentAddress += (0x100000 / 4);
  }
  return TRUE;
}

void kCopyKernel64ImageTo2Mbyte() {
  WORD wKernel32SectorCount, wTotalKernelSectorCount;
  DWORD *pdwSourceAddress, *pdwDestinationAddress;
  int i;

  wTotalKernelSectorCount = *((WORD *)0x7C05);
  wKernel32SectorCount = *((WORD *)0x7C07);

  pdwSourceAddress = (DWORD *)(0x10000 + (wKernel32SectorCount * 512));
  pdwDestinationAddress = (DWORD *)0x200000;

  for (i = 0; i < 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4;
       i++) {
    *pdwDestinationAddress = *pdwSourceAddress;
    pdwDestinationAddress++;
    pdwSourceAddress++;
  }
}