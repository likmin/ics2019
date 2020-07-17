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
    case SCAUSE : cpu.csr.scause  = val; printf("[system] cause = 0x%08x\n", val);break;
    default   : assert(0);
  }
}

/* SRET */ 
static inline void SRET(){
  /* 1.设置pc为CSRs[sepc]
   * 2.权限模式位CSRs[sstatus].SPP, 0代表用户模式，1表示其他模式，pa中一直在S模式，所以不用切换
   * 3.CSRs[sstatus].SIE = CSRs[sstatus].SPIE
   * 4.CSRs[sstatus].SPIE = 1
   * 5.CSRs[sstatus].SPP = 0
   */
   t0 = csr_read(SEPC) + 4;
   printf("[SRET] t0 = 0x%08x\n", t0);
   cpu.csr.sstatus_32.SIE = cpu.csr.sstatus_32.SPIE;
   cpu.csr.sstatus_32.SPIE = 1;
   cpu.csr.sstatus_32.SPP = 0;
   interpret_rtl_jr(&t0);
}
make_EHelper(ECALL_EBREAK) { /* void exec_ECALL_EBREAK */
    
    //printf("rd = %d, rs1 = %d\n", decinfo.isa.instr.rd, decinfo.isa.instr.rs1); 
    if (decinfo.isa.instr.rd == 0 && decinfo.isa.instr.rs1 == 0) {
        
        //printf("simm11_0 = %d, pc=0x%08x\n", decinfo.isa.instr.simm11_0, decinfo.seq_pc);
        
        switch (decinfo.isa.instr.simm11_0) {
          case 0b000000000000: /* ECALL */ raise_intr(9, cpu.pc); print_asm_template1(ecall); break;
          case 0b000000000001: /* EBREAK*/ TODO(); break;
          case 0b000100000010: /* SRET  */ SRET(); print_asm_template3(sret); break;
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
  print_asm_template3(csrrw);
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
  print_asm_template3(csrrs);
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
  print_asm_template3(csrrc);
}

make_EHelper(CSRRWI) {
 /* x[rd] = CSR[csr]
  * CSR[csr] = zimm
  */

  t0 = csr_read(id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);        /* x[rd] = CSR[csr] */
  csr_write(id_src2->val, id_src->reg); /* CSR[csr] = zimm  */
  print_asm_template3(csrrwi);
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