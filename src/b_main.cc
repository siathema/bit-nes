#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit_nes.h"
#include "b_6502.h"
#include "b_ppu.h"
#include "b_log.h"

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

    b6502* cpu = init_cpu(romBuffer, PRGROMIndex);
    bppu* ppu = init_ppu(cpu->memory);

    run_cpu(cpu, ppu);

    free(romBuffer);
    free(cpu->memory);
    free(cpu);
    free(ppu);
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
