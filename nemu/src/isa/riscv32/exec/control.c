#include "cpu/exec.h"

make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
  // rtl_add(&t0, &cpu.pc, 4);
   printf("[interpret_rtl_jal] pc = 0x%x\n", *pc);
   rtl_add(&decinfo.jmp_pc, &id_src->val, pc);
   //printf("jmp_pc = 0x%x\n", decinfo.jmp_pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template1(jal);
}