#ifndef _6502_H_
#define _6502_H_
#include <stdint.h>

#include "bit_nes.h"
#include "b_ppu.h"
#include "b_utils.h"



namespace BITNES
{

  struct b6502{
    u8 AReg, XReg, YReg, SPReg;
    bool Carry, Zero, Interrupt, Decimal, Break, Overflow, Negative;
    u16 PCReg;
    u8 *memory;
    uint cycles;
    nes *nes;
  };

  b6502* init_cpu(u8 *memory);
  void run_cpu(b6502 *cpu, bppu* ppu);
  bool run_opcode(u8 *opcodeAddress, b6502 *cpu);
  u8 pop_stack(b6502 *cpu);
  void push_stack(b6502 *cpu, u8 data);
}
#endif
