#include "VBE.h"

static VBEMODEINFOBLOCK *gs_pstVBEModeBlockInfo =
    (VBEMODEINFOBLOCK *)VBE_MODEINFOBLOCKADDRESS;

inline VBEMODEINFOBLOCK *kGetVBEModeInfoBlock() {
  return gs_pstVBEModeBlockInfo;
}