#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

/* Elf_Ehdr - ELF头文件的信息，以struct结构体形式呈现
 *    e_entry     - 程序的入口地址    
 *    e_phoff     - Program header table在文件中的偏移量
 *    e_shoff     - Section header table在文件中的偏移量
 *    e_ehsize    - ELF header大小
 *
 *    e_phentsize - Program header table中每一个条目的大小
 *    e_phnum     - Program header table中有多少条目
 *
 *    e_shentsize - Section header table中每一个条目的大小
 *    e_shnum     - Section header table中有多少条目
 *
 *    e_shstrndx  - 包含节名称的字符串是第几个节
 
 * Elf_Shdr - ELF Section Header Data Structure, 包括代码段、数据段、重定位段等信息
 * Elf_Phdr - ELF Program Header Data Structure, 描述的是一个段在文件中的位置, 大小以及它被放进内存后所在的位置和大小
 * 
 */
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  ramdisk_write(Elf32_Phdr, 0, get_ramdisk_size());
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
