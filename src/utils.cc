#include <stdio.h>
#include "bit_nes.h"
#include "6502.h"
#include "utils.h"
#include "log.h"

  void Instruction_Debug_Messege(nes* nes, u8* instruction)
  {
    char message[64] = {};

    u32 position =0;
    u16 counter = nes->cpu->PCReg;
    u16 address = (instruction[2] << 8) | instruction[1];
    u16 indirectZeroPageAddrY =  (((u16)read_memory(instruction[1]+1, nes) << 8) | (read_memory(instruction[1], nes)));
    u16 indirectZeroPageAddrX =  (((u16)read_memory((nes->cpu->XReg)+instruction[1]+1, nes) << 8) | (read_memory((nes->cpu->XReg)+instruction[1], nes)));

    Instruction ins = Instructions[*instruction];
    sprintf(message, "%04X  ", counter);
    Log(message);
    position = 6;

    for(i32 i=0; i<ins.Length; i++)
    {
      sprintf(message, "%02X ", instruction[i]);
      Log(message);
      position += 3;
    }

    while(position<16)
    {
      Log(" ");
      position++;
    }
    sprintf(message, "%s ", Mnemonics[ins.Name]);
    Log(message);
    position += 4;

    switch(ins.Mode)
    {
    case Implicit:
      break;
    case Accumulator:
      Log("A");
      position++;
      break;
    case Immediate:
      sprintf(message, "#$%02X", instruction[1]);
      Log(message);
      position += 4;
      break;
    case ZeroPage:
      sprintf(message, "$%02X = %02X", instruction[1], read_memory(instruction[1], nes));
      Log(message);
      position += 8;
      break;
    case ZeroPageX:
      sprintf(message, "$%02X,X @ %02X = %02X",
              instruction[1],
              (nes->cpu->XReg+instruction[1]),
              read_memory(instruction[1], nes));
      Log(message);
      position += 14;
      break;
    case ZeroPageY:
      sprintf(message, "$%02X,Y @ %02X = %02X",
              instruction[1],
              (nes->cpu->YReg+instruction[1]),
              read_memory(instruction[1], nes));
      Log(message);
      position += 14;
      break;
    case Relative:
      switch(ins.Name)
      {
      case BPL:
        if(!nes->cpu->Negative)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BMI:
        if(nes->cpu->Negative)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BVC:
        if(!nes->cpu->Overflow)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BVS:
        if(nes->cpu->Overflow)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BCC:
        if(!nes->cpu->Carry)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BCS:
        if(nes->cpu->Carry)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BNE:
        if(!nes->cpu->Zero)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;

      case BEQ:
        if(nes->cpu->Zero)
          sprintf(message, "$%04X", counter+(i8)instruction[1]);
        else
          sprintf(message, "$%04X", counter+2);
        break;
      default:
        break;
      }
      Log(message);
      position += 5;
      break;
    case Absolute:
      if(ins.Name == JMP || ins.Name == JSR)
      {
        sprintf(message, "$%04X", address);
        position += 5;
      }
      else
      {
        sprintf(message, "$%04X = %02X", address, read_memory(address, nes));
        position += 10;
      }
      Log(message);
      break;
    case AbsoluteX:
      sprintf(message, "$%04X,X @ %04X = %02X",
              address,
              (nes->cpu->XReg+address),
              read_memory(address, nes));
      Log(message);
      position += 19;
      break;
    case AbsoluteY:
      sprintf(message, "$%04X,Y @ %04X = %02X",
              address,
              (nes->cpu->YReg+address),
              read_memory(address, nes));
      Log(message);
      position += 19;
      break;
    case Indirect:
      sprintf(message, "($%04X) = %04X",
              address,
              (((u16)read_memory(address+1, nes) << 8) | (read_memory(address, nes))));
      Log(message);
      position += 13;
      break;
    case IndexedIndirect:
      sprintf(message, "($%02X,X) @ %02X = %04X = %02X",
              instruction[1],
              instruction[1]+nes->cpu->XReg,
              indirectZeroPageAddrX,
              read_memory(indirectZeroPageAddrX, nes));
      Log(message);
      position += 24;
      break;
    case IndirectIndexed:
      sprintf(message, "($%02X),Y = %04X @ %04X = %02X",
              instruction[1],
              indirectZeroPageAddrY,
              nes->cpu->YReg+indirectZeroPageAddrY,
              read_memory(nes->cpu->YReg+indirectZeroPageAddrY, nes));
      Log(message);
      position += 26;
      break;
    case Address_Mode_Count:
    default:
      Log("Invalid operation!");
      position += 21;
      break;
    }
    while(position<48)
    {
      Log(" ");
      position++;
    }

  }


const char* opcode_to_mnemonic(u8 opcode) {
  const char* mnemonic = Mnemonics[Instructions[opcode].Name];
  return mnemonic;
}
