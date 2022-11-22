#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"
#include "Task.h"
#include "Synchronization.h"
#include "DynamicMemory.h"

SHELLCOMMANDENTRY gs_vstCommandTable[]={
    { "help", "Show Help", kHelp },
    { "clear", "Clear Screen", kClear },
    { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
    { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
    { "shutdown", "Shutdown And Reboot OS", kShutdown },
    { "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer },
    { "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
    { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
    { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
    { "date", "Show Date And Time", kShowDateAndTime },
    { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },
    { "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)",kChangeTaskPriority },
    { "tasklist", "Show Task List", kShowTaskList },
    { "killtask", "End Task, ex)killtask 1(ID) or 0xffffffff(All Task)", kKillTask },
    { "cpuload", "Show Processor Load", kCPULoad },
    { "testmutex", "Test Mutex Function", kTestMutex },
    { "testthread", "Test Thread And Process Function", kTestThread},
    { "showmatrix", "Show Matrix Screen", kShowMatrix},
    { "testpie", "Test PIE Calculation", kTestPIE} ,
    { "dynamicmeminfo", "Show Dyanmic Memory Information", kShowDynamicMemoryInformation },
    { "testseqalloc", "Test Sequential Allocation & Free", kTestSequentialAllocation },
    { "testranalloc", "Test Random Allocation & Free", kTestRandomAllocation },
};

void kStartConsoleShell(){
    char vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;

    kPrintf(CONSOLESHELL_PROMPTMESSAGE);

    while(1){
        bKey = kGetCh();

        if(bKey == KEY_BACKSPACE){
            if(iCommandBufferIndex > 0){
                kGetCursor(&iCursorX, &iCursorY);
                kPrintStringXY(iCursorX - 1, iCursorY, " ");
                kSetCursor(iCursorX - 1, iCursorY);
                iCommandBufferIndex--;
            }
        } else if(bKey == KEY_ENTER){
            kPrintf("\n");

            if(iCommandBufferIndex > 0){
                vcCommandBuffer[iCommandBufferIndex] = '\0';
                kExecuteCommand(vcCommandBuffer);
            }

            kPrintf("%s", CONSOLESHELL_PROMPTMESSAGE);
            kMemSet(vcCommandBuffer, '\0',CONSOLESHELL_MAXCOMMANDBUFFERCOUNT);
            iCommandBufferIndex = 0;
        } else if((bKey == KEY_LSHIFT) || (bKey == KEY_RSHIFT) || (bKey == KEY_CAPSLOCK) || (bKey == KEY_NUMLOCK) || (bKey == KEY_SCROLLLOCK)){
            ;
        } else{
            if (bKey == KEY_TAB)
                bKey = ' ';

            if(iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT){
                vcCommandBuffer[iCommandBufferIndex++] = bKey;
                kPrintf("%c", bKey);
            }
        }
    }
}

void kExecuteCommand(const char* pcCommandBuffer){
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;

    iCommandBufferLength = kStrLen(pcCommandBuffer);
    for(iSpaceIndex=0; iSpaceIndex<iCommandBufferLength; iSpaceIndex++){
        if(pcCommandBuffer[iSpaceIndex] == ' ')
            break;
    }

    iCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);
    for(i=0; i<iCount; i++){
        iCommandLength = kStrLen(gs_vstCommandTable[i].pcCommand);
        if((iCommandLength == iSpaceIndex) && (kMemCmp(gs_vstCommandTable[i].pcCommand, pcCommandBuffer, iSpaceIndex) == 0)){
            gs_vstCommandTable[i].pfFunction(pcCommandBuffer + iSpaceIndex + 1);
            break;
        }
    }

    if(i >= iCount)
        kPrintf("'%s' is not found.\n", pcCommandBuffer);
}

void kInitializeParameter(PARAMETERLIST* pstList, const char* pcParameter){
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen(pcParameter);
    pstList->iCurrentPosition = 0;
}

int kGetNextParameter(PARAMETERLIST* pstList, char* pcParameter){
    int i;
    int iLength;

    if(pstList->iLength <= pstList->iCurrentPosition)
        return 0;

    for(i=pstList->iCurrentPosition; i<pstList->iLength; i++){
        if(pstList->pcBuffer[i] == ' ')
            break;
    }

    kMemCpy(pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i);
    iLength = i - pstList->iCurrentPosition;
    pcParameter[iLength] = '\0';

    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}

static void kHelp(const char* pcCommandBuffer){
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;

    kPrintf( "=========================================================\n" );
    kPrintf( "||                  JJANGU Shell Help                  ||\n" );
    kPrintf( "=========================================================\n" );

    iCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

    for(i=0; i<iCount; i++){
        iLength = kStrLen(gs_vstCommandTable[i].pcCommand);
        if(iLength > iMaxCommandLength)
            iMaxCommandLength = iLength;
    }

    for(i=0; i<iCount; i++){
        kPrintf("%s", gs_vstCommandTable[i].pcCommand);
        kGetCursor(&iCursorX, &iCursorY);
        kSetCursor(iMaxCommandLength, iCursorY);
        kPrintf(" - %s\n", gs_vstCommandTable[i].pcHelp);
        
        if((i != 0 ) && ((i % 20) == 0)){
            kPrintf("Press and key to continue... ('q' is exit) : ");
            if(kGetCh() == 'q'){
                kPrintf("\n");
                break;
            }
            kPrintf("\n");
        }
    }
}

static void kClear(const char* pcParameterBuffer){
    kClearScreen();
    kSetCursor(0, 1);
}

static void kShowTotalRAMSize(const char* pcParameterBuffer){
    kPrintf("Total RAM Size = %d MB\n", kGetTotalRAMSize());
}

static void kStringToDecimalHexTest(const char* pcParameterBuffer){
    char vcParameter[100];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;

    kInitializeParameter(&stList, pcParameterBuffer);

    while(1){
        iLength = kGetNextParameter(&stList, vcParameter);
        if(!iLength)
            break;

        kPrintf("Param %d = '%s', Length = %d, ", iCount + 1, vcParameter, iLength);

        if(!kMemCmp(vcParameter, "0x", 2)){
            lValue = kAToI(vcParameter + 2, 16);
            kPrintf("HEX Value = %q\n", lValue);
        }else {
            lValue = kAToI(vcParameter, 10);
            kPrintf("Decimal Value = %d\n", lValue);
        }

        iCount++;
    }
}

static void kShutdown(const char* pcParameterBuffer){
    kPrintf("System Shutdown Start\n");

    kPrintf("Press Any Key To Reboot");
    kGetCh();
    kReboot();
}

static void kSetTimer(const char* pcParameterBuffer){
    char vcParameter[100];
    PARAMETERLIST stList;
    long lValue;
    BOOL bPeriodic;

    kInitializeParameter(&stList, pcParameterBuffer);

    if(kGetNextParameter(&stList, vcParameter) == 0){
        kPrintf("ex)settimer 10(ms 1(periodic)\n");
        return ;
    }
    lValue = kAToI(vcParameter, 10);

    if(kGetNextParameter(&stList, vcParameter) == 0){
        kPrintf("ex)settimer 10(ms) 1(periodic)\n");
        return ;
    }
    bPeriodic = kAToI(vcParameter, 10);

    kInitializePIT(MSTOCOUNT(lValue), bPeriodic);
    kPrintf("Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic);
}

static void kWaitUsingPIT(const char* pcParameterBuffer){
    char vcParameter[100];
    int iLength;
    PARAMETERLIST stList;
    long lMillisecond;
    int i;

    kInitializeParameter(&stList, pcParameterBuffer);
    if(kGetNextParameter(&stList, vcParameter) == 0){
        kPrintf("ex)wait 100(ms)\n");
        return ;
    }

    lMillisecond = kAToI(pcParameterBuffer, 10);
    kPrintf("%d ms Sleep Start...\n", lMillisecond);

    kDisableInterrupt();
    for(i=0; i<lMillisecond/30; i++){
        kWaitUsingDirectPIT(MSTOCOUNT(30));
    }
    kWaitUsingDirectPIT(MSTOCOUNT(lMillisecond % 30));
    kEnableInterrupt();
    kPrintf("%d ms Sleep Complete\n", lMillisecond);

    kInitializePIT(MSTOCOUNT(1), TRUE);
}

static void kReadTimeStampCounter(const char* pcParameterBuffer){
    QWORD qwTSC;

    qwTSC = kReadTSC();
    kPrintf("Time Stamp counter = %q\n", qwTSC);
}

static void kMeasureProcessorSpeed(const char* pcParameterBuffer){
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;

    kPrintf("Now Measuring.");

    kDisableInterrupt();
    for(i=0; i<200; i++){
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT(MSTOCOUNT(50));
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf(".");
    }

    kInitializePIT(MSTOCOUNT(1),TRUE);
    kEnableInterrupt();

    kPrintf("\nCPU Speed = %d MHz\n", qwTotalTSC/10/1000/1000);
}

static void kShowDateAndTime(const char* pcParameterBuffer){
    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    kReadRTCTime(&bHour, &bMinute, &bSecond);
    kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

    kPrintf("Data: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString(bDayOfWeek));
    kPrintf("Time: %d:%d:%d\n",bHour, bMinute, bSecond);
}

static TCB gs_vstTask[2] = {0, };
static QWORD gs_vstStack[1024] = {0, };

static void kTestTask1(){
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;

    pstRunningTask = kGetRunningTask();
    iMargin = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) % 10;

    for(j=0; j<20000; j++){
        switch (i)
        {
        case 0:
            iX++;
            if(iX >= (CONSOLE_WIDTH - iMargin))
                i = 1;
            break;

        case 1:
            iY++;
            if(iY >= (CONSOLE_HEIGHT - iMargin))
                i = 2;
            break;

        case 2:
            iX--;
            if(iX < iMargin)
                i = 3;
            break;

        case 3:
            iY--;
            if(iY < iMargin)
                i = 0;
            break;
        }

        pstScreen[iY * CONSOLE_WIDTH + iX].bCharactor = bData;
        pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0x0F;
        bData++;

        // kSchedule();
    }
    kExitTask();
}

static void kTestTask2(){
    int i =0, iOffset;
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[4] = {'-', '\\','|','/'};

    pstRunningTask = kGetRunningTask();
    iOffset = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));
    while(1){
        pstScreen[iOffset].bCharactor = vcData[i % 4];
        pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
        i++;
        // kSchedule();
    }
}

static void kCreateTestTask(const char* pcParameterBuffer){
    PARAMETERLIST stList;
    char vcType[30];
    char vcCount[30];
    int i;

    kInitializeParameter(&stList, pcParameterBuffer);
    kGetNextParameter(&stList, vcType);
    kGetNextParameter(&stList, vcCount);

    switch(kAToI(vcType, 10)){
        case 1:
            for(i=0;i<kAToI(vcCount, 10);i++){
                if(kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD,0 ,0 , (QWORD)kTestTask1) == NULL)
                    break;
            }
            kPrintf("Task1 %d Created\n", i);
            break;

        case 2:
        default:
            for(i=0;i<kAToI(vcCount, 10);i++){
                if(kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD,0 ,0, (QWORD)kTestTask2) == NULL)
                    break;
            }
            kPrintf("Task2 %d Created\n",i);
            break;
    }
}

static void kChangeTaskPriority(const char* pcParameterBuffer){
    PARAMETERLIST stList;
    char vcID[30];
    char vcPriority[30];
    QWORD qwID;
    BYTE bPriority;

    kInitializeParameter(&stList, pcParameterBuffer);
    kGetNextParameter(&stList,vcID);
    kGetNextParameter(&stList,vcPriority);

    if(kMemCmp(vcID, "0x", 2) == 0)
        qwID = kAToI(vcID + 2, 16);
    else
        qwID = kAToI(vcID, 10);

    bPriority = kAToI(vcPriority, 10);

    kPrintf("Change Task Priority ID [0x%q] Priority[%d]", qwID, bPriority);
    if(kChangePriority(qwID,bPriority) == TRUE)
        kPrintf("Success\n");
    else
        kPrintf("Fail\n");
}

static void kShowTaskList(const char* pcParameterBuffer){
    int i;
    TCB* pstTCB;
    int iCount = 0;

    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for(i=0; i<TASK_MAXCOUNT; i++){
        pstTCB = kGetTCBInTCBPool(i);
        if((pstTCB->stLink.qwID >> 32) != 0){
            if((iCount != 0) && ((iCount % 10) == 0)){
                kPrintf("Press and key to continue... ('q' is exit) : ");
                if(kGetCh() == 'q'){
                    kPrintf("\n");
                    break;
                }
                kPrintf("\n");
            }

            
            kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q], Thread[%d]\n", 1 + iCount++, 
                pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ),pstTCB->qwFlags, kGetListCount(&(pstTCB->stChildThreadList)));

            kPrintf("    Parent PID[0x%Q], Memory Address[0x%Q], Size[0x%Q]\n", pstTCB->qwParentProcessID, pstTCB->pvMemoryAddress, pstTCB->qwMemorySize);
        }
    }
}

