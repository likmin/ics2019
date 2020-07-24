#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

/* Elf_Ehdr -  ELF Header的信息，以struct结构体形式呈现
 *    e_entry     - 程序的入口地址, 第一条指令的地址, riscv32为0x80100000, x86为0x30000    
 *    e_phoff     - Program header table在文件中的偏移量，Phdr在Ehdr后面，所以e_phoff大小也为Elf_Ehdr大小, 
 *                  通过该信息定位程序头表
 *    e_shoff     - Section header table在文件中的偏移量，通过该信息定位节头表
 *    e_ehsize    - ELF header大小，与e_phoff相同
 *
 *    e_phentsize - Program header table中每一个条目的大小
 *    e_phnum     - Program header table中有多少条目
 *
 *    e_shentsize - Section header table中每一个条目的大小
 *    e_shnum     - Section header table中有多少条目
 *
 *    e_shstrndx  - 包含节名称的字符串是第几个节
 *
 * Elf_Phdr - ELF Program Header Data Structure, elf文件中的哪一部分(节)搬运到内存中的哪个位置(段)
 *            *可选，仅可执行文件有，可重定位文件没有*
 *            Type      Offset         VirtAddr     PhysAddr     FileSiz    MemSiz   Flg   Align
 *            
 *            如果Type为LOAD, 则需要装载, 将文件中从Offset开始位置，连续FileSiz个字节的数据，装载
 *            到内存中从VirtAddr开始，连续MemSiz个字节的区域中.
 *            MemSiz可能大于FileSiz, 这时候需要把内存中VirtAddr + [FileSiz, MemSiz]的区域清理，但为什么呢？
 *            思考: elf中需要载入的有.text, .data, .bss节等。其中.text为程序代码, .data是已经初始化的全局变量数据，
 *                  而.bss为未初始化全局变量，但其实什么也没有存。所以在载入内存时MemSiz要比FileSiz大。
 *           
 *                            
 *
 * Elf_Shdr - ELF Section Header Data Structure, elf有许多小节组成：
 *            .text   - 如用于存储程序代码的节
 *            .data   - 存放已经初始化程序变量的取值的节
 *            .symtab - 符号表, 用于存储全局变量或函数——如main，文件，还有一些elf中每一小节的名字及其对应内存的哪个地址
 *            .strtab - 字符串表，
 *           
 *            节头表作用就是用于索引这些节.
 *             
 * readelf -h Elf头文件
 *         -S Elf节头表
 *         -l Elf程序头表
 *         -s Elf符号表
 */
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  /* 1. 读入ELF header
   * 2. 程序入口地址 = e_entry
   * 3. 定位程序头表，根据Elf_Ehdr的e_entry + e_phoff找到程序头表
   * 4. for 程序头表的每一项（e_phnum）
   *        if Type == Load then
   *            执行装载到内存，如果MemSiz>FileSiz，还要将大于的部分清零
   *        end if
   *    end for
   * 5. return 程序入口地址
   */
  // Elf_Ehdr *elf; 
  // Elf_Phdr *ph, *eph; 

  // elf = malloc(52); 
  // ramdisk_read(elf, 0, 52); 
  // Log("1. read the Elf header from ramdisk");
  
  // /* 2.程序入口地址 */
  // volatile uint32_t entry = elf->e_entry;
  // Log("2. ELF Entry address");
  // /* 3.定位程序头表 */
  // ph = (void *)elf + elf->e_phoff;
  // Log("3. Located The ELF Program header table");
  // /* 4.装载程序头表中的每一项 */
  // eph = ph + elf->e_phnum;
  // Log("4. Load ELF Program header"); 
  // for (; ph < eph; ph++) {  //扫描程序头表中的各个表项
  //   Log("start for ");
  //   if (ph->p_type == PT_LOAD) {
  //     /* 
  //      * 1. 将文件中从Offset开始位置，连续FileSiz个字节的数据，装载到内存中从VirtAddr开始，连续MemSiz个字节的区域中.
  //      * 2. 如果MemSiz > FileSiz, 把内存中VirtAddr + [FileSiz, MemSiz]的区域清理
  //      */
  //     void *memp = (void *)(ph->p_vaddr);
  //     ramdisk_read(memp, ph->p_offset, ph->p_filesz);

  //     if(ph->p_filesz < ph->p_memsz) 
  //       memset((void *)(ph->p_offset + ph->p_filesz), 0, ph->p_memsz - ph->p_filesz);
   
  //   }
  // }

  // /* 5.return 程序入口地址*/
  // return entry;

  Elf_Ehdr header;
   Log("size of header is %d\n", sizeof(header));
  ramdisk_read(&header, 0, sizeof(header));
  uint32_t phdr_offset = header.e_phoff;
  uint16_t phnum = header.e_phnum;
   Log("num of headers is %d\n", phnum);
  uint16_t phentsize = header.e_phentsize;
  uint32_t program_entry = header.e_entry;
   Log("size of entrysize is %d\n", phentsize);

  Elf_Phdr phdr[phnum];
  char buf[25000];
  ramdisk_read(&phdr, phdr_offset, phnum*phentsize);
  for (int index=0; index<phnum; index++) {
    uint32_t pt_load = phdr[index].p_type;
    if (pt_load != PT_LOAD) continue;
    uint32_t entry_offset = phdr[index].p_offset;
    uint32_t entry_filesize = phdr[index].p_filesz;
    uint32_t entry_memsize = phdr[index].p_memsz;
    uint32_t entry_vaddr = phdr[index].p_vaddr;
    // Log("entry_offset=%x, entry_filesize=%x, entry_memsize=%x, entry_vaddr=%x, type=%x\n------------------\n", entry_offset, entry_filesize, entry_memsize, entry_vaddr, pt_load);

    //血与痛的教训。buf切勿定义过大，否则极有可能缓冲区溢出导致覆盖IDTR。
    int left = entry_filesize;
    while (left>0) {
      if (left>=25000){
        ramdisk_read(buf, entry_offset, 25000);
        left-=25000;
      }
      else {
        ramdisk_read(buf, entry_offset, left);
        left-=left;
      }
      // printf("%p\n", (void*) entry_vaddr);
      memcpy((void*) entry_vaddr, buf, entry_filesize);
    }
    memset((void*) entry_vaddr+entry_filesize, 0, entry_memsize-entry_filesize);
  }
  return program_entry;

  Log("-------------------------------------------------------------------");
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
