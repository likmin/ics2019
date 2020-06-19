#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();
uint32_t paddr_read(paddr_t addr, int len);


void watchpoint_all_display();
void watchpoint_display(int N);
bool free_wp(int N);
WP* new_wp(char *EXPR);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  uint64_t n = 1; /* the default value of n is 1 */ 
  if(arg) n = strtoull(arg, NULL, 10);  
  /*
   * strtoull(str, endptr,base)
   * interpret the string to unsigned long long int, which is also named uint64_t
   * 
   */
  cpu_exec(n); 

  return 0;
}

static int cmd_info(char *args) {

  char *arg = strtok(NULL, " ");

  if(strcmp(arg, "r") == 0) {
    isa_reg_display();
  }else if(strcmp(arg, "w") == 0) {
	watchpoint_all_display();
  }
  return 0;
}


static int cmd_p(char *args) {
  bool success;
//  printf("[p expr] args = %s \n",args);
  uint32_t result = expr(args, &success);
  printf("result = 0x%x %u\n", result, result);
  return 0;  
}


static int cmd_x(char *args) {
 char *argN = strtok(NULL, " ");
 char *argEXPR = strtok(NULL, " "); 

 if(argN == NULL || argEXPR == NULL) {
	printf("[x] args error!\n the format of x is the following:\n");
	cmd_help("x"); /* TODO: call the 'help x' */ 
	return 0;
 }

 int N = atoi(argN); 

 bool success;
 uint32_t EXPR = expr(argEXPR, &success);

 if(!success) {
	printf("EXPR = %s is error!\n",argEXPR);
	return 0;
 }

 int i;
 printf("\tpaddr \t data  \t \n");
 for(i = 0; i < N; i++) {
	/*TODO: paddr_read ? vaddr_read? I think it should be vaddr_read! */
	printf(" 0x%08x \t 0x%08x \n", EXPR, vaddr_read(EXPR, 4)); /* len = 4  */
	EXPR += 4;
 }
 
 return 0; 
}

static int cmd_d(char *args) {

	char *argN = strtok(NULL, " ");// get the 'N'
	if(argN == NULL) {
		printf("please input the arg-N\n");
		return 0;
	} 

	
	int N = atoi(argN); /*same to sscanf(argN, "%d", &N)*/
	if (N < 0 || N > 32) {
		printf("%d is to large!, the range of N is 0 to %d\n", N, 32);
		return 0;
	}

   //	printf("%d\n", N);
  
	if(!free_wp(N)) {
		printf("No watchpoints number %d\n", N);	
	}

	return 0;	

}

static int cmd_w(char *args) {
	
	char *EXPR = args; 
	WP *wp = new_wp(EXPR);
	if(wp != NULL) 
		printf("watchpoint %d: %s\n", wp->NO, wp->EXPR);
	else 
		printf("Cannot creat watchpoint: %s\n", EXPR);

	return 0;
	
}
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */

  {"info", "info r: print the state of register; info w: print the infomation of watchpoint", cmd_info },
  {"si", "si n: Pausing the program after n step, the default n = 1",cmd_si},
  {"x", "scan the memory from $EXPR to $EXPR + N", cmd_x},
  {"p", "p EXPR",cmd_p},
  {"d", "d N", cmd_d},
  {"w", "w EXPR, stop the program when the EXPR changed", cmd_w},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;
 
  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);

    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }

    }
    printf("Unknown command '%s'\n", arg);

  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
