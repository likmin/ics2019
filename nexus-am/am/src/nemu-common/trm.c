#include <am.h>
#include <nemu.h>

extern char _heap_start;
extern char _heap_end;
int main(const char *args);

/* 用于指示堆区的起始和末尾 */
_Area _heap = {
  .start = &_heap_start,
  .end = &_heap_end,
};

/* 用于输出一个字符,通过串口输出的
 */
void _putc(char ch) {
  outb(SERIAL_PORT, ch); /* outb(0x3F8, ch);*/
}

/* 用于结束程序的运行 */
void _halt(int code) {
  nemu_trap(code);

  // should not reach here
  while (1);
}

/* 用于进行TRM相关的初始化工作 */
void _trm_init() {
  extern const char _start;
  const char *mainargs = &_start - 0x100000;
  int ret = main(mainargs);
  _halt(ret);
}
