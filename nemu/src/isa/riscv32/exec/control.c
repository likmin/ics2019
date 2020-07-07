#include "cpu/exec.h"

/*
 * J-type jal
 */
make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
   t0 = cpu.pc + 4;
   rtl_sr(id_dest->reg, &t0, 4);
   rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
   //printf("jmp_pc = 0x%x\n", decinfo.jmp_pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template2(jal);
}

/* 
 * I-type jalr
 */
make_EHelper(jalr) {

    /*
     * jalr rd,offset(rs1)
     * t = pc + 4
     * pc = x[rs1] + sext(offset) // TODO:pc = x[rs1] + sext(offset)&~1, is right? in riscv chinese manual.
     * x[rd] = t
     */
    t0 = cpu.pc + 4;
    rtl_add(&decinfo.jmp_pc, &id_src->val, &id_src2->val);    
    rtl_sr(id_dest->reg, &t0, 4);
    interpret_rtl_j(decinfo.jmp_pc); /* interpret_rtl_jr(&decinfo.jmp_pc); */

    print_asm_template2(jalr);   

//    printf("[interpret_rtl_jalr] id_src = 0x%x, pc = 0x%x\n",id_src->val, cpu.pc); 
}

/*
 * br_table, use to decode branch type:
 * instr  funct3  relop
 *  beq    000    RELOP_EQ
 *  bne    001    RELOP_NE
 *   -     010
 *   -     011
 *  blt    100    RELOP_LT
 *  bge    101    RELOP_GE
 *  bltu   110    RELOP_LTU
 *  bgeu   111    RELOP_GEU
 */
static uint32_t br_table[8] = {
    RELOP_EQ, RELOP_NE, RELOP_FALSE, RELOP_FALSE, RELOP_LT, RELOP_GE, RELOP_LTU, RELOP_GEU
};


/* TODO: remeber to print_asm_template
 */
make_EHelper(br) {
    rtl_jrelop(br_table[decinfo.isa.instr.funct3],
     &id_src->val, &id_src2->val, decinfo.jmp_pc);
    
    switch (br_table[decinfo.isa.instr.funct3]) {
        case RELOP_EQ: print_asm_template3(beq); break;
        case RELOP_NE: print_asm_template3(bne); break;
        case RELOP_LT: print_asm_template3(blt); break;
        case RELOP_GE: print_asm_template3(bge); break;
        case RELOP_LTU: print_asm_template3(bltu); break;
        case RELOP_GEU: print_asm_template3(bgeu); break;
        default: assert(0);
    }
}
