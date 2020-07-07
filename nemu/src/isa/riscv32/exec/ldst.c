#include "cpu/exec.h"

make_EHelper(ld) { /* void exec_ld(vaddr_t *pc) */
  /*
   * rtl_lm -> interpret_rtl_lm
   * s0 is a reg in 'nemu/src/cpu.c'
   */
  rtl_lm(&s0, &id_src->addr, decinfo.width);

  /*
   * rtl_sr -> rtl_mv
   * if id_dest->reg is not zero, then write the 's0' to destination reg
   */
  rtl_sr(id_dest->reg, &s0, 4);
  
  switch (decinfo.width) {
    case 4: print_asm_template2(lw); break;
    case 2: print_asm_template2(lhu); break;
    case 1:  print_asm_template2(lbu); break;
    default: assert(0);
  }
}

make_EHelper(lh_lb) {

  rtl_lm(&s0, &id_src->addr, decinfo.width);
  
  switch(decinfo.width) {
    case 2: rtl_sext(&s1, &s0, 2); print_asm_template2(lh);break;
    case 1: rtl_sext(&s1, &s0, 1); print_asm_template2(lb);break;
    default: assert(0);
  }

  rtl_sr(id_dest->reg, &s1, 4);  
}


make_EHelper(st) { /* void exec_st(vaddr_t *pc) */

  /*
   * rtl_sm -> interpret_rtl_sm -> vaddr_write()
   */
  rtl_sm(&id_src->addr, &id_dest->val, decinfo.width);

  switch (decinfo.width) {
    case 4: print_asm_template2(sw); break;
    case 2: print_asm_template2(sh); break;
    case 1: print_asm_template2(sb); break;
    default: assert(0);
  }
}
