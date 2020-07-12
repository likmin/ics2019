#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
/* 定义了HAS_CTE后
 * init_irq() -> _cte_init(): 
 *  1.设置异常入口地址。
 *  2.注册一个时间处理回调函数 
 */
#define HAS_CTE  

//#define HAS_VME

#include <am.h>
#include <klib.h>
#include "debug.h"

typedef char bool;
#define true 1
#define false 0

#endif
