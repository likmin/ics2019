#include "nemu.h"
#include "monitor/diff-test.h"


void difftest_regs_display(CPU_state *ref_r, vaddr_t pc) {
  int i;
  for (i = 0; i < 4; i++) printf("Reg.NO \t REF.REG \t REAL.REG \t\t");
  printf("\n");
  for (i = 0; i < 32; i++) {
    printf("%d \t %#X \t %#X \t\t", i, ref_r->gpr[i]._32, reg_l(i));
    if ((i+1) % 4 == 0) printf("\n"); 
  }
  printf("pc = %#X\n", pc);
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
