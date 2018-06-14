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
    cpu->PCReg = 0x8000;
    cpu->SPReg = 0xFD;
    cpu->Carry = cpu->Zero = cpu->Decimal = cpu->Break = cpu->Overflow = cpu->Negative = false;
    cpu->Interrupt = true;
    cpu->memory = memory;
    return cpu;
}

void run_cpu(b6502 *cpu, bppu *ppu) {
  bool reset = false;

  while(!reset) {
    reset = run_opcode(&cpu->memory[cpu->PCReg], cpu);

    for(int i=0; i<3; i++) {
      run_ppu(ppu);
    }
  }
}

void set_status(u8 data, b6502 *cpu) {
  cpu->Carry = (data & 0x01) != 0;
  cpu->Zero = (data & 0x02) != 0;
  cpu->Interrupt = (data & 0x04) != 0;
  cpu->Decimal = (data & 0x08) != 0;
  cpu->Break = false;
  cpu->Overflow = (data & 0x40) != 0;
  cpu->Negative = (data & 0x80) != 0;
}

u8 status_flags(b6502 *cpu) {
  u8 result = 0;

  if(cpu->Carry)
    result |= 0x01;
  if(cpu->Zero)
    result |= 0x02;
  if(cpu->Interrupt)
    result |= 0x04;
  if(cpu->Decimal)
    result |= 0x08;
  //if(cpu->Break)
    //result |= 0x10;
  result |= 0x20;
  if(cpu->Overflow)
    result |= 0x40;
  if(cpu->Negative)
    result |= 0x80;

  return result;
}

