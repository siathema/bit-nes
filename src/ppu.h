#ifndef PPU_H_
#define PPU_H_
#include "bit_nes.h"

namespace BITNES
{

#define PPU_PATTERN_TABLE0_ADDRESS 0x0000
#define PPU_PATTERN_TABLE1_ADDRESS 0x1000
#define PPU_NAME_TABLE0_ADDRESS 0x2000
#define PPU_NAME_TABLE1_ADDRESS 0x2400
#define PPU_NAME_TABLE2_ADDRESS 0x2800
#define PPU_NAME_TABLE3_ADDRESS 0x2C00

  struct bppu
  {
    u8* PPUCTRLreg;
    u8* PPUMASKreg;
    u8* PPUSTATUSreg;
    u8* OAMADDRreg;
    u8* OAMDATAreg;
    u8* PPUSCROLL;
    u8* PPUADDRreg;
    u8* PPUDATAreg;
    u8* OAMDMA;
    u8* Memory;
    u8* VRAM;
  };

  bppu* init_ppu(u8* memory);
  void run_ppu(bppu* ppu);

}
#endif
