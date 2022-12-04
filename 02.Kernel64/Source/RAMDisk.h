#pragma once

#include "HardDisk.h"
#include "Synchronization.h"
#include "Types.h"

#define RDD_TOTALSECTORCOUNT (8 * 1024 * 1024 / 512)

#pragma pack(push, 1)

typedef struct kRDDManagerStruct {
  BYTE *pbBuffer;
  DWORD dwTotalSectorCount;
  MUTEX stMutex;
} RDDMANAGER;

#pragma pack(pop)

BOOL kInitializeRDD(DWORD dwTotalSectorCount);
BOOL kReadRDDInformation(BOOL bPrimary, BOOL bMaster,
                         HDDINFORMATION *pstHDDInformation);
int kReadRDDSector(BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount,
                   char *pcBuffer);
int kWriteRDDSector(BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount,
                    char *pcBuffer);