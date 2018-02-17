#include "bit_nes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "b_6502.h"
#include "b_ppu.h"

namespace BITNES {

nes* init_nes(u8* rom, MapperType mapper) {
  nes* NES = (nes*)calloc(1, sizeof(nes));
  NES->memory = init_memory(rom, mapper);

  NES->mapper = mapper;
  NES->cpu = init_cpu(NES->memory);
  NES->ppu = init_ppu(NES->memory);

  return NES;
}

u8* init_memory(u8* rom, MapperType mapper) {
  u8* memory = (u8*)calloc(sizeof(char), sizeof(char) * MEM_SIZE);

  switch(mapper) {
  case M000_16K: // Super Mario Bros, nestest
    memcpy(memory + ROM_START, rom, KILOBYTE(16)*sizeof(char));
    printf("test\n");
    memcpy(memory + 0xc000, rom, KILOBYTE(16)*sizeof(char));
    break;

  case M000_32K:
    memcpy(memory + ROM_START, rom, KILOBYTE(32));
    break;

  default:
    break;
  }

  return memory;
}

u8 read_memory(u16 address, nes* nes) {
  //TODO(matthias): check if address is valid

  switch(nes->mapper) {
  case M000_16K:
    if(address >= 0xc000) //NOTE(matthias): memory is mirrored here
      return nes->memory[address - KILOBYTE(16)];
    else
      return nes->memory[address];
    break;

  case M000_32K:
    return nes->memory[address];
    break;

  default:
    break;
  }
  return 0;
}

void write_memory(u16 address, u8 value, nes* nes) {
  //TODO(matthias): check if address is valid

  switch(nes->mapper) {
  case M000_16K:
    if(address >= 0xc000) { //NOTE(matthias): memory is mirrored here
      nes->memory[address - KILOBYTE(16)] = value;
      nes->memory[address] = value;
    } else if(address >= ROM_START) {
      nes->memory[address] = value;
      nes->memory[address + KILOBYTE(16)] = value;
    } else {
      nes->memory[address] = value;
    }
    break;

  case M000_32K:
    nes->memory[address] = value;
    break;

  default:
    break;
  }
}

}
