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

u8
VRAM_read(vram* ram, u16 address) {
	if(address >= PPU_PATTERN_TABLE0_ADDRESS && address < PPU_PATTERN_TABLE1_ADDRESS) {
		return ram->patternTableL[address];
	} else if( address < PPU_NAME_TABLE0_ADDRESS) {
		return ram->patternTableR[address - PPU_PATTERN_TABLE1_ADDRESS];
	} else if( address < PPU_NAME_TABLE1_ADDRESS) {
		return ram->nameTable0[address - PPU_NAME_TABLE0_ADDRESS];
	} else if( address < PPU_NAME_TABLE2_ADDRESS) {
		return ram->nameTable1[address - PPU_NAME_TABLE1_ADDRESS];
	} else if( address < PPU_NAME_TABLE3_ADDRESS) {
		return ram->nameTable2[address - PPU_NAME_TABLE2_ADDRESS];
	} else if( address < PPU_NAME_TABLE0_MIRROR) {
		return ram->nameTable3[address - PPU_NAME_TABLE3_ADDRESS];
	} else if( address < PPU_PALETTE_ADDRESS) {
		return ram->nameTable0[address - PPU_NAME_TABLE0_MIRROR];
	} else if( address < PPU_PALETTE_MIRROR) {
		return ram->palette[address - PPU_PALETTE_ADDRESS];
	} else if( address < PPU_VRAM_END ) {
		return ram->palette[address - PPU_PALETTE_MIRROR];
	} else {
		printf("this shouldn't happen you tried reading ram at: $%X\n", address);
	}
	return 0;
};

void 
ppu_tick(bppu* ppu)
{
	u32 dot = ppu->ticks % 341;
	// visible scanlines
	if(ppu->scanline < 240) {
		if(dot == 0) {
			ppu->ticks++;
			return;
		}
		// Load NT byte
		if( (dot -2) % 8 == 0) {
			ppu->currentNameTableReg = VRAM_read(ppu->VRAM, ppu->vRamAddress);
			ppu->vRamAddress++;
			// Load AT byte
		} else if ( dot >= 4 && (dot - 4) % 8 == 0) {
			ppu->currentAttribReg = VRAM_read(ppu->VRAM, ppu->vRamAddress + 0x3c0);
		} else if (dot >= 6 && (dot - 6) % 8 == 0) {
			ppu->nextTilePlane0 = VRAM_read(ppu->VRAM, ppu->currentNameTableReg + PPU_PATTERN_TABLE1_ADDRESS);
		} else if (dot >= 8 && (dot - 8) % 8 == 0) {
			ppu->nextTilePlane1 = VRAM_read(ppu->VRAM, ppu->currentNameTableReg + 8 + PPU_PATTERN_TABLE1_ADDRESS);
		}

		if(dot == 340) {
			ppu->scanline++;
		}

		//rendering output
		
		// vblank scanlines
	} else if( ppu->scanline >= 240 && ppu->scanline < 261) {
		if(dot == 340) {
			ppu->scanline++;
		}
		// dummy scanline
	} else {
		if(dot == 339 && !ppu->evenFrame) {
			ppu->scanline = 0;
			ppu->ticks = 0;
			ppu->evenFrame = !ppu->evenFrame;
		} else if(dot == 340) {
			ppu->scanline = 0;
			ppu->ticks = 0;
			ppu->evenFrame = !ppu->evenFrame;
		}
		return;
	}

}
