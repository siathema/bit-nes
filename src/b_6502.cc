#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "b_6502.h"
#include "b_log.h"
#include "b_utils.h"
#include "bit_nes.h"

namespace BITNES
{

  b6502* init_cpu(u8 *memory) {
    b6502* cpu = (b6502*)calloc(1,sizeof(b6502));
    cpu->PCReg = 0xc000;
    cpu->SPReg = 0xFF;
    cpu->Carry = cpu->Zero = cpu->Interrupt = cpu->Decimal = cpu->Break = cpu->Overflow = cpu->Negative = false;
    cpu->memory = memory;
    return cpu;
}

void run_cpu(b6502 *cpu, bppu *ppu, nes* nes) {
  bool reset = false;

  while(!reset) {
    reset = run_opcode(&cpu->memory[cpu->PCReg], cpu, nes);

    for(int i=0; i<3; i++) {
      run_ppu(ppu);
    }
  }
}

bool run_opcode(u8 *opcodeAddress, b6502 *cpu, nes* nes) {
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
  u16 programAddress = 0;

  //NOTE(matthias): decode opcode
  switch(opcode) {

  case 0x10: //NOTE(matthias): BPL - Relative - Branch if Positive
    if(!(cpu->Negative)) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
    }
      cpu->PCReg += 2;
      cpu->cycles += 2;
    break;

  case 0x20: //NOTE(matthias): JSR - Absolute mode - Jump to subroutine
    printf("PC->%0X\n", cpu->PCReg);
    programAddress = cpu->PCReg;
    programAddress--;
    push_stack(cpu, programAddress>>8);
    push_stack(cpu, programAddress);
    cpu->PCReg = address;
    printf("PC->%0X\n", cpu->PCReg);
    cpu->cycles += 6;
    break;

  case 0x4C: //NOTE(matthias): JMP - absolute mode - Jump to address
    cpu->PCReg = address;
    cpu->cycles +=  3;
    break;

  case 0x78: //NOTE(matthias): SEI - Set Interrupt Disable
    cpu->Interrupt = true;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0x88: //NOTE(matthias): DEY - Implied mode - Decrement Y register
    cpu->YReg--;
    cpu->Zero = cpu->YReg == 0;
    cpu->Negative = (cpu->YReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0x85: //NOTE(matthias): STA - Zero Page - mode - Store Acumulator
    write_memory(opcodeAddress[1], cpu->AReg, nes);
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x86: //NOTE(matthias): STX - Zero Page - mode - Store X register
    write_memory(opcodeAddress[1], cpu->XReg, nes);
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x8D: //NOTE(matthias): STA - Absolute mode Store Accumulator
    write_memory(address, cpu->AReg, nes);
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x91: //NOTE(matthias): STA - (Indirect, Y) - Store Accumulator
    address = read_memory(opcodeAddress[1], nes) + cpu->YReg;
    write_memory(address, cpu->AReg, nes);
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x9a: //NOTE(matthias): TXS - Implied - Transfer X to Stack Pointer
    cpu->SPReg = cpu->XReg;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xa0: //NOTE(matthias): STY - Immediate - Store in Y register
    cpu->YReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = false;
    cpu->Zero = cpu->YReg == 0 ? true : false;
    cpu->Negative = (cpu->YReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xa2: //NOTE(matthias): LDX - Immediate mode - Load X Register
    cpu->XReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = false;
    cpu->Zero = cpu->XReg == 0 ? true : false;
    cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xa9: //NOTE(matthias): LDA - Immediate mode
    cpu->AReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xad: //NOTE(matthias): LDA - Absolute mode - Load Accumulator
    cpu->AReg = read_memory(address, nes);
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xB0: //NOTE(matthias): BCS - Relative mode - branch if carry set
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

  case 0xBD: //NOTE(matthias): LDA - Absolute,X mode - Load Accumulator
    cpu->AReg = read_memory(address + cpu->XReg, nes);
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false;
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xCA: //NOTE(matthias): DEX - Implied - Decrement X register
    cpu->XReg--;
    cpu->Zero = cpu->XReg == 0;
    cpu->Negative = (cpu->XReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xC9: //NOTE(matthias): CMP - Immediate mode - Compare Accumulator
    result = cpu->AReg - opcodeAddress[1];
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (cpu->AReg & 0x80) != 0;// A is Negative
    cpu->Carry = result <= 0; // A >= M
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xD0: //NOTE(matthias): BNE - Relative Mode - If Zero flag is clear branch
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

  case 0xD8: //NOTE(matthias): CLD - Clear Decimal Mode
    cpu->Decimal = false;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xE0: //NOTE(matthias): CPX - Immediate mode - Compare X register
    result = cpu->XReg - opcodeAddress[1];
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // X == M
    cpu->Negative = (cpu->XReg & 0x80) != 0;// A is Negative
    cpu->Carry = result <= 0; // X >= M
    cpu->PCReg += 2;
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

u8 pop_stack(b6502 *cpu) {
  u16 stackPointer = 0x100 | cpu->SPReg;
  cpu->SPReg++;
  return cpu->memory[stackPointer];
}

void push_stack(b6502 *cpu, u8 data) {
  u16 stackPointer = 0x0100 | cpu->SPReg;
  cpu->memory[stackPointer] = data;
  cpu->SPReg--;
}

}
