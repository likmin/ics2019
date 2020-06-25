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

    t0 = cpu.pc + 4;
    rtl_sr(id_dest->reg, &t0, 4);

    rtl_add(&t1, &id_src->val, &id_src2->val);    
    rtl_mv(&decinfo.jmp_pc, &t1);
    interpret_rtl_j(decinfo.jmp_pc); /* interpret_rtl_jr(&decinfo.jmp_pc); */
    print_asm_template2(jalr);   

    printf("[interpret_rtl_jalr] id_src = 0x%x, pc = 0x%x\n",id_src->val, cpu.pc); 
}
