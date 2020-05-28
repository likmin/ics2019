#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_PLUS,
  TK_EQ,

  /* TODO: Add more token types */
  
  TK_DNUM, TK_HNUM,
  TK_REG,
  TK_NEQ,
  TK_AND,
  TK_POINTER,
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces, one or more space
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal

  /* TODO: why is \\- nor \- ? '\' is Escape String */
 // {"^\\$e*[acdb][x|l|h]|^\\$e*[sb][p|h]|^\\$e*[sd][i|h]", TK_REG}, // regex for x86 register
  {"[0-9]+", TK_DNUM}, // decimal-number
 // {"0[xX][0-9a-fA-F]+",TK_HNUM}, // hexadecimal-number
  
  {"-", '-'},			// sub
  {"\\*", '*'},			// mul
  {"\\/", '/'},			// div
  /*{"\\(", '('},			// 
  {"\\)", ')'},			//
  {"!=", TK_NEQ},		// not equal
  {"&&", TK_AND},		// &&
  */
  /* how to express pointer '*' */
 /* {"^\\*", TK_POINTER},	
  */
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

/* tell the compiler that tokens is useful, there won't be warning even no used it*/
static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {

    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		  case '+':
		  case '-':
		  case '*':
		  case '/':
		/*  case '&':
		  case '(':
		  case ')': 
				//tokens[nr_token].str = rules[i].regex; 
				//tokens[nr_token].type = rules[i].token_type; 
				//nr_token++;
				//break;
		 */
		  case TK_EQ:
	//    case TK_REG:
		  case TK_DNUM:
		  case TK_HNUM:
		//  case TK_NEQ:
		 // case TK_AND:
		 // case TK_POINTER: 
				strncpy(tokens[nr_token].str,substr_start, substr_len); 
				tokens[nr_token].type = rules[i].token_type; 
				nr_token++;
				break;

		  case TK_NOTYPE: continue;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

 *success = true; 
 int i;
 for (i = 0; i < 32; i++) {
	printf("%s %d, ", tokens[i].str, tokens[i].type);	 
	if(i % 8 == 0) printf("\n");
 }
  
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
