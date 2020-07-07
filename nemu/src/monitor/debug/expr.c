#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include <stdlib.h>

uint32_t isa_reg_str2val(const char *s,bool *success);
uint32_t paddr_read(paddr_t addr, int len);

enum {
  TK_NOTYPE = 256,
  TK_PLUS,
  TK_EQ,

  /* TODO: Add more token types */
  
  TK_DNUM, TK_HNUM,
  TK_REG,
  TK_NEQ,
  TK_AND,
  TK_DEREF,
  TK_NEG,
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
  {REG_REGEX , TK_REG}, // regex for x86 register

  {"\\b[0-9]+\\b", TK_DNUM}, // decimal-number
  {"0[xX][0-9a-fA-F]+", TK_HNUM}, // hexadecimal-number
  
  {"-", '-'},			// sub
  {"\\*", '*'},			// mul
  {"\\/", '/'},			// div
  {"\\(", '('},			// 
  {"\\)", ')'},			//
  {"!=", TK_NEQ},		// not equal
  {"&&", TK_AND},		// &&
  
  /* how to express pointer '*' */
  // {"\\*", TK_POINTER}, /* TODO: try to implement it!: try to predecode it before eval it.*/	
  
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
  /* begin to make token, you should flush the tokens, or next time if may going wrong */
  int j;
  for (j = 0;j < NR_REGEX; j ++) {
	  memset(tokens[j].str, '\0', 32);
	  tokens[j].type = 0; 
  }

