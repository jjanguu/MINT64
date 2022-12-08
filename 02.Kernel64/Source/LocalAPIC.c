#include "LocalAPIC.h"
#include "MPConfigurationTable.h"

QWORD kGetLocalAPICBaseAddress() {
  MPCONFIGURATIONTABLEHEADER *pstMPHeader;

  pstMPHeader = kGetMPConfigurationManager()->pstMPConfigurationTableHeader;
  return pstMPHeader->dwMemoryMapIOAddressOfLocalAPIC;
}

void kEnableSoftwareLocalAPIC() {
  QWORD qwLocalAPICBaseAddress;

  qwLocalAPICBaseAddress = kGetLocalAPICBaseAddress();

  *(DWORD *)(qwLocalAPICBaseAddress + APIC_REGISTER_SVR) |= 0x100;
}