static void kKillTask(const char* pcParameterBuffer){
    PARAMETERLIST stList;
    char vcID[30];
    QWORD qwID;
    TCB* pstTCB;
    int i;

    kInitializeParameter(&stList, pcParameterBuffer);
    kGetNextParameter(&stList, vcID);

    if(kMemCmp(vcID, "0x", 2) == 0)
        qwID = kAToI(vcID + 2, 16);
    else
        qwID = kAToI(vcID, 10);
    
    if(qwID != 0xFFFFFFFF){
        pstTCB = kGetTCBInTCBPool(GETTCBOFFSET(qwID));
        qwID = pstTCB->stLink.qwID;

        if(((qwID >> 32) != 0) && ((pstTCB->qwFlags & TASK_FLAGS_SYSTEM) == 0x00)){
            kPrintf("Kill Task ID [0x%q]", qwID);
            if(kEndTask(qwID) == TRUE)
                kPrintf("Success\n");
            else
                kPrintf("Fail\n");
        }else
            kPrintf("Task does not exist or task is system task\n");
    } else{
        for(i=0; i<TASK_MAXCOUNT; i++){
            pstTCB = kGetTCBInTCBPool(i);
            qwID = pstTCB->stLink.qwID;

            if(((qwID >> 32) != 0) && ((pstTCB->qwFlags & TASK_FLAGS_SYSTEM) == 0x00)){
                kPrintf("Kill Task ID [0x%q]", qwID);
                if(kEndTask(qwID) == TRUE)
                    kPrintf("Success\n");
                else
                    kPrintf("Fail\n");
            }
        }
    }
}

