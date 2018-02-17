#ifndef BIT_NES_H_
#define BIT_NES_H_

#include <stdint.h>

#define PAGE_SIZE 256
#define ROM_START 0x8000
#define MEM_SIZE 0xffff

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

 struct b6502;
 struct bppu;

enum MapperType {
  M000_16K,
  M000_32K,
  COUNT
 };

struct nes {
  b6502* cpu;
  bppu* ppu;
  u8* memory;
  MapperType mapper;
 };

 nes* init_nes(u8* rom,  MapperType mapper);
 u8* init_memory(u8* rom, MapperType mapper);
 u8 read_memory(u16 address, nes* nes);
 void write_memory(u16 address, u8 value, nes* nes);

}
#endif
