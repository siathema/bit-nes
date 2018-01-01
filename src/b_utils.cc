#include "bit_nes.h"
#include "b_utils.h"

namespace BITNES
{

u32 translate_address(u16 address) {

  u8 high = address >> 8;
  u8 low = address & 0x00ff;
  i32 addressIndex = high * 0xff;
  addressIndex += low;

  return addressIndex;
}

}
