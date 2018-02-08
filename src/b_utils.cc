#include "bit_nes.h"
#include "b_utils.h"

namespace BITNES
{

const char* opcode_to_mnemonic(u8 opcode) {
  char* mnemonic = 0;

  switch(opcode) {
  case 0x01: case 0x05: case 0x09: case 0x0D: case 0x11: case 0x15: case 0x19: case 0x1D:
    mnemonic = "ORA";
    break;
  case 0x21: case 0x25: case 0x29: case 0x2D: case 0x31: case 0x35: case 0x39: case 0x3D:
    mnemonic = "AND";
    break;
  case 0x41: case 0x45: case 0x49: case 0x4D: case 0x51: case 0x55: case 0x59: case 0x5D:
    mnemonic = "EOR";
    break;
  case 0x61: case 0x65: case 0x69: case 0x6D: case 0x71: case 0x75: case 0x79: case 0x7D:
    mnemonic = "ADC";
    break;
  case 0x81: case 0x85: case 0x8D: case 0x91: case 0x95: case 0x99: case 0x9D:
    mnemonic = "STA";
    break;
  case 0xA1: case 0xA5: case 0xA9: case 0xAD: case 0xB1: case 0xB5: case 0xB9: case 0xBD:
    mnemonic = "LDA";
    break;
  case 0xC1: case 0xC5: case 0xC9: case 0xCD: case 0xD1: case 0xD5: case 0xD9: case 0xDD:
    mnemonic = "CMP";
    break;
  case 0xE1: case 0xE5: case 0xE9: case 0xED: case 0xF1: case 0xF5: case 0xF9: case 0xFD:
    mnemonic = "SBC";
    break;
  case 0x06: case 0x0A: case 0x0E: case 0x16: case 0x1E:
    mnemonic = "ASL";
    break;
  case 0x26: case 0x2A: case 0x2E: case 0x36: case 0x3E:
    mnemonic = "ROL";
    break;
  case 0x46: case 0x4A: case 0x4E: case 0x56: case 0x5E:
    mnemonic = "LSR";
    break;
  case 0x66: case 0x6A: case 0x6E: case 0x76: case 0x7E:
    mnemonic = "ROR";
    break;
  case 0xA2: case 0xA6: case 0xAE: case 0xB6: case 0xBE:
    mnemonic = "LDX";
    break;
  case 0xA0: case 0xA4: case 0xAC: case 0xB4: case 0xBC:
    mnemonic = "LDY";
    break;
  case 0xC6: case 0xCE: case 0xD6: case 0xDE:
    mnemonic = "DEC";
    break;
  case 0xE6: case 0xEE: case 0xF6: case 0xFE:
    mnemonic = "INC";
    break;
  case 0x84: case 0x8C: case 0x94:
    mnemonic = "STY";
    break;
  case 0x86: case 0x8E: case 0x96:
    mnemonic = "STX";
    break;
  case 0xC0: case 0xC4: case 0xCC:
    mnemonic = "CPY";
    break;
  case 0xE0: case 0xE4: case 0xEC:
    mnemonic = "CPX";
    break;
  case 0x24: case 0x2C:
    mnemonic = "BIT";
    break;
  case 0x4C: case 0x6C:
    mnemonic = "JMP";
    break;
  case 0x00:
    mnemonic = "BRK";
    break;
  case 0x08:
    mnemonic = "PHP";
    break;
  case 0x10:
    mnemonic = "BPL";
    break;
  case 0x18:
    mnemonic = "CLC";
    break;
  case 0x20:
    mnemonic = "JSR";
    break;
  case 0x28:
    mnemonic = "PLP";
    break;
  case 0x30:
    mnemonic = "BMI";
    break;
  case 0x38:
    mnemonic = "SEC";
    break;
  case 0x40:
    mnemonic = "RTI";
    break;
  case 0x48:
    mnemonic = "PHA";
    break;
  case 0x50:
    mnemonic = "BVC";
    break;
  case 0x58:
    mnemonic = "CLI";
    break;
  case 0x60:
    mnemonic = "RTS";
    break;
  case 0x68:
    mnemonic = "PLA";
    break;
  case 0x70:
    mnemonic = "BVS";
    break;
  case 0x78:
    mnemonic = "SEI";
    break;
  case 0x88:
    mnemonic = "DEY";
    break;
  case 0x8A:
    mnemonic = "TXA";
    break;
  case 0x90:
    mnemonic = "BCC";
    break;
  case 0x98:
    mnemonic = "TYA";
    break;
  case 0x9A:
    mnemonic = "TXS";
    break;
  case 0xA8:
    mnemonic = "TAY";
    break;
  case 0xAA:
    mnemonic = "TAX";
    break;
  case 0xB0:
    mnemonic = "BCS";
    break;
  case 0xB8:
    mnemonic = "CLV";
    break;
  case 0xBA:
    mnemonic = "TSX";
    break;
  case 0xC8:
    mnemonic = "INY";
    break;
  case 0xCA:
    mnemonic = "DEX";
    break;
  case 0xD0:
    mnemonic = "BNE";
    break;
  case 0xD8:
    mnemonic = "CLD";
    break;
  case 0xE8:
    mnemonic = "INX";
    break;
  case 0xEA:
    mnemonic = "NOP";
    break;
  case 0xF0:
    mnemonic = "BEQ";
    break;
  case 0xF8:
    mnemonic = "SED";
    break;
  default:
    mnemonic = "Invalid operation!";
    break;
  }

  return mnemonic;
}

}
