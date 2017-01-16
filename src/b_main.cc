#include <stdio.h>
#include <stdlib.h>
#include "b_6502.h"

int main(int argc, char** argv) {
  FILE* romFile = fopen(argv[1], "r");
  if(romFile == NULL) {
    printf("Could not load file %s\n", argv[1]);
    return -1;
  }
  
  int count = 0;
  while(fgetc(romFile) != EOF)
    count++;

  u8 *rom_buffer = (u8*)malloc(sizeof(u8)*count);
  fseek(romFile, 0, SEEK_SET);

  int temp = fgetc(romFile);
  int index = 0;

  while(temp != EOF) {
    rom_buffer[index] = temp;
    index++;
    temp = fgetc(romFile);
  }

  for(int i=0; i<count; i++) {
    printf("%02X ", rom_buffer[i]&0x00ff);
  }
  printf("\n");
  
  return 0;
  
}
