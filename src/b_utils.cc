#include "bit_nes.h"
#include "b_utils.h"

uint translate_address(u16 address) {

  u8 high = address >> 8;
  u8 low = address & 0x00ff;
  int addressIndex = high * 0xff;
  addressIndex += low;

  return addressIndex;

}
