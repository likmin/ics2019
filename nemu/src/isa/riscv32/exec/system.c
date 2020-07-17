#include "cpu/exec.h"
void raise_intr(uint32_t NO, vaddr_t epc);

static inline void check_csr_index(int index) {
  assert(index >= 0 && index <= 0xfff);
}

rtlreg_t csr_read(int index) {
  check_csr_index(index);
  switch(index) {
    case SSTATUS: return cpu.csr.sstatus;
    case STVEC  : return cpu.csr.stvec;
    case SEPC   : return cpu.csr.sepc;
    case SCAUSE : return cpu.csr.scause;
    default: assert(0);
  }
}

void csr_write(int index, rtlreg_t val) {
  check_csr_index(index);
  switch(index) {
    case SSTATUS: cpu.csr.sstatus = val; break;
    case STVEC  : cpu.csr.stvec   = val; break;
    case SEPC   : cpu.csr.sepc    = val; break;
    case SCAUSE : cpu.csr.scause  = val; printf("scause = 0x%08x\n", cpu.csr.scause);break;
    default   : assert(0);
  }
}

make_EHelper(ECALL_EBREAK) { /* void exec_ECALL_EBREAK */
    
    
    if (decinfo.isa.instr.rd == 0b00000 && decinfo.isa.instr.rs1 == 0b00000) {
        
        printf("simm11_0 = %d, pc=0x%08x\n", decinfo.isa.instr.simm11_0, decinfo.seq_pc);
        
        switch (decinfo.isa.instr.simm11_0) {
          case 0: /* ECALL */ raise_intr(9, cpu.pc); print_asm_template1(ecall); break;
          case 1: /* EBREAK */ TODO(); break;
          default:assert(0);
        }

    } else assert(0);
}
/* NOTE: 
 *      1.system instruction is I-type, so 'csr' has been transform to id_src2->val
 */
make_EHelper(CSRRW) {
  /* t        = CSR[csr]
   * CSR[csr] = x[rs1]
   * x[rd]    = t
   */
  t0 = csr_read(id_src2->val);
  csr_write(id_src2->val, id_src->val);
  rtl_sr(id_dest->reg, &t0, 4);

}

make_EHelper(CSRRS) {
  /* t        = CSR[csr]
   * CSR[csr] = t | x[rs1]
   * x[rd]    = t
   */
  t0 = csr_read(id_src2->val);
  rtl_or(&t1, &t0, &id_src->val);
  csr_write(id_src2->val, t1);
  rtl_sr(id_dest->reg, &t0, 4);
}

make_EHelper(CSRRC) {
   /* t        = CSR[csr]
   * CSR[csr] = t & ~x[rs1]
   * x[rd]    = t
   */
  t0 = csr_read(id_src2->val); /* t0 = CSR[csr] */
  rtl_not(&s0, &id_src->val);  /* s0 = ~x[rs1]  */
  rtl_and(&t1, &t0, &s0);      /* t1 = t0 | s0  */
  csr_write(id_src2->val, t1); /* CSR[csr] = t1 */
  rtl_sr(id_dest->reg, &t0, 4);/* x[rd] = t0    */
}

make_EHelper(CSRRWI) {
 /* x[rd] = CSR[csr]
  * CSR[csr] = zimm
  */

  t0 = csr_read(id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);        /* x[rd] = CSR[csr] */
  csr_write(id_src2->val, id_src->reg); /* CSR[csr] = zimm  */
}

make_EHelper(CSRRSI) {
 /* t = CSR[csr]
  * CSR[csr] = t | zimm
  * x[rd] = t
  */

  t0 = csr_read(id_src2->val);
  rtl_or(&t1, &t0, &id_src->reg);
  csr_write(id_src2->val, t1);
  rtl_sr(id_dest->reg, &t0, 4); 
}

make_EHelper(CSRRCI) {
  /* t = CSR[csr]
   * CSR[csr] = t & ~zimm
   * x[rd] = t
   */

   t0 = csr_read(id_src2->val);
   rtl_not(&t1, &id_src->reg);
   rtl_and(&s0, &t0, &t1);
   csr_write(id_src2->val, s0);
   rtl_sr(id_dest->reg, &t0, 4);
}