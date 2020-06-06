#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {}; /* why use static */
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  wp_pool[0].prev = NULL;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }

  for (i = 1; i< NR_WP; i++)
	wp_pool[i].prev = &wp_pool[i-1];
  
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

/* given the No, if there exit in the watchpoint, return pointer wp
   @param	N	-	No
   @return	WP*
 */

WP* find_wp(int N) {
	int i;
	WP *wp = head;
	
	if (wp == NULL) {
		printf("No watchpoints. \n");
		return false;	
	}
	
	for (i = 0; i < N; i ++) {
		if(wp->NO == N) break;
		else wp = wp->next;
	}

	return wp;
}

WP* new_wp() {
	if (free_ == NULL) { /* there is no free wp */
		assert(0);
	}

	WP* node = free_;
	free_ = free_->next;
	return node;
}

bool free_wp(int N) {
	
	/* find the pointer to N */
	WP *wp = find_wp(N);

	if (wp == NULL) {
		printf("There is no %d watchpoint. \n", N);
		return false;
	}

	/* delete the wp from the list */
	wp->prev->next = wp->next;
	wp->next->prev = wp->prev;

	/* insert to free_ */
	wp->next = free_;
	free_->prev = wp;

	/* move the free_ to wp */

	free_ = wp;
	
	printf("NO %d watchpoint has been deleted\n", N);
	return true; /* delete successfully */
}

void watchpoint_display(int N) {

	WP *wp = find_wp(N);
	if (wp == NULL) {
		printf("No breakpoint number %d\n", N);
		return;
	} 

	printf("Num \t What\n"); // TODO: complete this function 
	printf("%d\t%s\n", wp->NO, wp->what);

}

void watchpoint_all_display() {
	
	WP *wp = head;
	if (wp == NULL) {	
		printf("No watchpoints\n");	
		return;
	} 

	printf("Num \t What\n"); // TODO: complete this function
	
	while (wp) 	
		printf("%d\t%s\n", wp->NO, wp->what);
	
}
