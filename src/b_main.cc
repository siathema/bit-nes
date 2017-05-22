
#include <stdio.h>
#include <stdlib.h>
#include "b_6502.h"

int main(int argc, char** argv) {

  if(argc < 2 || argc > 2) {
    printf("Usage: bit-nes [Path to rom]\n");
    return -1;
  }

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

  free(rom_buffer);
  
  return 0;
  
}

b6502* init_cpu() {
  
}

void run_cpu(b6502 *cpu) {
  bool reset = false;

  while(!reset) {
    run_opcode(&cpu->memory[cpu->PCReg], cpu);
  }
}

void run_opcode(u8* opcodeAddress, b6502 *cpu) {
  u8 opcode = *opcodeAddress;

  // decode opcode
  switch(opcode){
  case 0xa9: // LDA - Immediate mode
    cpu->AReg = opcodeAddress[1];
    cpu->StatusReg |= cpu->AReg == 0 ? 0x02 : 0x00; // Setting zero flag
    cpu->StatusReg |= (cpu->AReg & 0x80) != 0 ? 0x80 : 0x00; // Setting negative flag
    cpu->PCReg += 2;
    cpu->cycles += 2;
    break;
  default:
    printf("Undefined opcode, halting\n");
    break;
  }
}
