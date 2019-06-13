#include <stdlib.h>
#include <stdio.h>

#include "ppu.h"
#include "utils.h"

bppu*
init_ppu(u8* memory, cartridge* cart) {
	bppu* ppu = (bppu*)malloc(sizeof(bppu));

	ppu->PPUCTRLreg = memory + 0x2000;
	ppu->PPUMASKreg = memory + 0x2001;
	ppu->PPUSTATUSreg = memory + 0x2002;
	ppu->OAMADDRreg = memory + 0x2003;
	ppu->OAMDATAreg = memory + 0x2004;
	ppu->PPUSCROLL = memory + 0x2005;
	ppu->PPUADDRreg = memory + 0x2006;
	ppu->PPUDATAreg = memory + 0x2007;
	ppu->OAMDMA = memory + 0x4014;
	ppu->Memory = memory;

	ppu->VRAM = (vram*)malloc(sizeof(vram));
	ppu->VRAM->patternTableL = cart->patternTableL;
	ppu->VRAM->patternTableR = cart->patternTableR;

	// NOTE(matthias): Nametable allocation. not final. TODO
	if(cart->nameTable0) {
		ppu->VRAM->nameTable0 = cart->nameTable0;
	} else {
		ppu->VRAM->nameTable0 = (u8*)calloc(1, KILOBYTE(1));
	}
	if(cart->nameTable1) {
		ppu->VRAM->nameTable1 = cart->nameTable1;
	} else {
		ppu->VRAM->nameTable1 = (u8*)calloc(1, KILOBYTE(1));
	}
	if(cart->nameTable2) {
		ppu->VRAM->nameTable2 = cart->nameTable2;
	} else {
		ppu->VRAM->nameTable2 = ppu->VRAM->nameTable0;
	}
	if(cart->nameTable3) {
		ppu->VRAM->nameTable3 = cart->nameTable3;
	} else {
		ppu->VRAM->nameTable3 = ppu->VRAM->nameTable1;
	}

	return ppu;
}

void 
run_ppu(bppu* ppu)
{
	*ppu->PPUSTATUSreg = 0xff;
	// printf("%d\n", *ppu->PPUSTATUSreg);

}
