#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context { /* riscv32 上下文信息,更改了_Context中的顺序，否则在trap.S保存上下文时，会乱 */
  uintptr_t gpr[32], cause, status, epc;
  struct _AddressSpace *as;
};

#define GPR1 gpr[17]
#define GPR2 gpr[0]
#define GPR3 gpr[0]
#define GPR4 gpr[0]
#define GPRx gpr[0]

#endif
