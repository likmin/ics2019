#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

void hello() {
  char str[20] = "everyone";
  for (int i = 0; i < 10; i ++) {
    //putstr("Hello, AM World @ " __ISA__ "\n");
    //printf("Hello, AM World @ " __ISA__ ",%d, %s \n", i, str);
   // printf("%d, %s\n", i, str);
    //printf("%s1,%s2\n", str, str);
    //printf("%d, %d \n", i, i+ 1);

    printf("hello , %s, %d \n",str, i);
    printf("\n");
    //  printf("%s, %d\n", str, i);
    
   // printf("Hello World\n");
  }
}
