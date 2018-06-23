#ifndef _UTILS_H_
#define _UTILS_H_

#include "bit_nes.h"

#define KILOBYTE(size) (i64)(size * 1024)

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
        IndirectIndexed, Address_Mode_Count
    };

    struct Instruction
    {
        Mnemonic Name;
        Address_Mode Mode;
        u8 Length;
    };

    const Instruction Instructions[] = {
        // NOTE(matthias): 0x0X
        {BRK, Implicit, 1},
        {ORA, IndexedIndirect, 2},
        {},
        {},
        {},
        {ORA, ZeroPage, 2},
        {ASL, ZeroPage, 2},
        {},
        {PHP, Implicit, 2},
        {ORA, Immediate, 2},
        {ASL, Accumulator, 1},
        {},
        {},
        {ORA, Absolute, 3},
        {ASL, Absolute, 3},
        {},

        // NOTE(matthias): 0x1X
        {BPL, Relative, 2},
        {ORA, IndirectIndexed, 2},
        {},
        {},
        {},
        {ORA, ZeroPageX, 2},
        {ASL, ZeroPageX, 2},
        {},
        {CLC, Implicit, 1},
        {ORA, AbsoluteY, 3},
        {},
        {},
        {},
        {ORA, AbsoluteX, 3},
        {ASL, AbsoluteX, 3},
        {},
        // NOTE(matthias): 0x2X
        {JSR, Absolute, 3},
        {AND, IndexedIndirect, 2},
        {},
        {},
        {BIT, ZeroPage, 2},
        {AND, ZeroPage, 2},
        {ROL, ZeroPage, 2},
        {},
        {PLP, Implicit, 1},
        {AND, Immediate, 2},
        {ROL, Accumulator, 1},
        {},
        {BIT, Absolute, 3},
        {AND, Absolute, 3},
        {ROL, Absolute, 3},
        {},
        // NOTE(matthias): 0x3X
        {BMI, Relative, 2},
        {AND, IndirectIndexed, 2},
        {},
        {},
        {},
        {AND, ZeroPageX, 2},
        {ROL, ZeroPageX, 2},
        {},
        {SEC, Implicit, 1},
        {AND, AbsoluteY, 3},
        {},
        {},
        {},
        {AND, AbsoluteX, 3},
        {ROL, AbsoluteX},
        {},
        // NOTE(matthias): 0x4X
        {RTI, Implicit, 1},
        {EOR, IndexedIndirect, 2},
        {},
        {},
        {},
        {EOR, ZeroPage, 2},
        {LSR, ZeroPage, 2},
        {},
        {PHA, Implicit, 2},
        {EOR, Immediate, 2},
        {LSR, Accumulator, 1},
        {},
        {JMP, Absolute, 3},
        {EOR, Absolute, 3},
        {LSR, Absolute, 3},
        {},
        // NOTE(matthias): 0x5X
        {BVC, Relative, 2},
        {EOR, IndirectIndexed, 2},
        {},
        {},
        {},
        {EOR, ZeroPageX, 2},
        {LSR, ZeroPageX, 2},
        {},
        {CLI, Implicit, 1},
        {EOR, AbsoluteY, 3},
        {},
        {},
        {},
        {EOR, AbsoluteX, 3},
        {LSR, AbsoluteX, 3},
        {},
        // NOTE(matthias): 0x6X
        {RTS, Implicit, 1},
        {ADC, IndexedIndirect, 2},
        {},
        {},
        {},
        {ADC, ZeroPage, 2},
        {ROR, ZeroPage, 2},
        {},
        {PLA, Implicit, 1},
        {ADC, Immediate, 2},
        {ROR, Accumulator, 1},
        {},
        {JMP, Indirect, 3},
        {ADC, Absolute, 3},
        {ROR, Absolute, 3},
        {},
        // NOTE(matthias): 0x7X
        {BVS, Relative, 2},
        {ADC, IndirectIndexed, 2},
        {},
        {},
        {},
        {ADC, ZeroPageX, 2},
        {ROR, ZeroPageX, 2},
        {},
        {SEI, Implicit, 1},
        {ADC, AbsoluteY, 3},
        {},
        {},
        {},
        {ADC, AbsoluteX, 3},
        {ROR, AbsoluteX, 3},
        {},
        // NOTE(matthias): 0x8X
        {},
        {STA, IndexedIndirect, 2},
        {},
        {},
        {STY, ZeroPage, 2},
        {STA, ZeroPage, 2},
        {STX, ZeroPage, 2},
        {},
        {DEY, Implicit, 1},
        {},
        {TXA, Implicit, 1},
        {},
        {STY, Absolute, 3},
        {STA, Absolute, 3},
        {STX, Absolute, 3},
        {},
        // NOTE(matthias): 0x9X
        {BCC, Relative, 2},
        {STA, IndirectIndexed, 2},
        {},
        {},
        {STY, ZeroPageX, 2},
        {STA, ZeroPageX, 2},
        {STX, ZeroPageY, 2},
        {},
        {TYA, Implicit, 1},
        {STA, AbsoluteY, 3},
        {TXS, Implicit, 1},
        {},
        {},
        {STA, AbsoluteX, 3},
        {},
        {},
        // NOTE(matthias): 0xAX
        {LDY, Immediate, 2},
        {LDA, IndexedIndirect, 2},
        {LDX, Immediate, 2},
        {},
        {LDY, ZeroPage, 2},
        {LDA, ZeroPage, 2},
        {LDX, ZeroPage, 2},
        {},
        {TAY, Implicit, 1},
        {LDA, Immediate, 2},
        {TAX, Implicit, 1},
        {},
        {LDY, Absolute, 3},
        {LDA, Absolute, 3},
        {LDX, Absolute, 3},
        {},
        // NOTE(matthias): 0xBX
        {BCS, Relative, 2},
        {LDA, IndirectIndexed, 2},
        {},
        {},
        {LDY, ZeroPageX, 2},
        {LDA, ZeroPageX, 2},
        {LDX, ZeroPageY, 2},
        {},
        {CLV, Implicit, 1},
        {LDA, AbsoluteY, 3},
        {TSX, Implicit, 1},
        {},
        {LDY, AbsoluteX, 3},
        {LDA, AbsoluteX, 3},
        {LDX, AbsoluteY, 3},
        {},
        // NOTE(matthias): 0xCX
        {CPY, Immediate, 2},
        {SBC, IndexedIndirect, 2},
        {},
        {},
        {CPY, ZeroPage, 2},
        {CMP, ZeroPage, 2},
        {DEC, ZeroPage, 2},
        {},
        {INY, Implicit, 1},
        {CMP, Immediate, 2},
        {DEX, Implicit, 1},
        {},
        {CPY, Absolute, 3},
        {CMP, Absolute, 3},
        {DEC, Absolute, 3},
        {},
        // NOTE(matthias): 0xDX
        {BNE, Relative, 2},
        {CMP, IndirectIndexed, 2},
        {},
        {},
        {},
        {CMP, ZeroPageX, 2},
        {DEC, ZeroPageX, 2},
        {},
        {CLD, Implicit, 1},
        {CMP, AbsoluteY, 3},
        {},
        {},
        {},
        {CMP, AbsoluteX, 3},
        {DEC, AbsoluteX, 3},
        {},
        // NOTE(matthias): 0xEX
        {CPX, Immediate, 2},
        {SBC, IndexedIndirect, 2},
        {},
        {},
        {CPX, ZeroPage, 2},
        {SBC, ZeroPage, 2},
        {INC, ZeroPage, 2},
        {},
        {INX, Implicit, 1},
        {SBC, Immediate, 2},
        {NOP, Implicit, 1},
        {},
        {CPX, Absolute, 3},
        {SBC, Absolute, 3},
        {INC, Absolute, 3},
        {},
        // NOTE(matthias): 0xFX
        {BEQ, Relative, 2},
        {SBC, IndirectIndexed, 2},
        {},
        {},
        {},
        {SBC, ZeroPageX, 2},
        {INC, ZeroPageX, 2},
        {},
        {SED, Implicit, 1},
        {SBC, AbsoluteY, 3},
        {},
        {},
        {},
        {SBC, AbsoluteX, 3},
        {INC, AbsoluteX, 3},
        {},

    };

    const char Mnemonics[56][4] = {
        {"ORA"}, {"AND"}, {"EOR"}, {"ADC"}, {"STA"}, {"LDA"}, {"CMP"}, {"SBC"},
        {"ASL"}, {"ROL"}, {"LSR"}, {"ROR"}, {"LDX"}, {"LDY"}, {"DEC"}, {"INC"},
        {"STY"}, {"STX"}, {"CPY"}, {"CPX"}, {"BIT"}, {"JMP"}, {"BRK"}, {"PHP"},
        {"BPL"}, {"CLC"}, {"JSR"}, {"PLP"}, {"BMI"}, {"SEC"}, {"RTI"}, {"PHA"},
        {"BVC"}, {"CLI"}, {"RTS"}, {"PLA"}, {"BVS"}, {"SEI"}, {"DEY"}, {"TXA"},
        {"BCC"}, {"TYA"}, {"TSX"}, {"INY"}, {"DEX"}, {"BNE"}, {"CLD"}, {"INX"},
        {"NOP"}, {"BEQ"}, {"SED"}, {"TAY"}, {"TAX"}, {"BCS"}, {"CLV"}, {"TSX"}
    };

    void Instruction_Debug_Messege(nes* nes, u8* instruction);
    const char* opcode_to_mnemonic(u8 opcode);
}
#endif
