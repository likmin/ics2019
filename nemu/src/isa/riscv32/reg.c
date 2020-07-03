#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

/*
char *reg_sl(uint32_t i) {
	return regsl[i];
}
*/
void isa_reg_display() {
  int i;
  for (i = 0; i < 32; i++) {
    printf("%s = 0x%08X  ", regsl[i], reg_l(i));
    if ((i + 1) % 8 != 0) printf(", "); else printf("\n");
  }

  printf("pc = %#X\n", cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  
  if(strcmp("pc",s) == 0) return cpu.pc;
  
  if(s[0] == '0') 
	return reg_l(0);

  int i;
  for (i = 1; i < 32; i++)
	  if(strcmp(regsl[i], s) == 0) break;	

  if(i < 32) return reg_l(i);

  *success = false;
  return 0;
}
