#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  
  int i;
  for (i = 0; i < 32; i++) {
    if (ref_r->gpr[i]._32 != reg_l(i)) return false;
  }
  if (ref_r->pc != pc) return false;

  return true;
}

void isa_difftest_attach(void) {
}
