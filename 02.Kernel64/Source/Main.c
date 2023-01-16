#include "2DGraphics.h"
#include "AssemblyUtility.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Descriptor.h"
#include "DynamicMemory.h"
#include "FileSystem.h"
#include "HardDisk.h"
#include "IOAPIC.h"
#include "InterruptHandler.h"
#include "Keyboard.h"
#include "LocalAPIC.h"
#include "MPConfigurationTable.h"
#include "Mouse.h"
#include "MultiProcessor.h"
#include "PIC.h"
#include "PIT.h"
#include "SerialPort.h"
#include "SystemCall.h"
#include "Task.h"
#include "Types.h"
#include "Utility.h"
#include "VBE.h"
#include "WindowManagerTask.h"

void kPrintString(int iX, int iY, const char *pcString);
void MainForApplicationProcessor();
BOOL kChangeToMultiCoreMode();
void kStartGraphicModeTest();

void Main() {
  int iCursorX, iCursorY;
  BYTE bButton;
  int iX;
  int iY;

  if (*((BYTE *)BOOTSTRAPPROCESSOR_FLAGADDRESS) == 0)
    MainForApplicationProcessor();

  *((BYTE *)BOOTSTRAPPROCESSOR_FLAGADDRESS) = 0;

  kInitializeConsole(0, 10);
  kPrintf("Switch To IA-32e Mode Success\n");
  kPrintf("IA-32e C Language Kernel Start..............[Pass]\n");
  kPrintf("Initialize Console..........................[Pass]\n");

  kGetCursor(&iCursorX, &iCursorY);
  kPrintf("GDT Initialize And Switch For IA-32e Mode...[    ]");
  kInitializeGDTTableAndTSS();
  kLoadGDTR(GDTR_STARTADDRESS);
  kSetCursor(45, iCursorY++);
  kPrintf("Pass\n");

  kPrintf("TSS Segment Load............................[    ]");
  kLoadTR(GDT_TSSSEGMENT);
  kSetCursor(45, iCursorY++);
  kPrintf("Pass\n");

  kPrintf("IDT Initialize..............................[    ]");
  kInitializeIDTTables();
  kLoadIDTR(IDTR_STARTADDRESS);
  kSetCursor(45, iCursorY++);
  kPrintf("Pass\n");

  kPrintf("Total RAM Size Check........................[    ]");
  kCheckTotalRAMSize();
  kSetCursor(45, iCursorY++);
  kPrintf("Pass], Size = %d MB\n", kGetTotalRAMSize());

  kPrintf("TCB Pool And Scheduler Initialize...........[Pass]\n");
  iCursorY++;
  kInitializeScheduler();

  kPrintf("Dynamic Memory Initialize...................[Pass]\n");
  iCursorY++;
  kInitializeDynamicMemory();

  kInitializePIT(MSTOCOUNT(1), 1);

  kPrintf("Keyboard Activate And Queue Initialize......[    ]");
  if (kInitializeKeyboard()) {
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");
    kChangeKeyboardLED(FALSE, FALSE, FALSE);
  } else {
    kSetCursor(45, iCursorY++);
    kPrintf("Fail\n");
    while (1)
      ;
  }

  kPrintf("Mouse Activate And Queue Initialize.........[    ]");
  if (kInitializeMouse() == TRUE) {
    kEnableMouseInterrupt();
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");
  } else {
    kSetCursor(45, iCursorY++);
    kPrintf("Fail\n");
    while (1)
      ;
  }

  kPrintf("PIC Controller And Interrupt Initialize.....[    ]");
  kInitializePIC();
  kMaskPICInterrupt(0);
  kEnableInterrupt();
  kSetCursor(45, iCursorY++);
  kPrintf("Pass\n");

  kPrintf("File System Initialize......................[    ]");
  if (kInitializeFileSystem() == TRUE) {
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");
  } else {
    kSetCursor(45, iCursorY++);
    kPrintf("Fail\n");
  }

  kPrintf("Serial Port Initialize......................[Pass]\n");
  iCursorY++;
  kInitializeSerialPort();

  kPrintf("Change To MultiCore Processor Mode..........[    ]");
  if (kChangeToMultiCoreMode() == TRUE) {
    kSetCursor(45, iCursorY++);
    kPrintf("Pass\n");
  } else {
    kSetCursor(45, iCursorY++);
    kPrintf("Fail\n");
  }

  kPrintf("System Call MSR Initialize..................[Pass]\n");
  iCursorY++;
  kInitializeSystemCall();

  kCreateTask(TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM |
                  TASK_FLAGS_IDLE,
              0, 0, (QWORD)kIdleTask, kGetAPICID());

  if (*(BYTE *)VBE_STARTGRAPHICMODEFLAGADDRESS == 0)
    kStartConsoleShell();
  else
    kStartWindowManager();
}

void MainForApplicationProcessor() {
  QWORD qwTickCount;

  kLoadGDTR(GDTR_STARTADDRESS);

  kLoadTR(GDT_TSSSEGMENT + (kGetAPICID() * sizeof(GDTENTRY16)));

  kLoadIDTR(IDTR_STARTADDRESS);

  kInitializeScheduler();

  kEnableSoftwareLocalAPIC();

  kSetTaskPriority(0);

  kInitializeLocalVectorTable();

  kEnableInterrupt();

  kInitializeSystemCall();

  kIdleTask();
}

BOOL kChangeToMultiCoreMode() {
  MPCONFIGURATIONMANAGER *pstMPManager;
  BOOL bInterruptFlag;
  int i;

  if (kStartUpApplicationProcessor() == FALSE)
    return FALSE;

  pstMPManager = kGetMPConfigurationManager();
  if (pstMPManager->bUsePICMode == TRUE) {
    kOutPortByte(0x22, 0x70);
    kOutPortByte(0x23, 0x01);
  }

  kMaskPICInterrupt(0xFFFF);
  kEnableGlobalLocalAPIC();
  kEnableSoftwareLocalAPIC();
  bInterruptFlag = kSetInterruptFlag(FALSE);
  kSetTaskPriority(0);
  kInitializeLocalVectorTable();
  kSetSymmetricIOMode(TRUE);
  kInitializeIORedirectionTable();
  kSetInterruptFlag(bInterruptFlag);
  kSetInterruptLoadBalancing(TRUE);

  for (i = 0; i < MAXPROCESSORCOUNT; i++) {
    kSetTaskLoadBalancing(i, TRUE);
  }
  return TRUE;
}
