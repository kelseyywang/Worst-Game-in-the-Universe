#include <stdio.h>
#include "gpio.h"
#include "ps2.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "uart.h"
#include "interrupts.h"
#include "gpioevent.h"
#include "circular.h"

static void setup_interrupts();
void int_handler();
static int cnt = 0;
static int onesCount = 0;
static int scancode = 0;
unsigned int byteArray[16];
static int elementCount = 0;

const unsigned CLK  = GPIO_PIN23;
const unsigned DATA = GPIO_PIN24; 

static int capsLockOn = 0;
static int currentModifier = 0x0;
static int enterState = 0;

static cir_t *cir;

void impossible_vector(unsigned pc){
    printf("impossible expception at pc=%x\n", pc);
}
// Interrupt handler, called on falling
// edge of CLK signal.
void keyboard_vector(int pc){
    if(cnt == 0){
        if(gpio_read(DATA) != 0){
            return;
        }
    }
    
    byteArray[cnt] = gpio_read(DATA);
    if(cnt > 0 && cnt < 9){
        if(gpio_read(DATA)){
            onesCount++;
        }
        scancode |= (byteArray[cnt] << (cnt - 1));
    }
   
    cnt++;
    if(cnt >= 11){
        cnt = 0;
        onesCount = 0;
        if(byteArray[10] == 1 && (byteArray[9] + onesCount) % 2 == 1)
            cir_enqueue(cir, scancode); // Add scancode to circular queue
        elementCount++;
        scancode = 0;
    }
    gpio_check_and_clear_event(CLK); // Clear event so it isn't stuck!
}

void wait_for_falling_clock_edge() {
  while(gpio_read(CLK) == 0) {}
  while(gpio_read(CLK) == 1) {}
}

void keyboard_init(void) {
  gpio_set_function(CLK, GPIO_FUNC_INPUT); 
  gpio_set_pullup(CLK); 
 
  gpio_set_function(DATA, GPIO_FUNC_INPUT); 
  gpio_set_pullup(DATA); 
  cir = cir_new();
  setup_interrupts();
}

// Function setup_interrupts sets up the appropriate
// events on which to call interrupts.
void setup_interrupts(){
    gpio_detect_falling_edge(CLK);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}
// Function keyboard_read_scancode
// dequeues saved values from the circular
// queue and returns them.
int keyboard_read_scancode(void) {
    return cir_dequeue(cir);
}

key_event_t keyboard_read_event(void) {
  key_event_t event;
  
  int scancode = keyboard_read_scancode();
  while(scancode == 0xE0){
    scancode = keyboard_read_scancode();
  }
  event.code = scancode;
  event.down = 1;
  if(scancode == 0xF0){
     event.code = keyboard_read_scancode();
     event.down = 0;
  }
  return event;
}

int isModifier(unsigned char code){
    if(code == KEY_LEFT_SHIFT || code == KEY_RIGHT_SHIFT || code == KEY_LEFT_ALT || code == KEY_LEFT_CONTROL || code == KEY_CAPS){
        return 1;
    }
    return 0;
}

int convertModifier(unsigned char code){
    switch(code){
        case KEY_LEFT_SHIFT:
            return KEY_MOD_SHIFT;
        case KEY_RIGHT_SHIFT:
            return KEY_MOD_SHIFT;
        case KEY_LEFT_ALT:
            return KEY_MOD_ALT;
        case KEY_LEFT_CONTROL:
            return KEY_MOD_CTRL;
        case KEY_CAPS:
            return KEY_MOD_CAPS;
        default:
            break;
    }
    return 0x0;
}

key_press_t keyboard_read_press(void) {
  key_press_t press;
  press.modifiers = currentModifier;
  key_event_t event = keyboard_read_event();
  unsigned char firstCode = event.code;
  press.code = firstCode;
  while(1){
      if(!isModifier(event.code) && event.down == 1){
        press.modifiers = currentModifier;
        press.code = event.code;
        return press;
      }
      if(isModifier(event.code) && event.down == 1){
        currentModifier = currentModifier | convertModifier(event.code);
        press.modifiers = press.modifiers | currentModifier;
        if(event.code == KEY_CAPS){
            if(capsLockOn == 0){
                capsLockOn = 1;
            }
            else if(capsLockOn == 2){
                capsLockOn = -1;
            }
        }
      }
      if(event.down == 0 && isModifier(event.code)){
        currentModifier = currentModifier & ~(convertModifier(event.code));
        if(event.code == KEY_CAPS){
            if(capsLockOn == 1){
                capsLockOn = 2;
            }
            else if(capsLockOn == -1){
                capsLockOn = 0;
            }
        }
      }
      event = keyboard_read_event();
  }
  return press;
}

char keyboard_read_char(void) {
  key_press_t press;
  if(enterState){
    enterState = 0;
    return '\n';
  }
  while(1){
    press = keyboard_read_press();
    if(press.code == 0x72){ //down key
        return 17; // device ctrl 1
    }
    if(press.code == 0x75){ //up key
        return 18; //device ctrl 2
    }
    if(is_char[press.code]){
        if(press.modifiers == KEY_MOD_SHIFT){
            return shift_char_from_scan[press.code];
        }
        else if(capsLockOn > 0 && char_from_scan[press.code] >= 'a' && char_from_scan[press.code] <= 'z'){
            return shift_char_from_scan[press.code];
        }
        else{
            if(press.code == KEY_ENTER){
                enterState = 1;
                    return '\r';
            }
            return char_from_scan[press.code];
         }
    }
  }
  return '!';
}

