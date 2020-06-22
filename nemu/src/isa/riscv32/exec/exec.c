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
  //EXW(lb, 1), EXW(lh, 2), EXW(ld, 4), EMPTY, EXW(lbu, 1), EXW(lhu, 2), EMPTY, EMPTY
  EMPTY, EMPTY, EXW(ld, 4),  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
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
  // EXW(sb, 1), EXW(sh, 2), 
  EMPTY, EMPTY,
  EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
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
 * - stli , 010;
 * - stliu, 011;
 * - xori , 100;
 * - ori  , 110;
 * - andi , 111;
 */

// static OpcodeEntry imm_table [8] = {
//   EXW(addi, 4), EXW(stli, 4), EXW(stliu, 4), EXW(xori, 4), EMPTY, EXW(ori, 4), EMPTY, EXW(andi, 4)
// };

static make_EHelper(imm) { /* static void exec_store(vaddr_t *pc) */
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  /* exec_addi(vaddr_t *pc) */
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}


/*
 * opcode_table
 */
static OpcodeEntry opcode_table [32] = {
  /* Introduction: IDEX is located in 'nemu/include/cpu/exec.h'
	 *		      	          IDEX(ld, load) 
   *                            |   |  
   *                     IDEXW(ld, load, 0) 
   *                            |    |
   *                     {decode_ld, exec_load, 0}
   *
	 *			         this is used to define the decode function, exection function 
	 *  		         and the width of operand.
   */

  /* NOTE:
   *      - 'decode_ld' use to decode I-type riscv instruction, 
   *        so the decode of 'imm' is 'decode_ld', which is located in 
   *        'nemu/src/isa/riscv32/decode.c'
   * 
   *      - 
   */

  
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, EMPTY, IDEX(ld, imm), IDEX(U, auipc), EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, EMPTY, IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ EMPTY, /*IDEX(J, jalr)*/EMPTY, EX(nemu_trap), EMPTY, /*IDEX(J, jal)*/EMPTY, EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4); 
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  
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