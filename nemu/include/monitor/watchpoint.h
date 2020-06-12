#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  
  // struct watchpoint *prev; /* the previous Watchpoint  */

  char EXPR[32];

  int Old_value, New_value;
   
} WP;

bool watchpoint_monitor();
#endif
