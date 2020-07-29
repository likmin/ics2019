#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

/*_start(...) 用户程序的入口*/
void _start(int argc, char *argv[], char *envp[]) {
  char *env[] = {NULL};
  environ = env;
  exit(main(argc, argv, env)); /* 从main() 函数返回后会调exit()结束运行 */ 
  assert(0);
}
