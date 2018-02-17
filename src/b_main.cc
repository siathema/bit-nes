#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit_nes.h"
#include "b_6502.h"
#include "b_ppu.h"
#include "b_log.h"
#include "b_utils.h"

namespace BITNES
{
  int proc(const char* filePath) {
    FILE* romFile = fopen(filePath, "r");
    if(romFile == NULL) {
      printf("Could not load file %s\n", filePath);
      return -1;
    }

    int count = 0;
    while(fgetc(romFile) != EOF)
      count++;

    u8 *romBuffer = (u8*)malloc(sizeof(u8)*count);
    fseek(romFile, 0, SEEK_SET);

    int temp = fgetc(romFile);
    int index = 0;

    while(temp != EOF) {
      romBuffer[index] = temp;
      index++;
      temp = fgetc(romFile);
    }
    Log("Read in rom file.\n");
    /*
      for(int i=0; i<count; i++) {
      printf("%02X ", rom_buffer[i]&0x00ff);
      }
      printf("\n");
    */
    //NOTE(matthias): emulation begins here
    //NOTE(matthias): Mapper 000: 16KB or 128KB PRG rom non-switchable banks. If 16KB rom mirror 0x8000-bfff at 0xc000 - 0xffff.
    u8 k16Pages = romBuffer[4];

    printf("%d 16k pages\n",k16Pages );

    int PRGROMIndex = 16;

    int mapperNum = (romBuffer[7] & 0xf0);
    mapperNum |= (romBuffer[6] & 0xf0) >> 4;

    printf("Mapper:%d\n", mapperNum);

    if((romBuffer[6] & 0x04) != 0) {
      printf("Trainer");
      PRGROMIndex += 512;
    }

    nes* nes = init_nes(romBuffer+PRGROMIndex, M000_16K);
    run_cpu(nes->cpu, nes->ppu, nes); //TODO(matthias):Get rid of redundancy

    free(romBuffer);
    free(nes->memory);
    free(nes->cpu);
    free(nes->ppu);
    free(nes);
    LogERROR("killed Emulation!\n");
    return 0;
  }
}

int main(int argc, char** argv) {

  if(argc < 2 || argc > 2) {
    printf("Usage: bit-nes [Path to rom]\n");
    return -1;
  }
  return BITNES::proc(argv[1]);
}