  while (e[position] != '\0') {

    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
		/*
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

		if (rules[i].token_type == TK_NOTYPE) break;
        switch (rules[i].token_type) {
		  case TK_DNUM:
		  case TK_HNUM:
				if(substr_len > 32) {
					printf("ERROR! the length of num in this expr is longer than 32bit\n");
					assert(0);
						
				}

		  case '+':
		  case '-':
		  case '*':
		  case '/':
		  case '(':
		  case ')': 
		  
		  case TK_EQ:
	      case TK_REG:
		  case TK_NEQ:
		  case TK_AND:
//		  case TK_POINTER: /* TODO: inplement it*/
				strncpy(tokens[nr_token].str,substr_start, substr_len); 
				tokens[nr_token].type = rules[i].token_type; 
				nr_token++;
				break;

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
bool check_parenteses(uint32_t p, uint32_t q) {
	int flag = 0;

	if (strcmp(tokens[p].str,"(") || strcmp(tokens[q].str,")")) return false;	
	p++;
	while(p < q) { /* left and right have checked*/
		//printf("[check_parenteses] p = %u, q = %u,flag = %d\n", p,q,flag);
		if(strcmp(tokens[p].str,"(") == 0) {
			p++;
			flag++;	
		} else if(strcmp(tokens[p].str,")") == 0) {
			p++;
			flag--;
			if(flag < 0) return false;
		} else p++;
	//	printf("[check_parenteses] p = %u, q = %u,flag = %d\n", p,q,flag);
	}	

	return flag==0;
}

uint32_t getOpPosition(uint32_t p, uint32_t q) {
	

	assert(p <= q);
	/*
	 * TODO: you need to check the parenteses, or you will get the wrong position.
	 *		 for example, (1 + 2) * 3. if you don't check the parenteses, you will
	 *       get the '+' position,nor the '*' position.
	 */
	uint32_t i = p;
	uint32_t op = p;
	int pnum = 0;

	for (i = p; i < q; i++) 
	{
	//	printf("[getOpPosition] i = %u, tokens[%u].str = %s, pnum = %d\n", i,i,tokens[i].str,pnum);							
		if(strcmp(tokens[i].str, "(") == 0) pnum++;
	    if(strcmp(tokens[i].str, ")") == 0) pnum--;
		
		
		if (pnum == 0)
			if (strcmp(tokens[i].str, "+") == 0 || strcmp(tokens[i].str, "-") == 0 ||
				strcmp(tokens[i].str, "*") == 0 || strcmp(tokens[i].str, "/") == 0 ||
		        tokens[i].type == TK_DEREF|| /* pointer */ 
				tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ ||  tokens[i].type == TK_AND) 
			{
				op = i; 
				break;
			}
		
	}
	return op;
		
}

/*
 * p is start position of the sub-expression
 * q is end position of the sub-expression
 */

uint32_t eval(uint32_t p, uint32_t q) {
	


	//printf("[eval]p = %u, q = %u\n", p, q);	

	if (p > q) {
	
		/* Bad expression */
		printf("Bad expression!");
		assert(0);

	}
	 if (p == q) {
		
		/* Single token.
		 * For now this token should be a number.
		 * Return the value of the number.
		 */

		 uint32_t value = 0;
		 if (tokens[p].type == TK_DNUM)
			 value = strtoul(tokens[p].str, NULL, 10);
		 
		 if (tokens[p].type == TK_HNUM) 
			 value = strtoul(tokens[p].str, NULL, 16);
		 
		 if (tokens[p].type == TK_REG) {
			int i;
			for (i = 0; i< strlen(tokens[p].str); i++) tokens[p].str[i] = tokens[p].str[i+1];
			bool success = true;
		    value = isa_reg_str2val(tokens[p].str, &success);
		 }
		
		 //if (tokens[p].type == TK_DEREF)
			 
		 return value;

	} else if (check_parenteses(p, q) == true) {
		printf("check parenteses is true, p = %u, q = %u\n", p, q);	
		return eval(p + 1, q - 1);
	
	} else {
	
		int op = getOpPosition(p, q); /* get the position of the op in the subexpression */
		
	//	printf("get op position = %u, op = %s\n",op, tokens[op].str);
		
		uint32_t val1 = 0;
		if(tokens[op].type != TK_DEREF &&  /* if the op is not a pointer, then calculate the value of the left expression */
		   tokens[op].type != TK_NEG)	   /* if the op is not a negative */
			val1 = eval(p, op - 1);
		
		uint32_t val2 = eval(op + 1, q);
		
	//	printf("[eval debug]op = %s, val1 = %u, val2 = %u\n", tokens[op].str, val1, val2);
		
		switch (tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
		 	case '*': return val1 * val2;
		 	case '/': return val1 / val2; /* TODO: what about val2 is zero? */
			case TK_DEREF:	return paddr_read(val2,4);
			case TK_EQ:		return val1 == val2 ? 1 : 0;
			case TK_NEQ:    return val1 != val2 ? 1 : 0;
			case TK_AND:	return val1 && val2 ? 1 : 0;
			case TK_NEG:	return -val2;
			default: assert(0);
				
		}
		
	}
	
	return 0;
}

bool certain_type_for_deref(int type) {
	
	switch(type) {
		case '+':
		case '-':
		case '*':
		case '/': return true; 
		default : return false;
	}
}

bool certain_type_for_neg(int type) {
	switch(type) {
		case '+':
		case '-':
		case '*':
		case '/': 
		case '(': return true; 
		default : return false;
	}
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

 *success = true; 
 int i;
 
  
  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  /* find the DEREF */
  for (i = 0; i < nr_token; i++) {
	if (tokens[i].type == '*' && (i == 0 || certain_type_for_deref(tokens[i-1].type))) {
		tokens[i].type = TK_DEREF; 
	}
	
	if (tokens[i].type == '-' && (i == 0 || certain_type_for_neg(tokens[i-1].type))) {
		tokens[i].type = TK_NEG; 
	}	  
  }
 /* print the tokens  
  for (i = 0; i < 32; i++) {
	printf("%s %d, ", tokens[i].str, tokens[i].type);	 
	if((i + 1) % 8 == 0) printf("\n");
  }
  */

  uint32_t result = eval(0, nr_token-1);
//  printf("result = %u\n", result);
  return result;
}
