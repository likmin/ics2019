#include <am.h>
#include <amdev.h>

void __am_vga_init();
void __am_timer_init();

int _ioe_init() {
  __am_vga_init();
  __am_timer_init();
  return 0;
}

size_t __am_timer_read(uintptr_t reg, void *buf, size_t size);    /* nemu-timer.c */
size_t __am_video_read(uintptr_t reg, void *buf, size_t size);    /* nemu-video.c */
size_t __am_video_write(uintptr_t reg, void *buf, size_t size);   /* nemu-video.c */
size_t __am_input_read(uintptr_t reg, void *buf, size_t size);    /* nemu-input.c */

/* _io_read: 从编号为'dev'的设备的'reg'寄存器中读出'size'字节的内容到缓冲区'buf'中。
 * _io_write: 向编号为'dev'的设备的'reg'寄存器中写入'buf'中的'size'字节的内容。
 * @warning: 这里的reg并不是设备寄存器，设备寄存器的编号与架构相关，
 * 而在这里'reg'是一个'功能编号',这样可以做到架构无关，同一个功能编号的含义是相同的。
 * 
 */
size_t _io_read(uint32_t dev, uintptr_t reg, void *buf, size_t size) {
  switch (dev) {
    case _DEV_INPUT: return __am_input_read(reg, buf, size);
    case _DEV_TIMER: return __am_timer_read(reg, buf, size);
    case _DEV_VIDEO: return __am_video_read(reg, buf, size);
  }
  return 0;
}

size_t _io_write(uint32_t dev, uintptr_t reg, void *buf, size_t size) {
  switch (dev) {
    case _DEV_VIDEO: return __am_video_write(reg, buf, size);
  }
  return 0;
}
