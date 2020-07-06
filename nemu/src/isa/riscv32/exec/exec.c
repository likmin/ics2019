#include "cpu/exec.h"
#include "all-instr.h"

/* There are a lot of tables:
 *  - Opcode_table[32], use to decode opcode, however funct3 and funct7 may be also need to decode, So there are some sub-table.
 *    - load_table[8], use to decode funct3
 *    - store_table[8], use to decode 
 *  
 *
 */

/* 
 * load_table,
 * instr funct3
 *  lb    000
 *  lh    001
 *  lw    010
 *   -     -
 *  lbu   100
 *  lhu   101
 * 
 */
static OpcodeEntry load_table [8] = {
  /*     EXW(   ld,       4)
   *             |        |
   *    {NULL, exec_ld,   w}
   *      |       |       |
   *  {decode, execute, width}
   *    
   */
  EXW(lh_lb, 1), EXW(lh_lb, 2), EXW(ld, 4), EMPTY, EXW(ld, 1), EXW(ld, 2), EMPTY, EMPTY

  //EMPTY, EMPTY, EXW(ld, 4),  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(load) { /* static void exec_load (vaddr_t *pc)*/
 
 /*
  * get the load width:
  *  1 is for lb,lbu; 
  *  2 is for lh,lhu;
  *  and 4 is for lw. 
  */

  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
 
  /*
   * idex use to instruction decode and execute.
   * 
   * static inline void idex(vaddr_t *pc, OpcodeEntry *e) {
   *   if (e->decode) // there is no need to decode when instruction is load or store.
   *     e->decode(pc);
   *   e->execute(pc);// exec_ld(vaddr_t *pc), located in 'ldst.c'
   * }
   * 
   */
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

/*
 * store_table
 * instr  funct3
 *  sb    000
 *  sh    001
 *  sw    010
 */
static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) { /* static void exec_store(vaddr_t *pc) */
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;

  /* exec_st(vaddr_t *pc) */
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

/*
 * imm_table, use to decode the following instruction:
 *   instr funct3
 * - addi , 000;
 * - slli , 001;
 * - stli , 010;
 * - stliu, 011;
 * - xori , 100;
 * - srli,srai, 101;
 * - ori  , 110;
 * - andi , 111;
 */

static OpcodeEntry imm_table [8] = {
  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(srli_srai), EX(ori), EX(andi)
};

static make_EHelper(imm) { /* static void exec_imm(vaddr_t *pc) */
  decinfo.width = imm_table[decinfo.isa.instr.funct3].width;
  idex(pc, &imm_table[decinfo.isa.instr.funct3]);
}


static OpcodeEntry r_table [8] = {
   EX(sub_add), EX(sll), EX(slt), EX(sltu), EX(xor), EX(srl_sra), EX(or), EX(and)
};

static make_EHelper(r) { /* static void exec_r(vaddr_t *pc) */
  decinfo.width = r_table[decinfo.isa.instr.funct3].width;
  idex(pc, &r_table[decinfo.isa.instr.funct3]);
}

/*
 * opcode_table, 译码查找表
 */
static OpcodeEntry opcode_table [32] = {
  /* Introduction: IDEX is located in 'nemu/include/cpu/exec.h'
	 *		      	          IDEX(ld, load) 
   *                            |   |  
   *                     IDEXW(ld, load, 0) 
   *                            |    |
   *        OpcodeEntry  {decode_ld, exec_load, 0}
   *
	 *			         this is used to define the decode function, exection function 
	 *  		         and the width of operand.
   */

  /* NOTE:
   *      - 'decode_ld' use to decode I-type riscv instruction, 
   *        so the decode of 'imm' is 'decode_ld', which is located in 
   *        'nemu/src/isa/riscv32/decode.c'
   * 
   *       
   */

  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, imm), IDEX(U, auipc), EMPTY,EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, r), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, br), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal),/* IDEX(I, system)*/ EMPTY, EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  //printf("[isa_exec] instr = 0x%08x\n", decinfo.isa.instr.val);
  if(decinfo.isa.instr.opcode1_0 != 0x3) printf("0x%08x\n", *pc);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  
  //printf("[isa_exec] opcode6_2 = %d\n", decinfo.isa.instr.opcode6_2);
  //printf("[isa_exec] pc = 0x%x\n", cpu.pc);
  /* Instruction decode and execute */
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}

/* 
 * the process of instruction execute:
 * for example, lw
 * 1.isa_exec: 
 *    1.1 fetch instr to decinfo.isa.instr.val
 *    1.2 idex, call the function 'decode_ld' to decode the instr;
 *              call the function 'exec_load' to execute the instr
 *      1.2.1 'decode_ld' try to take out the oprand
 *      1.2.2 In 'exec_load', idex, try to decode funct3, but load only need to give the width
 *        1.2.2.1 according to the load_table, there is no need to decode
 *        1.2.2.2 according to the load_table, try to execute 'exec_ld' which is located in 'ldst.c' to execute 'ld'.
 */