static void kCPULoad(const char* pcParameterBuffer){
    kPrintf("Processor Load : %d%%\n",kGetProcessorLoad());
}

static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

static void kPrintNumberTask(){
    int i, j;
    QWORD qwTickCount;

    qwTickCount = kGetTickCount();
    while((kGetTickCount() - qwTickCount) < 50){kSchedule();}

    for(i=0; i<5; i++){
        kLock(&(gs_stMutex));
        kPrintf("Task ID [0x%Q] Value[%d]\n", kGetRunningTask()->stLink.qwID, gs_qwAdder);

        gs_qwAdder += 1;
        kUnlock(&(gs_stMutex));

        for(j=0; j<300000; j++);
    }

    qwTickCount = kGetTickCount();
    while((kGetTickCount() - qwTickCount) < 1000){kSchedule();}

    kExitTask();
}

static void kTestMutex(const char* pcParameterBuffer){
    int i;

    gs_qwAdder = 1;

    kInitializeMutex(&gs_stMutex);

    for(i=0; i<3; i++){
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD)kPrintNumberTask);
    }
    kPrintf("Wait Utill %d Task End...\n",i);
    kGetCh();
}

static void kCreateThreadTask(){
    int i;

    for(i=0; i<3; i++){
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD)kTestTask2);
    }

    while(1){kSleep(1);}
}

