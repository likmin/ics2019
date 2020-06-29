#include "cpu/exec.h"

CPU_state cpu;

rtlreg_t s0, s1, t0, t1, ir;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp) {
  decinfo.is_jmp = is_jmp;
}

void isa_exec(vaddr_t *pc);

vaddr_t exec_once(void) {
  decinfo.seq_pc = cpu.pc;
  //printf("[exec_once] seq_pc = 0x%x \n", decinfo.seq_pc);
  isa_exec(&decinfo.seq_pc);
  update_pc();
  //printf("\n");
  return decinfo.seq_pc;
}
