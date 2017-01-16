#ifndef _6502_H_
#define _6502_H_
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

typedef struct {
  u8 AReg, XReg, YReg, SPReg, StatusReg;
  u16 PCReg;
  u8 *memory;
}b6502;

extern void run_instruction(u8 opcode, b6502 *cpu);

#endif