static void kTestThread(const char* pcParameterBuffer){
    TCB* pstProcess;

    pstProcess = kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_PROCESS, (void *)0xEEEEEEEE, 0x1000, (QWORD)kCreateThreadTask);

    if(pstProcess != NULL)
        kPrintf("Process [0x%Q] Create Success\n", pstProcess->stLink.qwID);
    else
        kPrintf("Process Create Fail\n");
}

static volatile QWORD gs_qwRandomValue = 0;

QWORD kRandom( void )
{
    gs_qwRandomValue = ( gs_qwRandomValue * 412153 + 5571031 ) >> 16;
    return gs_qwRandomValue;
}

static void kDropCharactorThread( void )
{
    int iX, iY;
    int i;
    char vcText[ 2 ] = { 0, };

    iX = kRandom() % CONSOLE_WIDTH;
    
    while( 1 )
    {
        kSleep( kRandom() % 20 );
        
        if( ( kRandom() % 20 ) < 16 )
        {
            vcText[ 0 ] = ' ';
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                kPrintStringXY( iX, i , vcText );
                kSleep( 50 );
            }
        }        
        else
        {
            for( i = 0 ; i < CONSOLE_HEIGHT - 1 ; i++ )
            {
                vcText[ 0 ] = i + kRandom();
                kPrintStringXY( iX, i, vcText );
                kSleep( 50 );
            }
        }
    }
}

