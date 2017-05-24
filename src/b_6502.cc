#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "b_6502.h"

#define KILOBYTE_16 16384

b6502* init_cpu(u8 *romBuffer, int romIndex) {
  b6502* cpu = (b6502*)malloc(sizeof(b6502));
  cpu->PCReg = 0x8000;
  cpu->memory = (u8*)malloc(sizeof(char) * translate_address(0xffff));
  memcpy(cpu->memory + translate_address(0x8000) ,romBuffer + romIndex, KILOBYTE_16);

  return cpu;
}

void run_cpu(b6502 *cpu) {
  bool reset = false;

  while(!reset) {
    reset = run_opcode(&cpu->memory[translate_address(cpu->PCReg)], cpu);
  }
}

bool run_opcode(u8 *opcodeAddress, b6502 *cpu) {
  u8 opcode = *opcodeAddress;

  bool reset = false;

  u16 address = opcodeAddress[1] << 8;
  address |= opcodeAddress[2];

  // decode opcode
  switch(opcode) {

  case 0x10: // BPL - Relative - Branch if Positive
  if((cpu->StatusReg & 0x40) != 0) {
    cpu->PCReg += (char)opcodeAddress[1];
  }
  cpu->PCReg += 2;
  cpu->cycles += 2;
  break;

  case 0x78: // SEI - Set Interrupt Disable
  cpu->StatusReg |= 0x04;
  cpu->PCReg++;
  cpu->cycles += 2;
  break;

  case 0x8D: // STA - Absolute mode Store Accumulator
  cpu->memory[translate_address(address)] = cpu->AReg;
  cpu->PCReg += 3;
  cpu->cycles += 4;
  break;

  case 0x9a: // TXS - Implied - Transfer X to Stack Pointer
  cpu->SPReg = cpu->XReg;
  cpu->PCReg++;
  cpu->cycles += 2;
  break;

  case 0xa0:
  printf("A0 not implemented\n");
  break;

  case 0xa2: // LDX - Immediate mode - Load X Register
  cpu->XReg = opcodeAddress[1];
    cpu->StatusReg |= cpu->XReg == 0 ? 0x02 : 0x00; // Setting zero flag
    cpu->StatusReg |= (cpu->XReg & 0x40) != 0 ? 0x40 : 0x00; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xa9: // LDA - Immediate mode
  cpu->AReg = opcodeAddress[1];
    cpu->StatusReg |= cpu->AReg == 0 ? 0x02 : 0x00; // Setting zero flag
    cpu->StatusReg |= (cpu->AReg & 0x40) != 0 ? 0x40 : 0x00; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xad: // LDA - Absolute mode - Load Accumulator
  cpu->AReg = cpu->memory[translate_address(address)];
    cpu->StatusReg |= cpu->AReg == 0 ? 0x02 : 0x00; // Setting zero flag
    cpu->StatusReg |= (cpu->AReg & 0x40) != 0 ? 0x40 : 0x00; // Setting negative flag
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0xD8: // CLD - Clear Decimal Mode
  cpu->StatusReg &= 0xf7;
  cpu->PCReg++;
  cpu->cycles += 2;
  break;

  default:
  printf("%02X  ", opcode&0x00ff);
  printf("Undefined opcode, halting\n");
  reset = true;
  break;
}

return reset;
}

