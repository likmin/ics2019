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

enum {             // funct3
  R_add_sub = 0 | 0 | 0 , // 000
  R_sll     = 0 | 0 | 1 , // 001
  R_slt     = 0 | 2 | 0 , // 010
  R_sltu    = 0 | 2 | 1 , // 011
  R_xor     = 4 | 0 | 0 , // 100
  R_srl_sra = 4 | 0 | 1 , // 101
  R_or      = 4 | 2 | 0 , // 110
  R_and     = 4 | 2 | 1  // 111
};

static inline void arith_logic(rtlreg_t *dest, const rtlreg_t *src1, const rtlreg_t *src2) {
  switch(decinfo.isa.instr.funct3) {
    case R_add_sub: if(decinfo.isa.instr.funct7) rtl_sub(dest, src1, src2); else rtl_add(dest, src1, src2); return;
    case R_sll    : rtl_shl(dest, src1, src2); return;
    case R_slt    : rtl_li(dest, interpret_relop(RELOP_LT, *src1, *src2)); return;
    case R_sltu   : rtl_li(dest, interpret_relop(RELOP_LTU, *src1, *src2)); return;
    case R_xor    : rtl_xor(dest, src1, src2); return;
    case R_srl_sra: if(decinfo.isa.instr.funct7) rtl_shr(dest, src1, src2); else rtl_sar(dest, src1, src2); return;
    case R_or     : rtl_or(dest, src1, src2); return;
    case R_and    : rtl_and(dest, src1, src2); return;
    default       : assert(0);
  }
}

make_EHelper(r) {
  arith_logic(&id_dest->val, &id_src->val, &id_src2->val);
}