static void kMatrixProcess( void )
{
    int i;
    
    for( i = 0 ; i < 300 ; i++ )
    {
        if( kCreateTask( TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, ( QWORD ) kDropCharactorThread ) == NULL )
            break;
        
        kSleep( kRandom() % 5 + 5 );
    }
    
    kPrintf( "%d Thread is created\n", i );

    kGetCh();
}

static void kShowMatrix( const char* pcParameterBuffer )
{
    TCB* pstProcess;
    
    pstProcess = kCreateTask( TASK_FLAGS_PROCESS | TASK_FLAGS_LOW, ( void* ) 0xE00000, 0xE00000, 
                              ( QWORD ) kMatrixProcess );
    if( pstProcess != NULL )
    {
        kPrintf( "Matrix Process [0x%Q] Create Success\n" );

        while( ( pstProcess->stLink.qwID >> 32 ) != 0 )
        {
            kSleep( 100 );
        }
    }
    else
        kPrintf( "Matrix Process Create Fail\n" );
}

static void kFPUTestTask(){
    double dValue1;
    double dValue2;
    TCB* pstRunningTask;
    QWORD qwCount = 0;
    QWORD qwRandomValue;
    int i;
    int iOffset;
    char vcData[4] = {'-', '\\', '|','/'};
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;

    pstRunningTask = kGetRunningTask();

    iOffset = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));

    while(1){
        dValue1 = 1;
        dValue2 = 1;

        for(i=0; i<10; i++){
            qwRandomValue = kRandom();
            dValue1 *= (double)qwRandomValue;
            dValue2 *= (double)qwRandomValue;

            kSleep(1);

            qwRandomValue = kRandom();
            dValue1 /= (double)qwRandomValue;
            dValue2 /= (double)qwRandomValue;
        }

        if(dValue1 != dValue2){
            kPrintf("Value Is Not Same [%f] != [%f]\n", dValue1, dValue2);
            break;
        }
        qwCount++;

        pstScreen[iOffset].bCharactor = vcData[qwCount % 4];
        pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
    }
}

static void kTestPIE(const char* pcParameterBuffer){
    double dResult;
    int i;

    kPrintf("PIE Cacluation Test\n");
    kPrintf("Result: 355 / 113 = ");
    dResult = (double) 355/113;
    kPrintf("%d.%d%d\n",(QWORD)dResult, ((QWORD)(dResult * 10) % 10), ((QWORD) (dResult * 100) % 10));

    for(i=0; i<100; i++){
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD) kFPUTestTask);
    }
}

static void kShowDynamicMemoryInformation(const char* pcParameterBuffer){
    QWORD qwStartAddress, qwTotalSize, qwMetaSize, qwUsedSize;

    kGetDynamicMemoryInformation(&qwStartAddress, &qwTotalSize, &qwMetaSize, &qwUsedSize);

    kPrintf("============ Dynamic Memory Information ============\n" );
    kPrintf("Start Address: [0x%Q]\n", qwStartAddress);
    kPrintf("Total Size:    [0x%Q]byte, [%d]MB\n", qwTotalSize, BTOMB(qwTotalSize));
    kPrintf("Meta Size:     [0x%Q]byte, [%d]KB\n",qwMetaSize, BTOKB(qwMetaSize));
    kPrintf("Used Size:     [0x%Q]byte, [%d]KB\n",qwUsedSize, BTOKB(qwUsedSize));
}

