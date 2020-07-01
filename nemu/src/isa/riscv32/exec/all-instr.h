#include "cpu/exec.h"
/* all support instruction */
/**********RV32I*************/

make_EHelper(inv); /* invalid instruction*/
make_EHelper(nemu_trap); 


/* U-type */
make_EHelper(lui);
make_EHelper(auipc);

/* J-type */
make_EHelper(jal);
make_EHelper(jalr);

/* B-type */
make_EHelper(br);

/* load instruction */
make_EHelper(ld);
make_EHelper(lh_lb);
/* store instruction */
make_EHelper(st);

/* I-type */
make_EHelper(addi);     /* 000 */
make_EHelper(slli);     /* 001 */
make_EHelper(slti);     /* 010 */ 
make_EHelper(sltiu);    /* 011 */
make_EHelper(xori);     /* 100 */
make_EHelper(srli_srai);/* 101 */
make_EHelper(ori);      /* 110 */
make_EHelper(andi);     /* 111 */


/* R-type */
make_EHelper(sub_add);
make_EHelper(sll);     /* 001 */
make_EHelper(slt);     /* 010 */ 
make_EHelper(sltu);    /* 011 */
make_EHelper(xor);     /* 100 */
make_EHelper(srl_sra);/* 101 */
make_EHelper(or);      /* 110 */
make_EHelper(and);     /* 111 */