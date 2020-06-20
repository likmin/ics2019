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

2. task 2.1 实验步骤


   - 通过反汇编，确定dummy程序所需的指令及其类型

     ```assembly
     Disassembly of section .text:
     
     80100000 <_start>:
     80100000:	00000413          	li	s0,0		; 等同于lui+addi
     80100004:	00009117          	auipc	sp,0x9	
     80100008:	ffc10113          	addi	sp,sp,-4 # 80109000 <_end>
     8010000c:	00c000ef          	jal	ra,80100018 <_trm_init>
     
     Disassembly of section .text.startup:
     
     80100010 <main>:
     80100010:	00000513          	li	a0,0
     80100014:	00008067          	ret	; 等同于jarl x0, 0(x1)
     
     Disassembly of section .text._trm_init:
     
     80100018 <_trm_init>:
     80100018:	80000537          	lui	a0,0x80000
     8010001c:	ff010113          	addi	sp,sp,-16
     80100020:	00050513          	mv	a0,a0	; 实际为addi
     80100024:	00112623          	sw	ra,12(sp)
     80100028:	fe9ff0ef          	jal	ra,80100010 <_etext>
     8010002c:	00050513          	mv	a0,a0	
     80100030:	0000006b          	0x6b	; nemu_trap
     80100034:	0000006f          	j	80100034 <_trm_init+0x1c> ; 等同于jal
     ```

     

     | 指令  |       类型       | 完成 |
     | :---: | :--------------: | :--: |
     |  lui  |      U-type      |  √   |
     | auipc |      U-type      |      |
     | addi  | I-type，对应于ld |      |
     |  jal  |      J-type      |      |
     | jalr  |      J-type      |      |
     |  sw   | S-type，对应于st |  √   |

   - 指令执行过程


     - exec_once每次都会执行一条指令，其过程如下：

       ```shell
       第一步：获取指令地址——cpu.pc值，床底给decinfo.seq_pc,供后面译码用
       第二步：将获取到的pc传递给isa_exec，执行该pc处的指令。isa_exec(&decinfo.seq_pc) ，每个架构都已经封装好了isa_exec函数。
       第三步：更新pc值，update_pc
       ```

     - isa_exec(&decinfo.seq_pc)会根据pc的值取指令并执行，以riscv32为例，其过程如下：

       ```shell
       第一步：根据pc的值去取指令放入到decinfo.isa.instr.val中
       第二步：检查指令的opcode是否合法
       第三步：对指令的opcode译码，并根据opcode_table[32]和idex定义并执行译码函数和执行函数，然后分别执行译码函数和执行函数。注意，执行函数可能还需要funct3获取进一步的信息。
       
       例如，load指令在`opcode_table`中的定义为IDEX(ld, load), 以此确定load的译码函数为docode_ld,执行函数为exec_load. 
       
       找不到相应的函数？那是因为译码函数由以下宏定义来定义的
       #define make_DHelper(name) void concat(decode_, name) (vaddr_t *pc)
       所以load的译码函数为make_DHelper(ld), 执行函数类似。
       
       执行idex时，如果译码函数不为空，则执行译码函数。为空则不执行，那什么时候为空呢？
       		   接下来会执行执行函数，但是对于同一个opcode可能会有不同的功能，如				load，所以这时候会根据funct3来确定访存的宽度，这时候会再次调用				idex，这时候就不需要译码了，因此译码函数为空。
       ```

     - 那么该如何添加一条新的指令呢？


       - 第一步，确定指令opcode，添加到opcode_table中。例如，lui的在opcode_table中的元素为`IDEX(U, lui)`
       - 第二步，根据第一步添加的信息，确定译码函数和执行函数，如lui的译码函数为`decode_U`, 执行函数为`exec_lui`,这两个分别用`make_DHelper(U)`和`make_EHelper(lui)`定义的。
       - 第三步，在decode.c中编写译码函数，如lui的`make_DHelper(U)`,展开则为`decode_U`.注意，添加了新的译码函数后，还需要再`include/decode.h`中添加声明
       - 第四部，编写执行函数。注意，对于R-type，I-type，S-type， B-type的执行函数还需要根据funct3进一步译码，所以在执行这一系列类型的执行函数时需要进一步定义。如load，store类型指令还需要根据funct3确定宽度。而lui为为U-type类型的指令，所以直接执行其执行函数`exec_lui`即`make_EHelper(lui)`即可。
