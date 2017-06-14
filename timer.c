#include "timer.h"
#define TIMER_CLO ((unsigned int*)0x20003004)

/*
 * initialize the timer - does not do anything
 */
void timer_init(void) {
}

/*
 * Returns the current system time in us.
 *
 * @return system time in microseconds
 */
unsigned int timer_get_time(void) {
 return *TIMER_CLO; // access clock register and return time!
}

/*
 * A simple busy loop that delays the program for `n` microseconds.
 *
 * @param n the number of microseconds to busy loop for
 */
void delay_us(unsigned int n) {
  volatile unsigned int start = timer_get_time();
  while ((volatile unsigned int)timer_get_time() - start < n) { /* spin */ }
  // must be volatile so compiler knows it can be modified externally
}
void delay_ms(unsigned int n){
	unsigned int i;
	for(i = 0; i < n; i++){
		delay_us(1000);
	}
}

/*
 * Delay for `n` seconds.
 *
 * @param n the number of seconds to busy loop for
 */
void delay(unsigned int n) {
  unsigned int i;
  for (i = 0; i < n; i++) {
    delay_us(1000000);
  }
}
