#ifndef _6502_H_
#define _6502_H_
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef unsigned int uint;

struct b6502{
  u8 AReg, XReg, YReg, SPReg, StatusReg;
  u16 PCReg;
  u8 *memory;
  uint cycles;
};

extern b6502* init_cpu();
extern void run_cpu(b6502 *cpu);
extern void run_opcode(u8 *opcodeAddress, b6502 *cpu);

#endif