static void kTestSequentialAllocation(const char* pcParameterBuffer){
    DYNAMICMEMORY* pstMemory;
    long i, j, k;
    QWORD* pqwBuffer;

    kPrintf("============ Dynamic Memory Test ============\n");
    pstMemory = kGetDynamicMemoryManager();

    for(i=0; i<pstMemory->iMaxLevelCount; i++){
        kPrintf("Block List [%d] Test Start\n", i);
        kPrintf("Allocation And Compare: ");

        for(j=0; j<(pstMemory->iBlockCountOfSmallestBlock >> i); j++){
            pqwBuffer = kAllocateMemory(DYNAMICMEMORY_MIN_SIZE << i);
            if(pqwBuffer == NULL){
                kPrintf("\nAllocation Fail\n");
                return ;
            }

            for(k=0; k<(DYNAMICMEMORY_MIN_SIZE << i) / 8; k++){
                pqwBuffer[k] = k;
            }

            for(k=0; k<(DYNAMICMEMORY_MIN_SIZE << i) / 8; k++){
                if(pqwBuffer[k] != k){
                    kPrintf("\nCompare Fail\n");
                    return ;
                }
            }
            kPrintf(".");
        }
        kPrintf("\nFree: ");

        for(j=0; j<(pstMemory->iBlockCountOfSmallestBlock >> i); j++){
            if(kFreeMemory((void *)(pstMemory->qwStartAddress + (DYNAMICMEMORY_MIN_SIZE << i) * j)) == FALSE){
                kPrintf("\nFree Fail\n");
                return ;
            }
            kPrintf(".");
        }
        kPrintf("\n");
    }
    kPrintf("Test Complete\n");
}

static void kRandomAllocationTask(){
    TCB* pstTask;
    QWORD qwMemorySize;
    char vcBuffer[200];
    BYTE* pbAllocationBuffer;
    int i, j;
    int iY;

    pstTask = kGetRunningTask();
    iY = (pstTask->stLink.qwID) % 15 + 9;

    for(j=0; j<10; j++){
        do{
            qwMemorySize = ((kRandom() % (32 * 1024)) + 1) * 1024;
            pbAllocationBuffer = kAllocateMemory(qwMemorySize);

            if(pbAllocationBuffer == 0)
                kSleep(1);
        }while(pbAllocationBuffer == 0);

        kSPrintf(vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Allocation Success", pbAllocationBuffer, qwMemorySize);
        kPrintStringXY(20, iY, vcBuffer);
        kSleep(200);

        kSPrintf(vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Verify...    ", pbAllocationBuffer, qwMemorySize);
        kPrintStringXY(20, iY, vcBuffer);
        for(i=0; i<(qwMemorySize/2); i++){
            pbAllocationBuffer[i] = kRandom() & 0xFF;
            pbAllocationBuffer[i + (qwMemorySize / 2)] = pbAllocationBuffer[i];
        }
        kSleep(200);

        kSPrintf(vcBuffer, "|Address: [0x%Q] Size: [0x%Q] Data Verify...    ", pbAllocationBuffer, qwMemorySize);
        kPrintStringXY(20, iY, vcBuffer);

        for(i=0; i<qwMemorySize/2; i++){
            if(pbAllocationBuffer[i] != pbAllocationBuffer[i + (qwMemorySize / 2)]){
                kPrintf("Task ID[0x%Q] Verify Fail\n", pstTask->stLink.qwID);
                kExitTask();
            }
        }
        kFreeMemory(pbAllocationBuffer);
        kSleep(200);
    }
    kExitTask();
}
static void kTestRandomAllocation(const char* pcParameterBuffer){
    int i;

    for(i=0; i<1000; i++){ 
        kCreateTask(TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD, 0, 0, (QWORD)kRandomAllocationTask);
    }
}