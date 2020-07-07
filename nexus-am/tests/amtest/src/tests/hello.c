#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

void hello() {
  char str[20] = "everyone";
  for (int i = 0; i < 10; i ++) {
    //putstr("Hello, AM World @ " __ISA__ "\n");
    printf("Hello, AM World @ " __ISA__ ",%d, %s \n", i, str);
    //printf("hello , %s, %d \n",str, i);
    printf("%s", str);
   // printf("Hello World\n");
  }
}
