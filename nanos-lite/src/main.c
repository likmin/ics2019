#include "common.h"

void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

extern const char logo[];

int main() {
  printf("%s", logo); 
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

#ifdef HAS_VME
  init_mm();
#endif

  init_ramdisk();

  init_device();

#ifdef HAS_CTE
  init_irq();
#endif

  init_fs();

  init_proc(); 


  Log("Finish initialization");

#ifdef HAS_CTE
  /* nanos-lite/src/main.c _yield()
   *              |
   * nexus-am/am/src/riscv32/nemu/cte.c _yield() // 执行ecall指令
   *              |
   *        nemu中经过一系列解码
   *              |
   * nemu/src/isa/riscv32/exec/system.c make_EHelper(ECALL_EBREAK) 
   *      raise_intr(9, cpu.pc)
   *              |
   * nemu/src/isa/riscv32/intr.c  void raise_intr(uint32_t NO, vaddr_t epc)                
   *        1. save the pc to epc
   *        2. set the exception code to scause
   *        3. get the exception entry address from stvec register
   *        4. jump to exception entry address ----+
   *                                               | 
   *                                          跳入陷入指令，即__am_asm_trap（nexus-am/am/src/riscv32/nemu/trap.S）
   *                                            1.保留上下文，即_Context,这里需要根据__am_asm_trap中的指令执行顺序
   *                                              对_Context中的顺序进行更改，否则会驴唇不对马嘴  
   *                                            2.跳转至真正的异常处理程序，即__am_irq_handle（nexus-am/am/src/riscv32/nemu/cte.c) 
   *                                                                           |
   *                                                                           |
   *                                                      1.根据上下文中的cause, 确定_Event类型, 接下来就交给操作系统来处理了
   *                                                      2.nanos-lite/src/irq.c中的do_event()函数会根据_Event类型决定怎么处理，
   *                                                
   *                                            3.恢复上下文, sret  
   *                                                          |
   *                         +--------------------------------+
   *                         |
   *                 继续执行下一条指令
   */
  printf("main\n");
  _yield();
 
#endif
  
  
  panic("Should not reach here");
}
