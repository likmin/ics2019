#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

/*
 * inline expansion, or inlining, is a manual or compiler optimization that
 * replaces a function call site with the body of the called function
 * 
 * it's faster than call function
 */
uint32_t choose(uint32_t n) {
	return rand() % n;	
}

static inline void gen_num() {
	char s[4];
	uint32_t n = choose(99);
	
	/* send the formatted data to string */
	sprintf(s, "%u", n);
	strcat(buf, s);		
}

static inline void gen(char str) {
	/* generate random white space */
	uint32_t lSpace = choose(4);
	uint32_t rSpace = choose(4);
	
	char s[lSpace + 1 + rSpace];

	uint32_t i;

	for (i = 0; i < lSpace; i++) s[i] = ' ';
	s[i++] = str;
	for (;i < lSpace + 1 + rSpace; i++) s[i] = ' ';
    s[lSpace + 1 + rSpace] = '\0';	
	strcat(buf, s);

}

static inline void gen_rand_op() {

	switch (choose(4)) {
		case 0: gen('+'); break;
		case 1: gen('-'); break;
		case 2: gen('*'); break;
		case 3: gen('/'); break;
	}	
}

static inline void gen_rand_expr() {
  
  switch(choose(3)) {
	case 0: gen_num(); break;
	case 1: gen('('); gen_rand_expr(); gen(')'); break;
	default:  gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break; 
  
  }

}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"_Bool flag;"
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
	printf("[loop %d] fsn = %d\t ", i, fsn); /* TODO: delete fsn and printf it */
    pclose(fp);

    printf("%u\t %s\n", result, buf);
  
	memset(buf, '\0', 65536);
  }
  return 0;
}
