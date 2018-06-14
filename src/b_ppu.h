#ifndef PPU_H_
#define PPU_H_
#include "bit_nes.h"

namespace BITNES
{

struct bppu{
  u8* PPUCTRLreg;
  u8* PPUMASKreg;
  u8* PPUSTATUSreg;
  u8* OAMADDRreg;
  u8* OAMDATAreg;
  u8* PPUSCROLL;
  u8* PPUADDRreg;
  u8* PPUDATAreg;
  u8* OAMDMA;
  u8* memory;
};

bppu* init_ppu(u8* memory);
void run_ppu(bppu* ppu);

}
#endif
