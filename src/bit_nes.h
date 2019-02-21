#ifndef BIT_NES_H_
#define BIT_NES_H_

#include <stdint.h>
#include <assert.h>

#define PAGE_SIZE 256
#define ROM_START 0x8000
#define MEM_SIZE 0xffff


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float r32;
typedef double r64;


namespace BITNES
{

 struct b6502;
 struct bppu;

#define Internal static

#define MEM_MAP_INTERNAL_RAM 0x0000
#define MEM_MAP_RAM_MIRROR_0 0x0800
#define MEM_MAP_RAM_MIRROR_1 0x1000
#define MEM_MAP_RAM_MIRROR_2 0x1800

#define MEM_MAP_PPU_REGS	 0x2000
#define MEM_MAP_PPU_MIRROR	 0x2008

#define MEM_MAP_APU_IO_REGS	 0x4000

#define MEM_MAP_CARTRIDGE	 0x4020

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
 bool run_nes(nes* nes);
}
#endif
