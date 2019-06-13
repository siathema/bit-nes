#include <stdlib.h>
#include <stdio.h>

#include "ppu.h"
#include "utils.h"

bppu*
init_ppu(u8* memory) {
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


	return ppu;
}

	void 
run_ppu(bppu* ppu)
{
	*ppu->PPUSTATUSreg = 0xff;
	// printf("%d\n", *ppu->PPUSTATUSreg);

}
