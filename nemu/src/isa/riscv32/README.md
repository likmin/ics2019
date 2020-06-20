1. riscv32目录结构

   ```shell
   riscv32
     |-- exec
     	|-- all-instr.h // 所有支持的指令执行函数的声明 
     	|-- compute.c  // 一些计算型指令执行函数的具体实现
   		|-- control.c  // 现在还没有用到
   		|-- exec.c   // 先根据opcode_table对指令的opcode进行译码并执行，有些指令还要根据funct3,funct7进一步译码再执行。
   		|-- ldst.c   // load 和 store指令执行函数
   		|-- muldiv.c  // 乘除法，但是现在还没有用到
   	    |-- special.c  // 无效指令inv和nemu陷入指令的执行函数
   		|-- system.c  // 尚未用到
     |-- include
   		|-- decode.h  // 包含isa指令结构，以及decode.c中六个译码函数的声明
   		|-- diff-test.h // 目前还不知道其作用
   		|-- reg.h // riscv32 GPRs和pc寄存器，组成了CPU_state。同时包含这些寄存器的正则表达式规则REG_REGEX
   		|-- rtl.h // 寄存器读取值rtl_lr, 寄存器写入值rtl_sr
     |-- decode.c  // 对riscv32的6种类型的指令进行译码
     |-- diff-test.c // 目前还不知道其作用
     |-- init.c // riscv32一个初始化程序
     |-- intr.c // 触发interrupt/exception的函数raise_intr。
     |-- logo.c // isa_logo - riscv Manual       
     |-- mmu.c  // isa_vaddr_read, isa_vaddr_write. mmu单元，通过虚地址访问内存，mmu会 转化为物理地址
     |-- reg.c  // riscv32寄存器相关函数，如寄存器显示函数isa_reg_display, 根据寄存器标识符返回寄存器值函数isa_reg_str2val 
   ```