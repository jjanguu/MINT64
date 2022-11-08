#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

#define PAGE_FLAGS_P        0X00000001
#define PAGE_FLAGS_RW       0X00000002
#define PAGE_FLAGS_US       0X00000004
#define PAGE_FLAGS_PWT      0X00000008
#define PAGE_FLAGS_PCD      0X00000010
#define PAGE_FLAGS_A        0X00000020
#define PAGE_FLAGS_D        0X00000040
#define PAGE_FLAGS_PS       0X00000080
#define PAGE_FLAGS_G        0X00000100
#define PAGE_FLAGS_PAT      0X00001000
#define PAGE_FLAGS_EXB      0X80000000
#define PAGE_FLAGS_DEFAULT  (PAGE_FLAGS_P|PAGE_FLAGS_RW)
#define PAGE_TABLESIZE      0X1000
#define PAGE_MAXENTRYCOUNT  512
#define PAGE_DEFAULTSIZE    0x200000

#pragma pack (push, 1)

typedef struct kPageTableEntryStruct{
    DWORD dwAttributeAndLowerBaseAddress;
    DWORD dwUpperBaseAddressAndEXB;
    /* data */
}PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack (pop)

void kInitializePageTables();
void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);

#endif /*__PAGE_H__*/