bool run_opcode(u8 *opcodeAddress, b6502 *cpu) {
  u8 opcode = *opcodeAddress;
#if DEBUG_PRINT
  char message[400];

  sprintf(message,"%04X %02X %02X %02X %s            A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", cpu->PCReg, opcodeAddress[0], opcodeAddress[1], opcodeAddress[2], opcode_to_mnemonic(opcode), cpu->AReg, cpu->XReg, cpu->YReg, status_flags(cpu), cpu->SPReg, cpu->cycles);
  Log(message);

  //sprintf(message, "Carry: %d, Zero: %d, Interrupt: %d, Decimal: %d, Break: %d, Overflow: %d, Negative: %d\n", cpu->Carry, cpu->Zero, cpu->Interrupt, cpu->Decimal, cpu->Break, cpu->Overflow, cpu->Negative);
  //Log(message);
#endif
  bool reset = false;

  u16 address = opcodeAddress[2] << 8;
  address |= opcodeAddress[1];

  u8 result = 0;
  u16 result16 = 0;
  u16 programAddress = 0;

  //NOTE(matthias): decode opcode
  switch(opcode) {

  case 0x01: //NOTE(matthias): ORA - Indirect,X mode - Bit-wise OR
    programAddress = read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+cpu->XReg+1, cpu->nes) << 8;
    cpu->AReg |= read_memory(programAddress, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x05: //NOTE(matthias): ORA - Zero page mode - Bit-wise OR
    cpu->AReg |= read_memory(opcodeAddress[1], cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x08: //NOTE(matthias): PHP - Implied - Push status flags to stack
    //cpu->Break = true;
    push_stack(cpu, status_flags(cpu) | 0x10); //NOTE(matthias): PHP sets Break bit in status reg
    cpu->Break = true;
    cpu->PCReg++;
    cpu->cycles += 4;
    break;

  case 0x09: //NOTE(matthias): ORA - Immediate mode - Bit-wise OR
    cpu->AReg |= opcodeAddress[1];
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x0D: //NOTE(matthias): ORA - Absolute mode - Bit-wise OR
    cpu->AReg |= read_memory(address, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

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

  case 0x11: //NOTE(matthias): ORA - Indirect,Y - Bit-wise OR
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    cpu->AReg |= read_memory((programAddress+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 5;
    break;

  case 0x15: //NOTE(matthias): ORA - Zero page, X mode - Bit-wise OR
    cpu->AReg |= read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0x18: //NOTE(matthias): CLC - Implied mode - Clear carry
    cpu->Carry = false;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0x19: //NOTE(matthias): ORA - Absolute,Y mode - Bit-wise OR
    cpu->AReg |= read_memory((address+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x1D: //NOTE(matthias): ORA - Absolute,X mode - Bit-wise OR
    cpu->AReg |= read_memory((address+cpu->XReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x20: //NOTE(matthias): JSR - Absolute mode - Jump to subroutine
    //printf("PC->%0X\n", cpu->PCReg);
    programAddress = cpu->PCReg;
    //programAddress--;
    push_stack(cpu, programAddress>>8);
    push_stack(cpu, programAddress);
    cpu->PCReg = address;
    //printf("PC->%0X\n", cpu->PCReg);
    cpu->cycles += 6;
    break;

  case 0x21: //NOTE(matthias): AND - Indirect,X mode - Bit-wise and
    programAddress = read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+cpu->XReg+1, cpu->nes) << 8;
    cpu->AReg &= read_memory(programAddress, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x24: //NOTE(matthias): BIT - Zero page mode - Bit test
    result = read_memory(opcodeAddress[1], cpu->nes);
    cpu->Zero = (result & cpu->AReg) == 0;
    cpu->Overflow = (result&0x40) != 0;
    cpu->Negative = (result&0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x25: //NOTE(matthias): AND - Zero page mode - Bit-wise and
    cpu->AReg &= read_memory(opcodeAddress[1], cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x28: //NOTE(matthias): PLP - Implied mode - pop stack into status register
    set_status(pop_stack(cpu), cpu);
    cpu->PCReg += 1;
    cpu->cycles += 4;
    break;

  case 0x29: //NOTE(matthias): AND - Immediate mode - Bit-wise and
    cpu->AReg &= opcodeAddress[1];
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x2D: //NOTE(matthias): AND - Absolute mode - Bit-wise and
    cpu->AReg &= read_memory(address, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x30: //NOTE(matthias): BMI - Relative - Branch if Negative
    if((cpu->Negative)) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x31: //NOTE(matthias): AND - Indirect,Y - Bit-wise and
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    cpu->AReg &= read_memory((programAddress+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 5;
    break;

  case 0x35: //NOTE(matthias): AND - Zero page, X mode - Bit-wise AND
    cpu->AReg &= read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0x38: //NOTE(matthias): SEC - Implied mode - Set Carry Flag
    cpu->Carry = true;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0x39: //NOTE(matthias): AND - Absolute,Y mode - Bit-wise and
    cpu->AReg &= read_memory((address+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x3D: //NOTE(matthias): AND - Absolute,X mode - Bit-wise and
    cpu->AReg &= read_memory((address+cpu->XReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x41: //NOTE(matthias): EOR - Indirect,X mode - Bit-wise XOR
    programAddress = read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+cpu->XReg+1, cpu->nes) << 8;
    cpu->AReg ^= read_memory(programAddress, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x45: //NOTE(matthias): EOR - Zero page mode - Bit-wise XOR
    cpu->AReg ^= read_memory(opcodeAddress[1], cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x48: //NOTE(matthias): PHA - Implied mode - push A on the stack
    push_stack(cpu, cpu->AReg);
    cpu->PCReg += 1;
    cpu->cycles += 3;
    break;

  case 0x49: //NOTE(matthias): EOR - Immediate mode - Bit-wise XOR
    cpu->AReg ^= opcodeAddress[1];
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x4C: //NOTE(matthias): JMP - absolute mode - Jump to address
    cpu->PCReg = address;
    cpu->cycles +=  3;
    break;

  case 0x4D: //NOTE(matthias): EOR - Absolute mode - Bit-wise XOR
    cpu->AReg ^= read_memory(address, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x50: //NOTE(matthias): BVC - Relative - Branch if Overflow Clear
    if(!(cpu->Overflow)) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x51: //NOTE(matthias): EOR - Indirect,Y - Bit-wise XOR
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    cpu->AReg ^= read_memory((programAddress+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 5;
    break;

  case 0x55: //NOTE(matthias): EOR - Zero page mode, X - Bit-wise XOR
    cpu->AReg ^= read_memory(opcodeAddress[1] + cpu->XReg, cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0x59: //NOTE(matthias): EOR - Absolute,Y mode - Bit-wise XOR
    cpu->AReg ^= read_memory((address+cpu->YReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x5D: //NOTE(matthias): EOR - Absolute,X mode - Bit-wise XOR
    cpu->AReg ^= read_memory((address+cpu->XReg), cpu->nes);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x60: //NOTE(matthias): RTS - Implied Mode - Return from subroutine
    programAddress = pop_stack(cpu);
    programAddress |= pop_stack(cpu) << 8;
    cpu->PCReg = programAddress + 3;
    cpu->cycles += 6;
    break;

  case 0x61: //NOTE(matthias): ADC - Indirect mode,X  - Add with Carry
    programAddress = read_memory(opcodeAddress[1]+cpu->XReg, cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+cpu->XReg+1, cpu->nes) << 8;
    result16 = cpu->AReg + read_memory(programAddress, cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x65: //NOTE(matthias): ADC - Zero page mode - Add with Carry
    result16 = cpu->AReg + read_memory(opcodeAddress[1], cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x68: //NOTE(matthias): PLA - Implied - pull Accumulator
    cpu->AReg = pop_stack(cpu);
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 4;
    break;

  case 0x69: //NOTE(matthias): ADC - Immediate mode - Add with Carry
    result16 = cpu->AReg + opcodeAddress[1];
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x6D: //NOTE(matthias): ADC - Absolute mode - Add with Carry
    result16 = cpu->AReg + read_memory(address, cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x70: //NOTE(matthias): BVS - Relative - Branch if Overflow set
    if((cpu->Overflow)) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x71: //NOTE(matthias): ADC - Indirect mode,X  - Add with Carry
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    result16 = cpu->AReg + read_memory(programAddress + cpu->YReg, cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x75: //NOTE(matthias): ADC - Zero page mode, X - Add with Carry
    result16 = cpu->AReg + read_memory(opcodeAddress[1] + cpu->XReg, cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(~((cpu->AReg^ opcodeAddress[1])) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0x78: //NOTE(matthias): SEI - Set Interrupt Disable
    cpu->Interrupt = true;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;


  case 0x79: //NOTE(matthias): ADC - Absolute mode,Y  - Add with Carry
    result16 = cpu->AReg + read_memory((address+cpu->YReg), cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x7D: //NOTE(matthias): ADC - Absolute mode,X  - Add with Carry
    result16 = cpu->AReg + read_memory((address+cpu->XReg), cpu->nes);
    result16 += cpu->Carry ? 1 : 0;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = true;
    } else {
      cpu->Overflow = false;
    }
    cpu->Carry = result16 > 0xFF ? true : false;
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x88: //NOTE(matthias): DEY - Implied mode - Decrement Y register
    cpu->YReg--;
    cpu->Zero = cpu->YReg == 0;
    cpu->Negative = (cpu->YReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0x85: //NOTE(matthias): STA - Zero Page - mode - Store Acumulator
    write_memory(opcodeAddress[1], cpu->AReg, cpu->nes);
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x86: //NOTE(matthias): STX - Zero Page - mode - Store X register
    write_memory(opcodeAddress[1], cpu->XReg, cpu->nes);
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0x8A: //NOTE(matthias): TxA - implied mode - Tranfer X to A
	  cpu->AReg = cpu->XReg;
	  cpu->Zero = cpu->AReg == 0;
	  cpu->Negative = (cpu->AReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0x8D: //NOTE(matthias): STA - Absolute mode Store Accumulator
    write_memory(address, cpu->AReg, cpu->nes);
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0x8E: //NOTE(matthias): STX - Absolute mode - Store X register
	  write_memory(address, cpu->XReg, cpu->nes);
	  cpu->PCReg += 3;
	  cpu->cycles += 4;
	  break;

  case 0x90: //NOTE(matthia): BCC - Relative mode - Branck if carry is clear
    if(!cpu->Carry) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0x91: //NOTE(matthias): STA - (Indirect, Y) - Store Accumulator
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    write_memory(programAddress + cpu->YReg, cpu->AReg, cpu->nes);
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0x96: //NOTE(matthias): STX - Zero Page, Y - mode - Store X register
	  write_memory(opcodeAddress[1]+cpu->YReg, cpu->XReg, cpu->nes);
	  cpu->PCReg += 2;
	  cpu->cycles += 4;
	  break;

  case 0x98: //NOTE(matthias): TYA - implied mode - Tranfer Y to A
	  cpu->AReg = cpu->YReg;
	  cpu->Zero = cpu->AReg == 0;
	  cpu->Negative = (cpu->AReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0x9a: //NOTE(matthias): TXS - Implied - Transfer X to Stack Pointer
    cpu->SPReg = cpu->XReg;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xa0: //NOTE(matthias): LDY - Immediate - Store in Y register
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

  case 0xa6: //NOTE(matthias): LDX - Zero Page mode - Load X Register
	  cpu->XReg = read_memory(opcodeAddress[0], cpu->nes);
	  cpu->Zero = cpu->Negative = false;
	  cpu->Zero = cpu->XReg == 0 ? true : false;
	  cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
	  cpu->PCReg += 2;
	  cpu->cycles += 3;
	  break;

  case 0xA8: //NOTE(matthias): TAY - Implied mode - Transfer A to Y
	  cpu->YReg = cpu->AReg;
	  cpu->Zero = cpu->YReg == 0;
	  cpu->Negative = (cpu->YReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0xa9: //NOTE(matthias): LDA - Immediate mode
    cpu->AReg = opcodeAddress[1];
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xAA: //NOTE(matthias): TAX - Implied mode - Transfer A to X
	  cpu->XReg = cpu->AReg;
	  cpu->Zero = cpu->XReg == 0;
	  cpu->Negative = (cpu->XReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0xAD: //NOTE(matthias): LDA - Absolute mode - Load Accumulator
    cpu->AReg = read_memory(address, cpu->nes);
	printf("%X\n", read_memory(address, cpu->nes));
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false; // Setting negative flag
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xAE: //NOTE(matthias): LDX - Absolute mode - Load X Register
	  cpu->XReg = read_memory(address, cpu->nes);
	  cpu->Zero = cpu->Negative = false;
	  cpu->Zero = cpu->XReg == 0 ? true : false;
	  cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
	  cpu->PCReg += 3;
	  cpu->cycles += 4;
	  break;

  case 0xB0: //NOTE(matthias): BCS - Relative mode - branch if carry set
    if(cpu->Carry) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xB6: //NOTE(matthias): LDX - Zero Page, Y mode - Load X Register
	  cpu->XReg = read_memory(opcodeAddress[0] + cpu->YReg, cpu->nes);
	  cpu->Zero = cpu->Negative = false;
	  cpu->Zero = cpu->XReg == 0 ? true : false;
	  cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
	  cpu->PCReg += 2;
	  cpu->cycles += 4;
	  break;

  case 0xB8: //NOTE(matthias): CLV - Implied mode - clear Overflow flag
    cpu->Overflow = false;
    cpu->PCReg++;
    cpu->cycles +=2;
    break;

  case 0xBA: //NOTE(matthias): TSX - Implied mode - Transfer stack pointer to X
	  cpu->XReg = cpu->SPReg;
	  cpu->Zero = cpu->XReg == 0;
	  cpu->Negative = (cpu->XReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0xBD: //NOTE(matthias): LDA - Absolute,X mode - Load Accumulator
    cpu->AReg = read_memory(address + cpu->XReg, cpu->nes);
    cpu->Zero = cpu->Negative = 0;
    cpu->Zero = cpu->AReg == 0 ? true : false;
    cpu->Negative = (cpu->AReg & 0x80) != 0 ? true : false;
    cpu->PCReg += 3;
    cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    break;

  case 0xBE: //NOTE(matthias): LDX - Absolute, Y mode - Load X Register
	  cpu->XReg = read_memory(address+cpu->YReg, cpu->nes);
	  cpu->Zero = cpu->Negative = false;
	  cpu->Zero = cpu->XReg == 0 ? true : false;
	  cpu->Negative = (cpu->XReg & 0x80) != 0 ? true : false; // Setting negative flag
	  cpu->PCReg += 3;
	  cpu->cycles += 4; //(TODO)matthias: Implement cycle increase if crossing page boundry.
	  break;

  case 0xC0: //NOTE(matthias): CPY - Immediate mode - Compare Y Register
    result = cpu->YReg - opcodeAddress[1];
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (result & 0x80) != 0;// A is Negative
    cpu->Carry = cpu->YReg >= opcodeAddress[1]; // A >= M
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xC4: //NOTE(matthias): CPY - Zero Page mode - Compare Y Register
    result = cpu->YReg - read_memory(opcodeAddress[1], cpu->nes);
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (result & 0x80) != 0;// A is Negative
    cpu->Carry = cpu->YReg >= opcodeAddress[1]; // A >= M
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0xC8: //NOTE(matthias): INY - Implied mide - Increment Y Register
    cpu->YReg += 1;
    cpu->Zero = cpu->YReg == 0;
    cpu->Negative = (cpu->YReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles +=2;
    break;

  case 0xC9: //NOTE(matthias): CMP - Immediate mode - Compare Accumulator
    result = cpu->AReg - opcodeAddress[1];
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (result & 0x80) != 0;// A is Negative
    cpu->Carry = cpu->AReg >= opcodeAddress[1]; // A >= M
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xCA: //NOTE(matthias): DEX - Implied - Decrement X register
    cpu->XReg--;
    cpu->Zero = cpu->XReg == 0;
    cpu->Negative = (cpu->XReg & 0x80) != 0;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xCC: //NOTE(matthias): CPY - Absolute mode - Compare Y Register
    result = cpu->YReg - read_memory(address, cpu->nes);
    cpu->Zero = cpu->Negative = cpu->Carry = 0;
    cpu->Zero = result == 0; // A == M
    cpu->Negative = (result & 0x80) != 0;// A is Negative
    cpu->Carry = cpu->YReg >= opcodeAddress[1]; // A >= M
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0xD0: //NOTE(matthias): BNE - Relative Mode - If Zero flag is clear branch
    if(!cpu->Zero) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    }
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
    cpu->Negative = (result & 0x80) != 0;// A is Negative
    cpu->Carry = cpu->XReg >= opcodeAddress[1]; // X >= M
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xE1: //NOTE(matthias): SBC - Indirect,X  mode - Subtract with Carry
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    result16 = cpu->AReg - read_memory(programAddress, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 6;
    break;

  case 0xE5: //NOTE(matthias): SBC - Zero Page mode - Subtract with Carry
    result16 = cpu->AReg - read_memory(opcodeAddress[1], cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 3;
    break;

  case 0xE8: //NOTE(matthias): INX Implied mode - Increment X register
	  cpu->XReg++;
	  cpu->Zero = cpu->XReg == 0;
	  cpu->Negative = (cpu->XReg & 0x80) != 0;
	  cpu->cycles += 2;
	  cpu->PCReg++;
	  break;

  case 0xE9: //NOTE(matthias): SBC - Immediate mode - Subtract with Carry
    result16 = cpu->AReg - opcodeAddress[1];
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xEA: //NOTE(matthias): NOP - Implied mode - increments PC
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xED: //NOTE(matthias): SBC - Absolute mode - Subtract with Carry
    result16 = cpu->AReg - read_memory(address, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(~(cpu->AReg ^ opcodeAddress[1]) & (cpu->AReg ^ (u8)result16) & 0x80) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0xF0: //NOTE(matthia): BEQ - Relative mode - Branck if Zero is set
    if(cpu->Zero) {
      if((int8_t)opcodeAddress[1] > 0)
        cpu->PCReg += opcodeAddress[1];
      else
        cpu->PCReg += (int8_t)opcodeAddress[1];
      cpu->cycles++; //(TODO)matthias: Implement cycle increase if crossing page boundry.
    }
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;

  case 0xF1: //NOTE(matthias): SBC - Indirect,Y  mode - Subtract with Carry
    programAddress = read_memory(opcodeAddress[1], cpu->nes);
    programAddress |= read_memory(opcodeAddress[1]+1, cpu->nes) << 8;
    result16 = cpu->AReg - read_memory(programAddress + cpu->YReg, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 5;
    break;

  case 0xF5: //NOTE(matthias): SBC - Zero Page,X mode - Subtract with Carry
    result16 = cpu->AReg - read_memory(opcodeAddress[1] + cpu->XReg, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 2;
    cpu->cycles += 4;
    break;

  case 0xF8: //NOTE(matthias): SED - Implied - Set Decimal Flag
    cpu->Decimal = true;
    cpu->PCReg++;
    cpu->cycles += 2;
    break;

  case 0xF9: //NOTE(matthias): SBC - Absolute,Y  mode - Subtract with Carry
    result16 = cpu->AReg - read_memory(address + cpu->YReg, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  case 0xFD: //NOTE(matthias): SBC - Absolute,X  mode - Subtract with Carry
    result16 = cpu->AReg - read_memory(address + cpu->XReg, cpu->nes);
    result16 -= cpu->Carry ? 0 : 1;
    if(!((cpu->AReg^ opcodeAddress[1]) & ((cpu->AReg ^ (u8)result16) & 0x80))) {
      cpu->Overflow = false;
    } else {
      cpu->Overflow = true;
    }
    //cpu->Carry = !cpu->Carry;
    cpu->Carry = !(result16 > 0xFF);
    cpu->AReg = result16;
    cpu->Zero = cpu->AReg == 0;
    cpu->Negative = (cpu->AReg & 0x80) != 0;
    cpu->PCReg += 3;
    cpu->cycles += 4;
    break;

  default:
    sprintf(message,"%02X %s: Undefined opcode, halting\n", opcode&0x00ff, opcode_to_mnemonic(opcode));
    Log(message);
    printf(message);
    reset = true;
    break;
  }

  return reset;
}

u8 pop_stack(b6502 *cpu) {
  u16 stackPointer = 0x0100 | cpu->SPReg;
  cpu->SPReg++;
  return read_memory(stackPointer, cpu->nes);
}

void push_stack(b6502 *cpu, u8 data) {
  cpu->SPReg--;
  u16 stackPointer = 0x0100 | cpu->SPReg;
  //cpu->memory[stackPointer] = data;
  write_memory(stackPointer, data, cpu->nes);
}

}
