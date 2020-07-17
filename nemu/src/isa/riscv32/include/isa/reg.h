#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)

#define SSTATUS 0x100
#define STVEC   0x105
#define SEPC    0x141
#define SCAUSE  0x142

//extern CPU_STATE cpu;

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
  
  struct {
    union{
      struct {
        uint32_t SD           : 1;	
        uint32_t sstatu_30_20 :11;	
        uint32_t MXR          : 1;	
        uint32_t SUM          : 1;	
        uint32_t sstatus_17   : 1;	
        uint32_t XS           : 2;	
        uint32_t FS           : 2;	
        uint32_t sstatus_12_9 : 4;	
        uint32_t SPP          : 1;	
        uint32_t sstatus_7    : 1;	
        uint32_t UBE          : 1;	
        uint32_t SPIE         : 1;	
        uint32_t sstatus_4_2  : 3;	
        uint32_t SIE          : 1;	
        uint32_t sstatus_0    : 1;
      };
      rtlreg_t sstatus;
    };
    
    rtlreg_t stvec;
    rtlreg_t sepc;
    rtlreg_t scause;
  } csr;

} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  assert(index >= 0 && index < 32);
  return regsl[index];
}

rtlreg_t csr_read(int index);
void csr_write(int index, rtlreg_t val);

//#define REG_REGEX "^\\$0$|^\\$ra$|^\\$[sgt]p$|^\\$t[0-6]$|^\\$s[0-9]$|^\\$s1[01]$|^\\$a[0-7]$|^\\$pc$"  

#define REG_REGEX "^\\$0|^\\$ra|^\\$[sgt]p|^\\$t[0-6]|^\\$s[0-9]|^\\$s1[01]|^\\$a[0-7]|^\\$pc"  

#endif
