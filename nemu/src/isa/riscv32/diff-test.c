#include "nemu.h"
#include "monitor/diff-test.h"


const char *reg_sl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void difftest_regs_display(CPU_state *ref_r, vaddr_t pc) {
  printf("REF REGs\n");
  int i;

  for (i = 0; i < 32; i++) {
    printf("%s = 0x%08X  ", reg_sl[i], ref_r->gpr[i]._32);
    if ((i + 1) % 8 != 0) printf(", "); else printf("\n");
  }

  printf("pc = 0x%08X\n", cpu.pc);
  printf("\nriscv32-NEMU:\n");

}


bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  
  int i;
  for (i = 0; i < 32; i++) {
    if (ref_r->gpr[i]._32 != reg_l(i)) {
      difftest_regs_display(ref_r, pc);
      return false;
    }
  }
  //if (ref_r->pc != pc) return false;

  return true;
}



void isa_difftest_attach(void) {
}
