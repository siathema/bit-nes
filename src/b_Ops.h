#ifndef _B_OPS_H
#define _B_OPS_H

#include "b_6502.h"

namespace BITNES
{
    enum Mnemonic
    {
        ORA, AND, EOR, ADC, STA, LDA, CMP, SBC,
        ASL, ROL, LSR, ROR, LDX, LDY, DEC, INC,
        STY, STX, CPY, CPX, BIT, JMP, BRK, PHP,
        BPL, CLC, JSR, PLP, BMI, SEC, RTI, PHA,
        BVC, CLI, RTS, PLA, BVS, SEI, DEY, TXA,
        BCC, TYA, TXS, INY, DEX, BNE, CLD, INX,
        NOP, BEQ, SED, TAY, TAX, BCS, CLV, TSX, Mnemonic_Count
    };

    enum Address_Mode
    {
        Implicit, Accumulator, Immediate, ZeroPage,
        ZeroPageX, ZeroPageY, Relative, Absolute,
        AbsoluteX, AbsoluteY, Indirect, IndexedIndirect,
        IndirectIndexed, Invalid, Address_Mode_Count
    };

    struct Instruction
    {
        Mnemonic Name;
        Address_Mode Mode;
        u8 Length;
        u8 Cycles;
    };

    extern const Instruction Instructions[256];

#define OP(name) void name(u8* opcodeAddr,  b6502* cpu, Instruction instruction)

    OP(_ORA); OP(_AND); OP(_EOR); OP(_ADC); OP(_STA); OP(_LDA); OP(_CMP); OP(_SBC);
    OP(_ASL); OP(_ROL); OP(_LSR); OP(_ROR); OP(_LDX); OP(_LDY); OP(_DEC); OP(_INC);
    OP(_STY); OP(_STX); OP(_CPY); OP(_CPX); OP(_BIT); OP(_JMP); OP(_BRK); OP(_PHP);
    OP(_BPL); OP(_CLC); OP(_JSR); OP(_PLP); OP(_BMI); OP(_SEC); OP(_RTI); OP(_PHA);
    OP(_BVC); OP(_CLI); OP(_RTS); OP(_PLA); OP(_BVS); OP(_SEI); OP(_DEY); OP(_TXA);
    OP(_BCC); OP(_TYA); OP(_TXS); OP(_INY); OP(_DEX); OP(_BNE); OP(_CLD); OP(_INX);
    OP(_NOP); OP(_BEQ); OP(_SED); OP(_TAY); OP(_TAX); OP(_BCS); OP(_CLV); OP(_TSX);

    typedef void (*OP_Ptr)(u8* opcodeAddr, b6502* cpu, Instruction instruction);

    extern const OP_Ptr Ops[56];
}
#endif
