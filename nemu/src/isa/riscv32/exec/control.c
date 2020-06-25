#include "cpu/exec.h"

make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
   t0 = cpu.pc + 4;
   rtl_sr(id_dest->reg, &t0, 4);
   rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
   //printf("jmp_pc = 0x%x\n", decinfo.jmp_pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template1(jal);
}

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
