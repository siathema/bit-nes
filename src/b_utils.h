#ifndef _UTILS_H_
#define _UTILS_H_

#include "bit_nes.h"
#include "b_Ops.h"

#define KILOBYTE(size) (i64)(size * 1024)

namespace BITNES
{
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
