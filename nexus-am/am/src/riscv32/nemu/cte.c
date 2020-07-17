#include <am.h>
#include <riscv32.h>


/* 上下文管理 ConText Extension */
static _Context* (*user_handler)(_Event, _Context*) = NULL;


/* 将执行流切换的原因打包成事件，然后调用在`_cte_init()`中注册的事件处理回调函数， 
 * 将事件交给Nanos-lite处理。
 * 
 * 在回调函数`nanos-lite/src/irq.c`中的`do_event()`函数会根据事件类型再次进行分发
 */
_Context* __am_irq_handle(_Context *c) {
  _Context *next = c;
 // Log("epc = %d, cause = %d, status = %d\n", c->epc, c->cause, c->status);
  if (user_handler) {
    _Event ev = {0};
    switch (c->cause) {
      case 9 : ev.event = _EVENT_YIELD; break;
      default: ev.event = _EVENT_YIELD; break;
     // default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }

  return next;
}

extern void __am_asm_trap(void);

/* 直接将异常入口地址设置到stvec寄存器中 */
int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

/* 进行自陷操作，会触发一个编号为_EVENT_YIELD事件 */
void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
