#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  printf("[irq] c.scause = %d\n", c->scause);
  switch (e.event) {
    case _EVENT_YIELD: printf("self int\n"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
