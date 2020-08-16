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
    case SCAUSE : cpu.csr.scause  = val; break;
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
  //  cpu.csr.sstatus_32.SIE  = cpu.csr.sstatus_32.SPIE;
  //  cpu.csr.sstatus_32.SPIE = 1;
  //  cpu.csr.sstatus_32.SPP  = 0;
   interpret_rtl_jr(&t0);
}

/* ECALL EBREAK */
make_EHelper(ECALL_EBREAK) { /* void exec_ECALL_EBREAK */
    
    //printf("rd = %d, rs1 = %d\n", decinfo.isa.instr.rd, decinfo.isa.instr.rs1); 
    if (decinfo.isa.instr.rd == 0 && decinfo.isa.instr.rs1 == 0) {
        
        //printf("simm11_0 = %d, pc=0x%08x\n", decinfo.isa.instr.simm11_0, decinfo.seq_pc);
        
        switch (decinfo.isa.instr.simm11_0) {
          case 0b000000000000: /* ECALL */ 
          
            raise_intr(9, cpu.pc); 
            printf("cpu.pc = %8x, decinfo.seq_pc = %8x\n", cpu.pc, decinfo.seq_pc);
            print_asm_template1(ecall); 
            break;
          case 0b000000000001: /* EBREAK*/ TODO(); break;
          case 0b000100000010: /* SRET  */ SRET(); print_asm_template3(sret); break;
          default:assert(0);
        }

    } else assert(0);

    /* ecall执行顺序：
     *  1.raise_intr(),设置scause，sstatus，sepc及stvec，并跳到陷入程序，即__am_asm_trap（nexus-am/am/src/riscv32/nemu/trap.S）
     *  
     *  2.__am_asm_trap会做三件事：
     *    2.1 保留上下文，即_Context,这里需要根据__am_asm_trap中的指令执行顺序对_Context中的顺序进行更改，否则会驴唇不对马嘴
     *    2.2 跳转至真正的异常处理程序，即__am_irq_handle（nexus-am/am/src/riscv32/nemu/cte.c)
     *    2.3 恢复上下文
     *
     *  3.异常处理程序__am_irq_handle要做什么呢？
     *    3.1 根据上下文中的cause，确定_Event类型，ecall确定为_EVENT_YIELD, 接下来就交给操作系统来处理了
     *    3.2 nanos-lite/src/irq.c中的do_event()函数会根据_Event类型决定怎么处理，这里只输出`self int`
     *    
     */
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