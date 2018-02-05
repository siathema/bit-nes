#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "b_6502.h"
#include "b_log.h"

#define KILOBYTE(size) (i64)(size * 1024)

namespace BITNES
{

  b6502* init_cpu(u8 *romBuffer, int romIndex) {
  b6502* cpu = (b6502*)calloc(1,sizeof(b6502));
  cpu->PCReg = 0x8000;
  cpu->Carry = cpu->Zero = cpu->Interrupt = cpu->Decimal = cpu->Break = cpu->Overflow = cpu->Negative = false;
  cpu->memory = (u8*)malloc(sizeof(char) * translate_address(0xffff));
  memset(cpu->memory, 0, translate_address(0xffff));
  memcpy(cpu->memory + translate_address(0x8000) ,romBuffer + romIndex, KILOBYTE(16));


  return cpu;
}

void run_cpu(b6502 *cpu, bppu *ppu) {
  bool reset = false;

  while(!reset) {
    reset = run_opcode(&cpu->memory[translate_address(cpu->PCReg)], cpu);

    for(int i=0; i<3; i++) {
      run_ppu(ppu);
    }
  }
}

bool run_opcode(u8 *opcodeAddress, b6502 *cpu) {
  u8 opcode = *opcodeAddress;
#if DEBUG_PRINT
  char message[18];
  sprintf(message, "Instruction: %s\n", opcode_to_mnemonic(opcode));
  Log(message);
#endif
  bool reset = false;

  u16 address = opcodeAddress[2] << 8;
  address |= opcodeAddress[1];
  u8 result = 0;

  // decode opcode
  switch(opcode) {

  case 0x10: // BPL - Relative - Branch if Positive
    if(!(cpu->Negative)) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
    }
      cpu->PCReg += 2;
      cpu->cycles += 2;
    break;

  case 0x78: // SEI - Set Interrupt Disable
    cpu->Interrupt = true;
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
    cpu->YReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = false;
    cpu->Zero = cpu->YReg == 0 ? true : false; // Setting zero flag
    cpu->Negative = (cpu->YReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xa2: // LDX - Immediate mode - Load X Register
    cpu->XReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = false;
    cpu->Zero = cpu->XReg == 0 ? true : false; // Setting zero flag
    cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xa9: // LDA - Immediate mode
    cpu->AReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false; // Setting zero flag
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xad: // LDA - Absolute mode - Load Accumulator
    cpu->AReg = cpu->memory[translate_address(address)];
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false; // Setting zero flag
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xB0: // BCS - Relative mode - branch if carry set
    if(cpu->Carry) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    } else
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xBD: // LDA - Absolute,X mode - Load Accumulator
    cpu->AReg = cpu->memory[translate_address(address + cpu->XReg)];
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false; // Setting zero flag
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xCA: // DEX - Implied - Decrement X register
    cpu->XReg--;
    cpu->Zero = cpu->XReg == 0;
    cpu->Negative = (cpu->XReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xC9: // CMP - Immediate mode - Compare Accumulator
    result = cpu->AReg - opcodeAddress[1];
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (cpu->AReg & 0x80) != 0;// A is Negative
    cpu->Carry = result <= 0; // A >= M
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xD0: // BNE - Relative Mode - If Zero flag is clear branck
    if(!cpu->Zero) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    } else
      cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xD8: // CLD - Clear Decimal Mode
    cpu->Decimal = false;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  default:
    printf("%02X  ", opcode&0x00ff);
    printf("%s\n", opcode_to_mnemonic(opcode));
    printf("Undefined opcode, halting\n");
    reset = true;
    break;
  }

  return reset;
}

}
