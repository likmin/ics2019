#include "cpu/exec.h"

make_EHelper(lui) { /* void exec_lui(vaddr_t *pc) */

  /*          rtl_sr(id_dest->reg, &id_src->val, 4)      
   *              |     |             |          
   *              |     |r            |*src1      
   *              |     |             |
   *          rtl_mv(&reg_l[r],      *src1) // r!=0 ,'nemu/src/isa/riscv32/include/isa/rtl.h'
   *              |     |             |        
   *              |     |             |
   *              |     |             |
   *interpret_rtl_mv(*dest,          *src1) //       'nemu/include/rtl/rtl.h'
   *            
   *            *dest <- *src1
   * 
   */

  rtl_sr(id_dest->reg, &id_src->val, 4); 

  /* print the asm */
  print_asm_template2(lui);
}


make_EHelper(auipc) {
  rtl_add(&id_dest->val, &cpu.pc, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(auipc);
}


make_EHelper(addi) { /* void exec_addi(vaddr_t *pc) */
  /* compute,
   * id_dest->val = id_src->val + id_src2->val
   */  
  rtl_add(&id_dest->val, &id_src->val, &id_src2->val);

  /*
   * write back:
   *  reg_l[id_dest->reg] = id_dest->val 
   */
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template2(addi);
}