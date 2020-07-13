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

make_EHelper(CSRRW) {
  /* t        = CSR[csr]
   * CSR[csr] = x[rs1]
   * x[rd]    = t
   */
  t0 = csr_read(id_src2->val);
  csr_write(id_src2->val, id_src);
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
  rtl_and(&t1, &t0, &s0);       /* t1 = t0 | s0  */
  csr_write(id_src2->val, t1); /* CSR[csr] = t1 */
  rtl_sr(id_dest->reg, &t0, 4);/* x[rd] = t0    */
}

make_EHelper(CSRRWI) {
 /* x[rd] = CSR[csr]
  * CSR[csr] = zimm
  */

  t0 = csr_read(id_src2->val);
  rtl_sr(id_dest->reg, &t0, 4);/* x[rd] = CSR[csr] */
  csr_write(id_src2->val, id_src->reg) /* CSR[csr] = zimm */
}

make_EHelper(CSRRSI) {
 /* t = CSR[csr]
  * CSR[csr] = t | zimm
  * x[rd] = t
  */

  t0 = csr_read(id_src2->val);
  rtl_or(&t1, &t0, &id_src->reg);
  rtl_sr(id_dest->reg, &t0, 4); 
}

make_EHelper(CSRRCI) {
  /* t = CSR[csr]
   * CSR = t & ~zimm
   * x[rd] = t
   */

   t0 = csr_read(id_src2->val);
   rtl_not(&t1, &id_src->reg);
   rtl_and(&r0, &t0, &t1);
   rtl_sr(id_dest->reg, &t0, 4);
}