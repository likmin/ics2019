#include "cpu/exec.h"
/* all support instruction */
make_EHelper(lui);

make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(auipc);
make_EHelper(addi);

make_EHelper(jal);