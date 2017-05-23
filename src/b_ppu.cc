#include <stdlib.h>

#include "b_ppu.h"
#include "b_utils.h"

 bppu* init_ppu(u8* memory) {
 	bppu* ppu = (bppu*)malloc(sizeof(bppu));

 	ppu->PPUCTRLreg = memory + translate_address(0x2000);
    ppu->PPUMASKreg = memory + translate_address(0x2001);
  	ppu->PPUSTATUSreg = memory + translate_address(0x2002);
 	ppu->OAMADDRreg = memory + translate_address(0x2003);
  	ppu->OAMDATAreg = memory + translate_address(0x2004);
  	ppu->PPUSCROLL = memory + translate_address(0x2005);
 	ppu->PPUADDRreg = memory + translate_address(0x2006);
  	ppu->PPUDATAreg = memory + translate_address(0x2007);
	ppu->OAMDMA = memory + translate_address(0x4014);
 	ppu->memory = memory;

 	return ppu;
 }