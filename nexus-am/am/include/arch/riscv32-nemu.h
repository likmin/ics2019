#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context { /* riscv32 上下文信息 */
  uintptr_t gpr[32], scause, sstatus, sepc;
  struct _AddressSpace *as;
};

#define GPR1 gpr[17]
#define GPR2 gpr[0]
#define GPR3 gpr[0]
#define GPR4 gpr[0]
#define GPRx gpr[0]

#endif
