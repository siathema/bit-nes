#ifndef PPU_H_
#define PPU_H_
#include "bit_nes.h"

#define PPU_PATTERN_TABLE0_ADDRESS 0x0000
#define PPU_PATTERN_TABLE1_ADDRESS 0x1000
#define PPU_NAME_TABLE0_ADDRESS 0x2000
#define PPU_NAME_TABLE1_ADDRESS 0x2400
#define PPU_NAME_TABLE2_ADDRESS 0x2800
#define PPU_NAME_TABLE3_ADDRESS 0x2C00
  
struct vram {
	u8* patternTableL;
	u8* patternTableR;
	u8* nameTable0;
	u8* nameTable1;
	u8* nameTable2;
	u8* nameTable3;
};

struct bppu {
  bool evenFrame;
  u32 ticks;
  u32 scanline;
  u16 vRamAddress;
  u8  currentNameTableReg;
  u8  currentAttribReg;
  u8  nextTilePlane0;
  u8  nextTilePlane1;
  u16 tileShiftRegPlane0;
  u16 tileShiftRegPlane1;
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
	vram* VRAM;
};

bppu* init_ppu(u8* memory, cartridge* cart);
void run_ppu(bppu* ppu);
#endif
