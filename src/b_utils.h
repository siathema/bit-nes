#ifndef _UTILS_H_
#define _UTILS_H_

namespace BITNES
{

u32 translate_address(u16 address);
const char* opcode_to_mnemonic(u8 opcode);
}
#endif
