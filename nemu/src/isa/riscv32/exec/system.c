#include "cpu/exec.h"

make_EHelper(ECALL_EBREAK) { /* void exec_ECALL_EBREAK */
    
    
    if (decinfo.isa.instr.rd == 0b00000 && decinfo.isa.instr.funct3 == 0b000 &&
      decinfo.isa.instr.rs1 == 0b00000) {

        switch (decinfo.isa.instr.imm11_0) {
          case 0b000000000000: /* ECALL */ raise_intr();break;
          case 0b000000000001: /* EBREAK */ TODO(); break;
          default: assert(0);
        }

    } else assert(0);
}