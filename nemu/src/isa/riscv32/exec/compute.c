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

/*
 * imm_table, use to decode the following instruction:
 *   instr funct3
 * - addi , 000;
 * - slli , 001;
 * - stli , 010;
 * - stliu, 011;
 * - xori , 100;
 * - srli,srai, 101;
 * - ori  , 110;
 * - andi , 111;
 * 这里写的太啰嗦了，I-type和R-type的计算都是类似的，我觉得可以共用，但是print_asm_template不同该怎么办呢？
 * TODO：思考I-type和R-type如何共用一个计算函数。
 */

// make_EHelper(addi) { /* void exec_addi(vaddr_t *pc),funct3 == 000*/
//   /* compute,
//    * id_dest->val = id_src->val + id_src2->val
//    */  
//   rtl_add(&id_dest->val, &id_src->val, &id_src2->val);

//   /*
//    * write back:
//    *  reg_l[id_dest->reg] = id_dest->val 
//    */
//   rtl_sr(id_dest->reg, &id_dest->val, 4);

//   print_asm_template2(addi);
// }

// make_EHelper(slli) { /*void exec_slli(vaddr_t *pc), funct3 == 001*/
//   rtl_shl(&id_dest->val, id_src->val, &id_src2->val);
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(slli);
// }

// make_EHelper(stli) { /*void exec_stli(vaddr_t *pc), funct3 == 010*/
//   rtl_li(id_dest, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(stli);
// }

// make_EHelper(stliu) { /*void exec_stliu(vaddr_t *pc), funct3 == 011*/
//   rtl_li(id_dest, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(stliu);
// }

// make_EHelper(xori) { /*void exec_xori(vaddr_t *pc), funct3 == 100*/
//   rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(xori);
// }

// make_EHelper(srli_srai) { /*void exec_srli_srai(vaddr_t *pc), funct3 == 101*/
//   if(decinfo.isa.instr.funct7 == 0b0100000) { /* srai */
//     rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
//     rtl_sr(id_dest->reg, &id_dest->val, 4);
//     print_asm_template2(srai);
//   }
//   else {                                     /* srli */
//     rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
//     rtl_sr(id_dest->reg, &id_dest->val, 4);
//     print_asm_template2(srli);
//   }
// }

// make_EHelper(ori) { /*void exec_ori(vaddr_t *pc), funct3 == 110*/
//   rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(ori);
// }

// make_EHelper(andi) { /*void exec_ori(vaddr_t *pc), funct3 == 111*/
//   rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
//   print_asm_template2(andi);
// }

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

/*
 * 问：怎样让R-type结合I-type共用这个计算函数呢？
 * 答：利用OP_TYPE，我们约定I-type的立即数统一写入到id_src2中，
 *     通过判断id_src2的类型来决定print_asm_template2还是print_asm_template3
 *  
 */
 
static inline void arith_logic(rtlreg_t *dest, const rtlreg_t *src1, const rtlreg_t *src2) {
  switch(decinfo.isa.instr.funct3) {
    case R_add_sub: 
                    if(decinfo.isa.instr.funct7) /*sub*/
                      rtl_sub(dest, src1, src2); 
                    else {
                      rtl_add(dest, src1, src2); /*add, addi*/
                      if(src2->type == OP_TYPE_IMM) print_asm_template2(addi); else print_asm_template3(add); 
                    }                         
                    break;

    case R_sll    : 
                    rtl_shl(dest, src1, src2);  
                    if(src2->type == OP_TYPE_IMM) print_asm_template2(slli); else print_asm_template3(sll); 
                    break;

    case R_slt    : 
                    rtl_li(dest, interpret_relop(RELOP_LT, *src1, *src2));  
                    if(src2->type == OP_TYPE_IMM) print_asm_template2(slti); else print_asm_template3(slt); 
                    break;

    case R_sltu   : 
                    rtl_li(dest, interpret_relop(RELOP_LTU, *src1, *src2));  
                    if(src2->type == OP_TYPE_IMM) print_asm_template2(sltui); else print_asm_template3(sltu); 
                    break;

    case R_xor    : 
                    rtl_xor(dest, src1, src2); 
                    if(src->type == OP_TYPE_IMM) print_asm_template2(xori); else print_asm_template3(xori);
                    break; 
    case R_srl_sra:
                    if(decinfo.isa.instr.funct7) {
                      rtl_shr(dest, src1, src2); 
                      if(src2->type == OP_TYPE_IMM) print_asm_template2(srli); else print_asm_template3(srl); 
                    } else {
                      rtl_sar(dest, src1, src2); 
                      if(src2->type == OP_TYPE_IMM) print_asm_template2(srai); else print_asm_template3(sra); 
                    }                         
                    break;
    case R_or     : 
                    rtl_or(dest, src1, src2);
                    if(src2->type == OP_TYPE_IMM) print_asm_template2(ori); else print_asm_template3(or); 
                    break;
    case R_and    :
                    rtl_and(dest, src1, src2); 
                    if(src2->type == OP_TYPE_IMM) print_asm_template2(andi); else print_asm_template3(and);  
                    break;
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(r) {
  arith_logic(&id_dest->val, &id_src->val, &id_src2->val);
}

