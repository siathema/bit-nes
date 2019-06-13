#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "6502.h"
#include "ops.h"
#include "log.h"
#include "utils.h"
#include "bit_nes.h"


  b6502* init_cpu(u8 *memory) {
    b6502* cpu = (b6502*)calloc(1,sizeof(b6502));
    cpu->PCReg = 0x8000;
    cpu->SPReg = 0xFD;
    cpu->Carry = cpu->Zero = cpu->Decimal = cpu->Break = cpu->Overflow = cpu->Negative, cpu->Reset = false;
    cpu->Interrupt = true;
    cpu->memory = memory;
    return cpu;
}

void run_cpu(b6502 *cpu) {

    run_opcode(&cpu->memory[cpu->PCReg], cpu);
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

void run_opcode(u8 *opcodeAddress, b6502 *cpu) {

  u8 opcode = *opcodeAddress;
#if DEBUG_PRINT
  char message[400];
  Instruction_Debug_Messege(cpu->console, opcodeAddress);
  sprintf(message,"A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n",  cpu->AReg, cpu->XReg, cpu->YReg, status_flags(cpu), cpu->SPReg, cpu->cycles);
  Log(message);
#endif

  Instruction instruction = Instructions[opcode];
  if(instruction.Mode == Invalid) {
    cpu->Reset = true;
    return;
  }

  OP_Ptr operation = Ops[instruction.Name];
  operation(opcodeAddress, cpu, instruction);
}

u8 pop_stack(b6502 *cpu) {
  u16 stackPointer = 0x0100 | ++cpu->SPReg;
  return read_memory(stackPointer, cpu->console);
}

void push_stack(b6502 *cpu, u8 data) {
  u16 stackPointer = 0x0100 | cpu->SPReg;
  //cpu->memory[stackPointer] = data;
  write_memory(stackPointer, data, cpu->console);
  cpu->SPReg--;
}
