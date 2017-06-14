#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"
#include "buttons.h"
#include "circular.h"
#include "malloc.h"
#include "gl.h"
#include "main.h"

char button_read(void);

static int cnt;
static int cnt2;
static cir_t * circ;
static cir_t * circ2;

//Set GPIO20 and 21 to input
void setPins() {
  gpio_set_function(GPIO_PIN20, GPIO_FUNC_INPUT);
  gpio_set_pullup(GPIO_PIN20);
  gpio_set_function(GPIO_PIN21, GPIO_FUNC_INPUT);
  gpio_set_pullup(GPIO_PIN21);
}

//Should not get called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}

//We only enable GPIO interrupts, so just forward it blindly
void interrupt_vector(unsigned pc) {
  int_handler(pc);
}

void int_handler(unsigned pc) {
  if(gpio_check_and_clear_event(GPIO_PIN20)){
      if(handleInterrupts()){
          cnt2++;
          cir_enqueue(circ2, cnt2);
	  }
  }
  if(gpio_check_and_clear_event(GPIO_PIN21)){
      if(handleInterrupts()){
		  cnt++;
    	  cir_enqueue(circ, cnt);
	  }
  }
}

void setup_interrupts() {
    gpio_detect_falling_edge(GPIO_PIN21);
    gpio_detect_rising_edge(GPIO_PIN21);
    gpio_detect_falling_edge(GPIO_PIN20);
    gpio_detect_rising_edge(GPIO_PIN20);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}


//Returns '0' if SET (20) pushed, '1' if ENTER (21) pushed
char button_read(void) {
  char retval = 'x';
  int pin20 = gpio_read(GPIO_PIN20);
  int pin21 = gpio_read(GPIO_PIN21);

  while(pin20 == 1 && pin21 == 1) {
    pin20 = gpio_read(GPIO_PIN20);
    pin21 = gpio_read(GPIO_PIN21);
  }
  if (pin20 != 1) {
    //SET pushed
    retval = '0';
  }
  else if (pin21 != 1) {
    //ENTER pushed
    retval = '1';
  }
  while(gpio_read(GPIO_PIN20) == 0 || gpio_read(GPIO_PIN21) == 0){}
  return retval;
}

int returnButton1() {
  if (!cir_empty(circ)){
    return cir_dequeue(circ);
  }
  return 0;
}
int returnButton2() {
  if (!cir_empty(circ2)){
    return cir_dequeue(circ2);
  }
  return 0;
}
int isEmpty(unsigned int buttonNum){
  if(buttonNum == 1)
    return(cir_empty(circ));
  if(buttonNum == 2)
    return(cir_empty(circ2));
  return 0;
}

void buttons_init(void) {
  printf_init();
  circ = malloc(sizeof(int) * 800);
  circ2 = malloc(sizeof(int) * 800);
  //button_test();
  setPins();
  setup_interrupts();
  /*while (1) {
      if(!cir_empty(circ)){
      printf("%d \n", cir_dequeue(circ));
      }
    // Do nothing, for now.
    //button_test();
  }*/
}
