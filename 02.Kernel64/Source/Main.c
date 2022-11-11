#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"

#define PRINT_PASS(line) kPrintString(45, line, "Pass");
#define PRINT_FAIL(line) kPrintString(45, line, "Fail");

void kPrintString( int iX, int iY, const char* pcString );

void Main( void ){
    char vcTemp[2] = {0, };
    BYTE bFlags;
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;

    kPrintString( 0, 10, "Switch To IA-32e Mode Success" );
    kPrintString( 0, 11, "IA-32e C Language Kernel Start..............[Pass]" );
    
    kPrintString( 0, 12, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR(GDTR_STARTADDRESS);
    PRINT_PASS(12)

    kPrintString( 0, 13, "TSS Segment Load............................[    ]" );
    kLoadTR(GDT_TSSSEGMENT);
    PRINT_PASS(13)

    kPrintString( 0, 14, "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();
    kLoadIDTR(IDTR_STARTADDRESS);
    PRINT_PASS(14)

    kPrintString( 0, 15, "Keyboard Activate And Queue Initialize......[    ]" );
    if(kInitializeKeyboard()){
        PRINT_PASS(15)
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    } else{
        PRINT_FAIL(15)
        while (1);
    }

    kPrintString( 0, 16, "PIC Controller And Interrupt Initialize.....[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    PRINT_PASS(16);

    while(1){
        if(kGetKeyFromKeyQueue(&stData)){
            if(stData.bFlags & KEY_FLAGS_DOWN){
                vcTemp[0] = stData.bASCIICode;
                kPrintString(i++, 17, vcTemp);

                if(stData.bASCIICode == '0') bTemp=bTemp/0;
            }
        }
    }


}

void kPrintString( int iX, int iY, const char* pcString){
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;

    pstScreen += (iY * 80) + iX;
    for( i=0; pcString[i] !=0; i++){
        pstScreen[i].bCharactor = pcString[i];
    }
}

