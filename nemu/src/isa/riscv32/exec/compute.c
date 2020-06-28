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

make_EHelper(addi) { /* void exec_addi(vaddr_t *pc),funct3 == 000*/

  rtl_add(&id_dest->val, &id_src->val, &id_src2->val); // id_dest->val = id_src->val + id_src2->val
  rtl_sr(id_dest->reg, &id_dest->val, 4); // write back:  reg_l[id_dest->reg] = id_dest->val 
  print_asm_template2(addi);
}

make_EHelper(slli) { /*void exec_slli(vaddr_t *pc), funct3 == 001*/
  rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(slli); else print_asm_template3(sll); 

}

make_EHelper(slti) { /*void exec_stli(vaddr_t *pc), funct3 == 010*/
  rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(slti); else print_asm_template3(slt); 
}

make_EHelper(sltiu) { /*void exec_stliu(vaddr_t *pc), funct3 == 011*/
  rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(sltiu); else print_asm_template3(sltu); 
}

make_EHelper(xori) { /*void exec_xori(vaddr_t *pc), funct3 == 100*/
  rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(xori); else print_asm_template3(xor);
}

make_EHelper(srli_srai) { /*void exec_srli_srai(vaddr_t *pc), funct3 == 101*/
  if(decinfo.isa.instr.funct7 == 0b0100000) { /* srai */
    rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    if(id_src2->type == OP_TYPE_IMM) print_asm_template2(srai); else print_asm_template3(sra);
  }
  else {                                     /* srli */
    rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    if(id_src2->type == OP_TYPE_IMM) print_asm_template2(srli); else print_asm_template3(srl); 
  }
}

make_EHelper(ori) { /*void exec_ori(vaddr_t *pc), funct3 == 110*/
  rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(ori); else print_asm_template3(or); 
}

make_EHelper(andi) { /*void exec_ori(vaddr_t *pc), funct3 == 111*/
  rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  if(id_src2->type == OP_TYPE_IMM) print_asm_template2(andi); else print_asm_template3(and); 
}

make_EHelper(sub_add) {
  if(decinfo.isa.instr.funct7) {/* sub */
    rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
    print_asm_template3(sub);
  } else {
    rtl_add(&id_dest->val, &id_src->val, &id_src2->val); /* add */
    print_asm_template3(add);
  } 
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}


