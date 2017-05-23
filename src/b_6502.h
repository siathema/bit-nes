#ifndef _6502_H_
#define _6502_H_
#include <stdint.h>

#include "bit_nes.h"
#include "b_utils.h"

struct b6502{
  u8 AReg, XReg, YReg, SPReg, StatusReg;
  u16 PCReg;
  u8 *memory;
  uint cycles;
};

extern b6502* init_cpu(u8 *romBuffer, int romIndex);
extern void run_cpu(b6502 *cpu);
extern bool run_opcode(u8 *opcodeAddress, b6502 *cpu);

#endif
