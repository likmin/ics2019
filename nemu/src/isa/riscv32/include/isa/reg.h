#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)

#define SSTATUS_NO 0x100
#define STVEC_NO   0x105
#define SEPC_NO    0x141
#define SCAUSE_NO  0x142



typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;

  struct {
    /* Supervisor Trap Setup */
    // 0x100
    struct {
      uint32_t sstatus_0    : 1;
      uint32_t SIE          : 1;
      uint32_t sstatus_4_2  : 3;
      uint32_t SPIE         : 1;
      uint32_t UBE          : 1;
      uint32_t sstatus_7    : 1;
      uint32_t SPP          : 1;
      uint32_t sstatus_12_9 : 4;
      uint32_t FS           : 2;
      uint32_t XS           : 2;
      uint32_t sstatus_17   : 1;
      uint32_t SUM          : 1;
      uint32_t MXR          : 1;
      uint32_t sstatu_30_20 :11;
      uint32_t SD           : 1;
    } sstatus;

    // 0x105
    rtlreg_t stvec;

    /* Supervisor Trap Handling */
    // 0x141
    rtlreg_t sepc;
    
    // 0x142
    struct {
      uint32_t Ecode        :31;
      uint32_t Interrupt    : 1;
    } scause;


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

static inline void check_csr_index(int index) {
  assert(index >= 0 && index <= 0xfff);
}

inline rtlreg_t csr_read(int index) {
  check_csr_index(index);
  switch(index) {
    case SSTATUS_NO: return cpu.csr.sstatus;
    case STVEC_NO  : return cpu.csr.stvec;
    case SEPC_NO   : return cpu.csr.sepc;
    case SCAUSE_NO : return cpu.csr.scause;
    default: return;
  }
}

inline void csr_write(int index, rtlreg_t val) {
  check_csr_index(index);
  switch(index) {
    case SSTATUS_NO: cpu.csr.sstatus = val; return;
    case STVEC_NO  : cpu.csr.stvec   = val; return;
    case SEPC_NO   : cpu.csr.sepc    = val; return;
    case SCAUSE_NO : cpu.csr.scause  = val; return;
    default   : assert(0);
  }
}
//#define REG_REGEX "^\\$0$|^\\$ra$|^\\$[sgt]p$|^\\$t[0-6]$|^\\$s[0-9]$|^\\$s1[01]$|^\\$a[0-7]$|^\\$pc$"  

#define REG_REGEX "^\\$0|^\\$ra|^\\$[sgt]p|^\\$t[0-6]|^\\$s[0-9]|^\\$s1[01]|^\\$a[0-7]|^\\$pc"  

#endif
