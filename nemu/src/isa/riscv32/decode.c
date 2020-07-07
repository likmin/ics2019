#include "cpu/decode.h"
#include "rtl/rtl.h"

/*
 * 利用c语言中的位操作实现符号位拓展
 */
static inline void sext(rtlreg_t *temp, uint32_t width) {
  int32_t num = *temp;
  assert(width >=0 && width <= 32);
  num = (num) << width;
  num = (num) >> width;
  *temp = num;
}
/*
 * 这能是一个好想法，但是不适合于这里，因为并不是所有的符号拓展，都是对操作数。
 * 例如，分支指令Branch。
 */
// #define make_ExtHelper(name) void concat(name, ext) (Operand *op, uint32_t val, uint32_t width) 

// static inline make_ExtHelper(s) { /* static inline void sext(Operand *op, uint32_t val, uint32_t width) */
//   assert(width >=0 && width <= 32);
//   op->simm = val;
//   op->simm = (op->simm) << width;
//   op->simm = (op->simm) >> width;
// }

// static inline make_ExtHelper(u) { /* static inline void uext(Operand *op, uint32_t val, uint32_t width) */
//   assert(width >= 0 && width <= 32);
//   op->type = OP_TYPE_IMM;
//   op->imm = val;
//   op->imm = (op->imm) << width;
//   op->imm = (op->imm) >> width;
//   rtl_li(&op->val, op->imm);
//   print_Dop(op->val, OP_STR_SIZE, "%d", op->imm);
// }


// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (Operand *op, uint32_t val, bool load_val)

static inline make_DopHelper(i) { /* static inline void decode_op_i (...) */
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) { /* static inline void decode_op_r (...)  */
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    /* read the reg value from reg_l to 'op->val'
     * rtl_lr(&op->val, op->reg, 4);
     *
     * if r != 0  rtl_mv(dest, &reg_l[r]) -> rtl_li(dest, imm) -> *dest = imm
     * else       rtl_li(dest, 0) -> *dest = 0
     *      
     */
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}


static inline make_DopHelper(m) {
  op->type = OP_TYPE_MEM; /*TODO: It maybe not right!*/
  op->addr = val;

  rtl_li(&op->val, op->addr);

  print_Dop(op->str, OP_STR_SIZE, "0x%x", op->addr);
}

/*
 * There are six types of instruction in RISCV：
 *  - R-type
 *  - I-type, for example 'ld'
 *  - S-type, for example 'st'
 *  - B-type
 *  - U-type, for example 'lui'
 *  - J-type
 * The define of these type can be found in 'nemu/src/isa/$ISA/include/isa/decode.h'
 * 
 */

make_DHelper(R) { /* void decode_R (vaddr_t *pc), use to decode R-type instruction  */
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(ld) { /* void decode_ld (vaddr_t *pc), use to decode I-type instruction */
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);/* TODO: 我认为这里有问题，应该是符号拓展，但是并没有 */

  /*
   * 操作数中存在str的意义在于，执行一条指令都会将该指令的汇编形式打印出来，根据操作数的个数，
   * 打印指令的形式分为print_asm_template1,print_asm_template2,print_asm_template3.
   * 
   * 为什么在decode_op_r和decode_op_i已经调用了print_Dop，load译码这里还要调用print_Dop?
   * 答：打印load指令会调用print_asm_template2,同时load的指令格式为
   *     lw rd,offset(rs1)
   * 而id_src->str只打印了rs1,所以需要加上offset，所以需要更换为offset(rs1)
   */
  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(st) { /*void decode_st (vaddr_t *pc), use to decode S-type instruction*/
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  int32_t simm = (decinfo.isa.instr.simm11_5 << 5) | decinfo.isa.instr.imm4_0;

  decode_op_i(id_src2, simm, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}

make_DHelper(B) { /* void decode_B (vaddr_t *pc), use to decode B-type instruction  */
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  t0 = (decinfo.isa.instr.simm12 << 12) | (decinfo.isa.instr.imm11 << 11) | 
       (decinfo.isa.instr.imm10_5 << 5) | (decinfo.isa.instr.imm4_1 << 1);
 
  sext(&t0, 19);
  
  rtl_add(&decinfo.jmp_pc, &t0, &cpu.pc);
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", t0);
}

/*
 * TODO: what does the 'print_Dop' mean?
 *       print_Dop() -> snprintf()
 */
make_DHelper(U) { /* void decode_U (vaddr_t *pc), use to decode U-type instruction  */
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}


make_DHelper(J) { /* void decode_J (vaddr_t *pc), use to decode J-type instruction  */

  /*
   * The 'rtl_sext' method located in the 'neum/rtl/rtl.h' may be not apply in here,
   * for the width of simm is 20bit, which is not a multiple of 8.
   */
  t0 =  (decinfo.isa.instr.simm20 << 20) | (decinfo.isa.instr.imm19_12 << 12) | (decinfo.isa.instr.imm11_ << 11) | (decinfo.isa.instr.imm10_1 << 1);   
  
  sext(&t0, 11);
  //simm = simm << 11;
  //simm = simm >> 11;
  //printf("[decode_J] jmp_pc = 0x%x\n", simm+cpu.pc);
  //printf("[decode_J] simm = 0x%x, pc = 0x%x\n",simm, cpu.pc); 
  decode_op_i(id_src , t0, true);
 // decode_op_r(id_src2, *pc , true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
  
  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", t0);
}




make_DHelper(I) {
  int32_t simm = decinfo.isa.instr.simm11_0;
  simm = simm << 20;
  simm = simm >> 20;
  //printf("[decode_Jalr] pc   = 0x%8x\n", decinfo.isa.instr.val);
  //printf("[decode_Jalr] simm = 0x%8x\n", decinfo.isa.instr.simm11_0);
  //printf("[decode_Jalr] rs1  = 0x%8x\n", decinfo.isa.instr.rs1);
  
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, simm, true);

  //printf("[decode_Jalr] src1 = 0x%8x\n", id_src->val);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.rs1);
  print_Dop(id_src2->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.simm11_0);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}