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

  csr_write(SEPC, epc); /* 1.save the pc to epc */
  csr_write()
  interpret_rtl_j()
}

bool isa_query_intr(void) {
  return false;
}
