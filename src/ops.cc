#include "stdio.h"
#include "stdlib.h"
#include "log.h"
#include "ops.h"

#define ZERO(value) (cpu->Zero = (value) == 0)
#define NEGATIVE(value) (cpu->Negative = (((value) & 0x80) != 0))

    Internal u16 Absolute_Address(u8 low, u8 high)
    {
        u16 result = ((u16)high<<8) | low;
        return result;
    }

    Internal u8 Get_Data(u8* opcodeAddr, b6502* cpu, Instruction instruction)
    {
        u8 result = 0;

        switch(instruction.Mode)
        {
            case Accumulator:
            {
                result = cpu->AReg;
            }break;

            case Immediate:
            {
                result = opcodeAddr[1];
            }break;

            case ZeroPage:
            {
                u16 tableAddress = opcodeAddr[1] & 0x00FF;
                result = read_memory(tableAddress, cpu->console);
            }break;

            case ZeroPageX:
            {
                u16 address = (opcodeAddr[1] + cpu->XReg) & 0x00FF;
                result = read_memory(address, cpu->console);
            }break;

            case ZeroPageY:
            {
                u16 address = (opcodeAddr[1] + cpu->YReg) & 0x00FF;
                result = read_memory(address, cpu->console);
            }break;

            case Absolute:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                result = read_memory(address, cpu->console);
            }break;

            case AbsoluteX:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                address += cpu->XReg;
                result = read_memory(address, cpu->console);
            }break;

            case AbsoluteY:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                address += cpu->YReg;
                result = read_memory(address, cpu->console);
            }break;

            case IndexedIndirect:
            {
                u16 tableAddress = (u8)opcodeAddr[1] + (u8)cpu->XReg;
                tableAddress &= 0x00FF;
                u16 address = read_memory(tableAddress, cpu->console);
                tableAddress = (tableAddress+1) & 0x00FF;
                address |= read_memory(tableAddress, cpu->console) << 8;
                result = read_memory(address, cpu->console);
            }break;

            case IndirectIndexed:
            {
                u16 tableAddress = opcodeAddr[1] & 0x00FF;
                u16 address = read_memory(tableAddress, cpu->console);
                tableAddress = (tableAddress+1) & 0x00FF;
                address |= read_memory(tableAddress, cpu->console) << 8;
                address += cpu->YReg;
                result = read_memory(address, cpu->console);
            }break;

            default:
            {
                Log("Invalid Addressing Mode\n");
                cpu->Reset = true;
            }break;
        }
        return result;
    }

    Internal void Set_Data(u8 byte, u8* opcodeAddr, b6502* cpu, Instruction instruction)
    {
        switch(instruction.Mode)
        {
            case Accumulator:
            {
                cpu->AReg = byte;
            }break;

            case Immediate:
            {
                Log("Implement me\n");
                char msg[16];
                sprintf(msg, "%d ", __LINE__);
                Log(msg);
                Log(__FILE__);
                cpu->Reset = true;
            }break;

            case ZeroPage:
            {
                u16 tableAddress = opcodeAddr[1] & 0x00FF;
                write_memory(tableAddress, byte, cpu->console);
            }break;

            case ZeroPageX:
            {
                u16 address = (opcodeAddr[1] + cpu->XReg) & 0x00FF;
                write_memory(address, byte, cpu->console);
            }break;

            case ZeroPageY:
            {
                u16 address = (opcodeAddr[1] + cpu->YReg) & 0x00FF;
                write_memory(address, byte, cpu->console);
            }break;

            case Absolute:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                write_memory(address, byte, cpu->console);
            }break;

            case AbsoluteX:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                address += cpu->XReg;
                write_memory(address, byte, cpu->console);
            }break;

            case AbsoluteY:
            {
                u16 address = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                address += cpu->YReg;
                write_memory(address, byte, cpu->console);
            }break;

            case IndexedIndirect:
            {
                u16 tableAddress = (u8)opcodeAddr[1] + (u8)cpu->XReg;
                tableAddress &= 0x00FF;
                u16 address = read_memory(tableAddress, cpu->console);
                tableAddress = (tableAddress+1) & 0x00FF;
                address |= read_memory(tableAddress, cpu->console) << 8;
                write_memory(address, byte, cpu->console);
            }break;

            case IndirectIndexed:
            {
                u16 tableAddress = opcodeAddr[1] & 0x00FF;
                u16 address = read_memory(tableAddress, cpu->console);
                tableAddress = (tableAddress+1) & 0x00FF;
                address |= read_memory(tableAddress, cpu->console) << 8;
                address += cpu->YReg;
                write_memory(address, byte, cpu->console);
            }break;

            default:
            {
                Log("Invalid Addressing Mode\n");
                cpu->Reset = true;
            }break;
        }

    }

    OP(_ORA) //NOTE(matthias): ORA - Bit-wise OR
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->AReg |= dataRead;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_AND) //NOTE(matthias): AND - Bit-wise and
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->AReg &= dataRead;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_EOR) //NOTE(matthias): EOR - Bit-wise XOR
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->AReg ^= dataRead;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_ADC) //NOTE(matthias): ADC - Add with Carry
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        u16 result = cpu->AReg + dataRead;
        result += cpu->Carry ? 1 : 0;
        cpu->Overflow = (~(cpu->AReg ^ dataRead) & (cpu->AReg ^ (u8)result) & 0x80)!=0 ? true : false;
        cpu->Carry = result > 0xFF;
        cpu->AReg = result;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_STA)  //NOTE(matthias): STA - Store Acumulator
    {
        Set_Data(cpu->AReg ,opcodeAddr, cpu, instruction);

        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_LDA) //NOTE(matthias): LDA - Load Accumulator
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->AReg = dataRead;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_CMP) //NOTE(matthias): CMP - Compare Accumulator
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        u8 result = cpu->AReg - dataRead;
        ZERO(result);
        NEGATIVE(result);
        cpu->Carry = cpu->AReg >= dataRead;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_SBC) //NOTE(matthias): SBC - Subtract with Carry
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        u16 result = cpu->AReg - dataRead;
        result -= cpu->Carry ? 0 : 1;
        cpu->Overflow = (!((cpu->AReg^ dataRead) & ((cpu->AReg ^ (u8)result) & 0x80))) ? false : true;
        cpu->Carry = !(result > 0xFF);
        cpu->AReg = result;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_ASL) //NOTE(matthias): ASL - Arithmetic Shift Left
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->Carry = (dataRead & 0x80) != 0;
        dataRead <<= 1;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_ROL) //NOTE(matthias): ROL - Rotate Left
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        bool newCarry = (dataRead & 0x80) != 0;
        dataRead <<= 1;
        dataRead |= (cpu->Carry ? 0x01 : 0x00);
        cpu->Carry = newCarry;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_LSR) //NOTE(matthias): LSR - Logical Shift Right
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->Carry = (dataRead & 0x01) != 0;
        dataRead >>= 1;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_ROR) //NOTE(matthias): ROR - Rotate right
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        bool newCarry = (dataRead & 0x01) != 0;
        dataRead >>= 1;
        dataRead |= (cpu->Carry ? 0x80 : 0x00);
        cpu->Carry = newCarry;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_LDX) //NOTE(matthias): LDX - Load X Register
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->XReg = dataRead;
        ZERO(cpu->XReg);
        NEGATIVE(cpu->XReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_LDY) //NOTE(matthias): LDY - Store in Y register
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        cpu->YReg = dataRead;
        ZERO(cpu->YReg);
        NEGATIVE(cpu->YReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_DEC) //NOTE(matthias): DEC - Decrement
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        dataRead--;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);

        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_INC) //NOTE(matthias): INC - Increment
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        dataRead++;
        ZERO(dataRead);
        NEGATIVE(dataRead);

        Set_Data(dataRead, opcodeAddr, cpu, instruction);

        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_STY) //NOTE(matthias): STY - Store Y register
    {
        Set_Data(cpu->YReg ,opcodeAddr, cpu, instruction);

        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_STX) //NOTE(matthias): STX - Store X register
    {
        Set_Data(cpu->XReg ,opcodeAddr, cpu, instruction);

        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_CPY) //NOTE(matthias): CPY - Compare Y Register
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        u8 result = cpu->YReg - dataRead;
        ZERO(result);
        NEGATIVE(result);
        cpu->Carry = cpu->YReg >= dataRead;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_CPX)  //NOTE(matthias): CPX - Compare X register

    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);

        u8 result = cpu->XReg - dataRead;
        ZERO(result);
        NEGATIVE(result);
        cpu->Carry = cpu->XReg >= dataRead;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BIT) //NOTE(matthias): BIT - Zero page mode - Bit test
    {
        u8 dataRead = Get_Data(opcodeAddr, cpu, instruction);
        cpu->Zero = (dataRead & cpu->AReg) == 0;
        cpu->Overflow = (dataRead&0x40) != 0;
        NEGATIVE(dataRead);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_JMP) //NOTE(matthias): JMP - Jump to Address
    {
        switch(instruction.Mode)
        {
            case Absolute:
            {
                cpu->PCReg = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
            }break;
            case Indirect:
            {
                u16 addrAddr = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
                u16 jmpAddr = read_memory(addrAddr, cpu->console) & 0x00FF;
                u16 upperAddrAddr = addrAddr & 0xFF00;
                addrAddr = (addrAddr+1) & 0x00FF;
                addrAddr |= upperAddrAddr;
                jmpAddr |= read_memory(addrAddr, cpu->console)<<8;
                cpu->PCReg = jmpAddr;
            }break;
            default:
            {
                Log("Invalid jmp");
                cpu->Reset = true;
            }break;
        }
        cpu->cycles += instruction.Cycles;
    }

    OP(_BRK)
    {
        cpu->Reset = true;
        char message[64];
        sprintf(message,"%02X %s: Undefined opcode, halting\n", opcodeAddr[0], opcode_to_mnemonic(opcodeAddr[0]));
        Log(message);
    }

    OP(_PHP) //NOTE(matthias): PHP - Push status flags to stack
    {
        u8 result = status_flags(cpu) | 0x10;
        push_stack(cpu, result);
        cpu->Break = true;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BPL) //NOTE(matthias): BPL - Branch if Positive
    {
        if(!(cpu->Negative))
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_CLC) //NOTE(matthias): CLC - Clear carry
    {
        cpu->Carry = false;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_JSR) //NOTE(matthias): JSR - Jump to subroutine
    {
        u16 programCounter = cpu->PCReg + (instruction.Length - 1);
        u16 jumpAddress = Absolute_Address(opcodeAddr[1], opcodeAddr[2]);
        push_stack(cpu, programCounter>>8);
        push_stack(cpu, programCounter);
        cpu->PCReg = jumpAddress;
        cpu->cycles += 6;
    }

    OP(_PLP) //NOTE(matthias): PLP - pop stack into status register
    {
        u8 statusReg = pop_stack(cpu);
        set_status(statusReg, cpu);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BMI) //NOTE(matthias): BMI - Branch if Negative
    {
        if(cpu->Negative)
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_SEC) //NOTE(matthias): SEC -  Set Carry Flag
    {
        cpu->Carry = true;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_RTI) //NOTE(matthias): RTI - Return from Interrupt
    {
        u8 flags = pop_stack(cpu);
        set_status(flags, cpu);
        u16 returnAddress = pop_stack(cpu);
        returnAddress |= (pop_stack(cpu)<<8);
        cpu->PCReg = returnAddress;
        //cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_PHA) //NOTE(matthias): PHA - push A on the stack
    {
        push_stack(cpu, cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BVC) //NOTE(matthias): BVC - Branch if Overflow Clear
    {
        if(!(cpu->Overflow))
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_CLI) //NOTE(matthias): CLI - Clear Interrupt Disable
    {
	//(TODO)matthias: Implement me
        cpu->Reset = true;
        char message[64];
        sprintf(message,"%02X %s: Undefined opcode, halting\n", opcodeAddr[0], opcode_to_mnemonic(opcodeAddr[0]));
        Log(message);
    }

    OP(_RTS) //NOTE(matthias): RTS - Return from subroutine
    {
        u16 returnAddress = pop_stack(cpu);
        returnAddress |= (pop_stack(cpu) << 8);
        cpu->PCReg = returnAddress;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_PLA) //NOTE(matthias): PLA - pull Accumulator
    {
        cpu->AReg = pop_stack(cpu);
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BVS) //NOTE(matthias): BVS - Branch if Overflow set
    {
        if(cpu->Overflow)
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_SEI) //NOTE(matthias): SEI - Set Interrupt Disable
    {
        cpu->Interrupt = true;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_DEY)  //NOTE(matthias): DEY - Decrement Y register
    {
        cpu->YReg--;
        ZERO(cpu->YReg);
        NEGATIVE(cpu->YReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_TXA) //NOTE(matthias): TXA - Transfer X to Accumulator
    {
        cpu->AReg = cpu->XReg;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->AReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BCC) //NOTE(matthia): BCC - Branck if carry is clear
    {
        if(!(cpu->Carry))
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_TYA) //NOTE(matthias): TYA - Tranfer Y to A
    {
        cpu->AReg = cpu->YReg;
        ZERO(cpu->AReg);
        NEGATIVE(cpu->YReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_TXS) //NOTE(matthias): TXS - Transfer X to Stack Pointer
    {
        cpu->SPReg = cpu->XReg;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_INY) //NOTE(matthias): INY - Increment Y Register
    {
        cpu->YReg++;
        ZERO(cpu->YReg);
        NEGATIVE(cpu->YReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_DEX) //NOTE(matthias): DEX - Decrement X register
    {
        cpu->XReg--;
        ZERO(cpu->XReg);
        NEGATIVE(cpu->XReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BNE) //NOTE(matthias): BNE - If Zero flag is clear branch
    {
        if(!(cpu->Zero))
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_CLD) //NOTE(matthias): CLD - Clear Decimal Mode
    {
        cpu->Decimal = false;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_INX) //NOTE(matthias): INX - Increment X register
    {
        cpu->XReg++;
        ZERO(cpu->XReg);
        NEGATIVE(cpu->XReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_NOP) //NOTE(matthias): NOP - increments PC
    {
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BEQ) //NOTE(matthia): BEQ - Branck if Zero is set
    {
        if(cpu->Zero)
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_SED) //NOTE(matthias): SED - Set Decimal Flag
    {
        cpu->Decimal = true;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_TAY)  //NOTE(matthias): TAY - Transfer A to Y
    {
        cpu->YReg = cpu->AReg;
        ZERO(cpu->YReg);
        NEGATIVE(cpu->YReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_TAX) //NOTE(matthias): TAX - Transfer A to X
    {
        cpu->XReg = cpu->AReg;
        ZERO(cpu->XReg);
        NEGATIVE(cpu->XReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_BCS) //NOTE(matthias): BCS - branch if carry set
    {
        if(cpu->Carry)
        {
            i8 jumpOffset = (i8)opcodeAddr[1];
            cpu->PCReg += jumpOffset;
        }
        cpu->PCReg += instruction.Length;
        //(TODO)matthias: Implement cycle increase if crossing page boundry.
        cpu->cycles += instruction.Cycles;
    }

    OP(_CLV) //NOTE(matthias): CLV - clear Overflow flag
    {
        cpu->Overflow = false;
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    OP(_TSX)  //NOTE(matthias): TSX - Transfer stack pointer to X
    {
        cpu->XReg = cpu->SPReg;
        ZERO(cpu->XReg);
        NEGATIVE(cpu->XReg);
        cpu->PCReg += instruction.Length;
        cpu->cycles += instruction.Cycles;
    }

    const OP_Ptr Ops[56] = {
        &_ORA, &_AND, &_EOR, &_ADC, &_STA, &_LDA, &_CMP, &_SBC,
        &_ASL, &_ROL, &_LSR, &_ROR, &_LDX, &_LDY, &_DEC, &_INC,
        &_STY, &_STX, &_CPY, &_CPX, &_BIT, &_JMP, &_BRK, &_PHP,
        &_BPL, &_CLC, &_JSR, &_PLP, &_BMI, &_SEC, &_RTI, &_PHA,
        &_BVC, &_CLI, &_RTS, &_PLA, &_BVS, &_SEI, &_DEY, &_TXA,
        &_BCC, &_TYA, &_TXS, &_INY, &_DEX, &_BNE, &_CLD, &_INX,
        &_NOP, &_BEQ, &_SED, &_TAY, &_TAX, &_BCS, &_CLV, &_TSX,
    };


    const Instruction Instructions[256] = {
        // NOTE(matthias): 0x0X
        {BRK, Implicit, 1, 7},
        {ORA, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {NOP, Invalid, 2, 2}, // NOTE(matthias): Undocumented opcode 0x04
        {ORA, ZeroPage, 2, 3},
        {ASL, ZeroPage, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {PHP, Implicit, 1, 3},
        {ORA, Immediate, 2, 2},
        {ASL, Accumulator, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {ORA, Absolute, 3, 4},
        {ASL, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},

        // NOTE(matthias): 0x1X
        {BPL, Relative, 2, 2},
        {ORA, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {ORA, ZeroPageX, 2, 4},
        {ASL, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {CLC, Implicit, 1, 2},
        {ORA, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {ORA, AbsoluteX, 3, 4},
        {ASL, AbsoluteX, 3, 7},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x2X
        {JSR, Absolute, 3, 6},
        {AND, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {BIT, ZeroPage, 2, 3},
        {AND, ZeroPage, 2, 3},
        {ROL, ZeroPage, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {PLP, Implicit, 1, 4},
        {AND, Immediate, 2, 2},
        {ROL, Accumulator, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {BIT, Absolute, 3, 4},
        {AND, Absolute, 3, 4},
        {ROL, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x3X
        {BMI, Relative, 2, 2},
        {AND, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {AND, ZeroPageX, 2, 4},
        {ROL, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {SEC, Implicit, 1, 2},
        {AND, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {AND, AbsoluteX, 3, 4},
        {ROL, AbsoluteX, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x4X
        {RTI, Implicit, 1, 6},
        {EOR, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {NOP, Invalid, 2, 2},// NOTE(matthias): Undocumented opcode 0x44
        {EOR, ZeroPage, 2, 3},
        {LSR, ZeroPage, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {PHA, Implicit, 1, 3},
        {EOR, Immediate, 2, 2},
        {LSR, Accumulator, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {JMP, Absolute, 3, 3},
        {EOR, Absolute, 3, 4},
        {LSR, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x5X
        {BVC, Relative, 2, 2},
        {EOR, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {EOR, ZeroPageX, 2, 4},
        {LSR, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {CLI, Implicit, 1, 2},
        {EOR, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {EOR, AbsoluteX, 3, 4},
        {LSR, AbsoluteX, 3, 7},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x6X
        {RTS, Implicit, 1, 6},
        {ADC, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {NOP, Invalid, 2, 2},// NOTE(matthias): Undocumented opcode 0x44
        {ADC, ZeroPage, 2, 3},
        {ROR, ZeroPage, 2, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {PLA, Implicit, 1, 4},
        {ADC, Immediate, 2, 2},
        {ROR, Accumulator, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {JMP, Indirect, 3, 5},
        {ADC, Absolute, 3, 4},
        {ROR, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x7X
        {BVS, Relative, 2, 2},
        {ADC, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {ADC, ZeroPageX, 2},
        {ROR, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {SEI, Implicit, 1, 2},
        {ADC, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {ADC, AbsoluteX, 3, 4},
        {ROR, AbsoluteX, 3, 7},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x8X
        {Mnemonic_Count, Invalid, 0, 0},
        {STA, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {STY, ZeroPage, 2, 3},
        {STA, ZeroPage, 2, 3},
        {STX, ZeroPage, 2, 3},
        {Mnemonic_Count, Invalid, 0, 0},
        {DEY, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {TXA, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {STY, Absolute, 3, 4},
        {STA, Absolute, 3, 4},
        {STX, Absolute, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0x9X
        {BCC, Relative, 2, 2},
        {STA, IndirectIndexed, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {STY, ZeroPageX, 2, 4},
        {STA, ZeroPageX, 2, 4},
        {STX, ZeroPageY, 2, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {TYA, Implicit, 1, 2},
        {STA, AbsoluteY, 3, 5},
        {TXS, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {STA, AbsoluteX, 3, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xAX
        {LDY, Immediate, 2, 2},
        {LDA, IndexedIndirect, 2, 6},
        {LDX, Immediate, 2, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {LDY, ZeroPage, 2, 3},
        {LDA, ZeroPage, 2, 3},
        {LDX, ZeroPage, 2, 3},
        {Mnemonic_Count, Invalid, 0, 0},
        {TAY, Implicit, 1, 2},
        {LDA, Immediate, 2, 2},
        {TAX, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {LDY, Absolute, 3, 4},
        {LDA, Absolute, 3, 4},
        {LDX, Absolute, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xBX
        {BCS, Relative, 2, 2},
        {LDA, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {LDY, ZeroPageX, 2, 4},
        {LDA, ZeroPageX, 2, 4},
        {LDX, ZeroPageY, 2, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {CLV, Implicit, 1, 2},
        {LDA, AbsoluteY, 3, 4},
        {TSX, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {LDY, AbsoluteX, 3, 4},
        {LDA, AbsoluteX, 3, 4},
        {LDX, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xCX
        {CPY, Immediate, 2, 2},
        {CMP, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {CPY, ZeroPage, 2, 3},
        {CMP, ZeroPage, 2, 3},
        {DEC, ZeroPage, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {INY, Implicit, 1, 2},
        {CMP, Immediate, 2, 2},
        {DEX, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {CPY, Absolute, 3, 4},
        {CMP, Absolute, 3, 4},
        {DEC, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xDX
        {BNE, Relative, 2, 2},
        {CMP, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {CMP, ZeroPageX, 2, 4},
        {DEC, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {CLD, Implicit, 1, 2},
        {CMP, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {CMP, AbsoluteX, 3, 4},
        {DEC, AbsoluteX, 3, 7},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xEX
        {CPX, Immediate, 2, 2},
        {SBC, IndexedIndirect, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {CPX, ZeroPage, 2, 3},
        {SBC, ZeroPage, 2, 3},
        {INC, ZeroPage, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {INX, Implicit, 1, 2},
        {SBC, Immediate, 2, 2},
        {NOP, Implicit, 1, 2},
        {Mnemonic_Count, Invalid, 0, 0},
        {CPX, Absolute, 3, 4},
        {SBC, Absolute, 3, 4},
        {INC, Absolute, 3, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        // NOTE(matthias): 0xFX
        {BEQ, Relative, 2, 2},
        {SBC, IndirectIndexed, 2, 5},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {SBC, ZeroPageX, 2, 4},
        {INC, ZeroPageX, 2, 6},
        {Mnemonic_Count, Invalid, 0, 0},
        {SED, Implicit, 1, 2},
        {SBC, AbsoluteY, 3, 4},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {Mnemonic_Count, Invalid, 0, 0},
        {SBC, AbsoluteX, 3, 4},
        {INC, AbsoluteX, 3, 7},
        {Mnemonic_Count, Invalid, 0, 0},

    };
