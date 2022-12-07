#include "AssemblyUtility.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Descriptor.h"
#include "DynamicMemory.h"
#include "FileSystem.h"
#include "HardDisk.h"
#include "Keyboard.h"
#include "PIC.h"
#include "PIT.h"
#include "SerialPort.h"
#include "Task.h"
#include "Types.h"
#include "Utility.h"

void kPrintString(int iX, int iY, const char *pcString);

void Main(void) {
  int iCursorX, iCursorY;

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

  kCreateTask(TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM |
                  TASK_FLAGS_IDLE,
              0, 0, (QWORD)kIdleTask);
  kStartConsoleShell();
}