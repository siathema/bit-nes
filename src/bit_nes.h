#ifndef BIT_NES_H_
#define BIT_NES_H_

#include <stdint.h>
#include "b_6502.h"
#inlcude "b_ppu.h"

#define PAGE_SIZE 256

namespace BITNES
{

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

 struct nes {
   b6502* cpu;
   bppu* ppu;
   MapperType mapper;
 };

 nes* init_nes(u8* rom, u32 PRGROMIndex, MapperType mapper);

}
#endif
