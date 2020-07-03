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
  // rtl_sr(id_src->reg, &id_src->val, 4); 

  /* print the asm */
  print_asm_template2(lui);
}


make_EHelper(auipc) {
  rtl_add(&id_dest->val, &cpu.pc, &id_src->val); // correct 'id_src2' to 'id_src'
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
  /* 
   * x[rd] = x[rs1] << shamt，这里的shamt对应的是rs2的寄存器编号
   */
  rtl_shl(&id_dest->val, &id_src->val, &id_src2->reg);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(slli); 
 
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
  /* warning: 对于srli和srai以及slli，第二个操作数都是shamt即rs2 */ 
  switch(decinfo.isa.instr.funct7) {
 
    case 0b0000000: // srli  
                    rtl_shr(&id_dest->val, &id_src->val, &id_src2->reg);
                    print_asm_template2(srli); 
                    break;
 
    case 0b0100000: // srai
                    rtl_sar(&id_dest->val, &id_src->val, &id_src2->reg);
                    print_asm_template2(srai);
                    break;

    default       : assert(0);
  }
  
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(ori) { /*void exec_ori(vaddr_t *pc), funct3 == 110*/
  rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(ori); 
}

make_EHelper(andi) { /*void exec_ori(vaddr_t *pc), funct3 == 111*/
  rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(andi);
}



/* R-type */
make_EHelper(sub_add) {           
 
  switch(decinfo.isa.instr.funct7) {

    case 0b0000000: /* add */
                    rtl_add(&id_dest->val, &id_src->val, &id_src2->val); 
                    print_asm_template3(add);
                    break;

    case 0b0100000: /* sub */ 
                    rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(sub);
                    break;

    case 0b0000001: /* mul */
                    rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(mul);
                    break;

    default       : assert(0);
  } 

  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(sll) { /*void exec_sll(vaddr_t *pc), funct3 == 001*/
  
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* sll */
                    rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(sll); 
                    break;

    case 0b0000001: /* mulh, x[rd] = (x[rs1]s *s x[rs2]) >>s XLEN, 
                     * 因为使有符号乘法，所以选择imul_hi
                     */
                    rtl_imul_hi(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(mulh);
                    break;

    default       : assert(0);
  }

  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(slt) { /*void exec_slt(vaddr_t *pc), funct3 == 010*/
  
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* slt */
                    rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
                    print_asm_template3(slt);
                    break;

    case 0b0000001: /* mulhsu */
                    rtl_mul_hsu(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(mulhsu);
                    break;

    default       : assert(0);
  }
   
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(sltu) { /*void exec_sltu(vaddr_t *pc), funct3 == 011*/
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* sltu */
                    rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
                    print_asm_template3(sltu); 
                    break;
    
    case 0b0000001: /* mulhu */
                    rtl_mul_hi(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(mulhu);
                    break;

    default       : assert(0);
  } 
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(xor) { /*void exec_xor(vaddr_t *pc), funct3 == 100*/

  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* xor */
                    rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(xor);
                    break;

    case 0b0000001: /* div */
                    rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(div);   
                    break;

    default       : assert(0);
    
  }

  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(srl_sra) { /*void exec_srl_sra(vaddr_t *pc), funct3 == 101*/

  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* srli */
                    rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(srl); 
                    break;
    
    case 0b0100000: /* srai */
                    rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(sra);
                    break;

    case 0b0000001: /* divu */
                    rtl_div_q(&id_dest->val, &id_src->val, &id_src2->val);                
                    print_asm_template3(divu);
                    break;

    default       : assert(0);
  }
  
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(or) { /*void exec_or(vaddr_t *pc), funct3 == 110*/
  
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* or */
                    rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(or);
                    break; 

    case 0b0000001: /* rem */
                    rtl_idiv_r(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(rem); 
                    break;

    default       : assert(0);
  }

  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(and) { /*void exec_and(vaddr_t *pc), funct3 == 111*/
  
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: /* and */
                    rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(and); 
                    break;

    case 0b0000001: /* remu */
                    rtl_div_r(&id_dest->val, &id_src->val, &id_src2->val);
                    print_asm_template3(remu);
                    break;

    default       : assert(0);
  }

  rtl_sr(id_dest->reg, &id_dest->val, 4);
}
