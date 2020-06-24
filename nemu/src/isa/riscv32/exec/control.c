#include "cpu/exec.h"

make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
   // rtl_add(&t0, &cpu.pc, 4);
   //printf("[interpret_rtl_jal] pc = 0x%x\n", *pc);
   //printf("[interpret_rtl_jal] pc+ id_src = 0x%x\n", *pc + id_src->val);
   
   rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
   //printf("jmp_pc = 0x%x\n", decinfo.jmp_pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template1(jal);
}

make_EHelper(jalr) {
    rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
    t0 = cpu.pc + 4;
    rtl_sr(id_dest->reg, &t0, 4);
    interpret_rtl_j(decinfo.jmp_pc); /* interpret_rtl_jr(&decinfo.jmp_pc); */
    print_asm_template2(jalr);   

    printf("[interpret_rtl_jalr] id_src = 0x%x, pc = 0x%x\n",id_src->val, cpu.pc); 
}