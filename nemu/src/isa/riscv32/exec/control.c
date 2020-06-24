#include "cpu/exec.h"

make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
   
   decinfo.jmp_pc = id_src->val + id_src2->val;
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template1(jal);
}