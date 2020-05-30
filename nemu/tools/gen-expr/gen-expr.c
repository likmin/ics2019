#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

char *getnum() {
	srand(time(NULL));
	uint32_t num = 	rand();
	char *Num;
	utoa(num, Num);
	return Num;
}
// this should be enough
static char buf[65536];
static inline void gen_rand_expr() {
  
  switch (rand() % 3) {
	case 0: getnum();
	case 1: 
	default:  
  
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"bool flag;"
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int fsn = fscanf(fp, "%d", &result);
    printf("fsn = %d\n", fsn);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
