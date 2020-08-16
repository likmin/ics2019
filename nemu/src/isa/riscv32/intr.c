#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  /* for riscv32
   * 1. save the pc to epc
   * 2. set the exception code to scause
   * 3. get the exception entry address from stvec register
   * 4. jump to exception entry address 
   */

  /* 你可能会问：stvec的值是从哪里来的？
   * stvec寄存器用来存放异常入口地址的，也就是__am_asm_trap的地址，
   * 在操作系统初始化时，_cte_init()函数中就已经将__am_asm_trap的
   * 地址通过内敛函数传递给了stvec。
   */
  csr_write(SEPC, epc);  /* 1.save the pc to epc                */
  csr_write(SCAUSE, NO); /* 2.set the exception code to scause  */
  t0 = csr_read(STVEC);  /* 3.get the exception entry address from stvec register */ 
  interpret_rtl_jr(&t0); /* 4.jump to exception entry address   */
}

bool isa_query_intr(void) {
  return false;
}
