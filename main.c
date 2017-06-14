//#include "keyboard.h"
#include "console.h"
#include "shell.h"
#include "interrupts.h"
#include "buttons.h"
#include "gpio.h"
#include "gl.h"

#define NROWS 20
#define NCOLS 40

void level1();

int x1 = 0;
int y1 = 50;
int x2 = 0;
int y2 = 400;

void main(void) {
  buttons_init();
  gl_init(1280, 720, 1);
  gl_draw_string(70, 10, "WELCOME TO THE WORST GAME IN THE UNIVERSE! BROUGHT TO YOU BY 107E AND TOO MUCH TIME.", GL_WHITE);
  gl_swap_buffer();
  gl_draw_string(70, 10, "WELCOME TO THE WORST GAME IN THE UNIVERSE! BROUGHT TO YOU BY 107E AND TOO MUCH TIME.", GL_WHITE);
  level1();
}

void level1(){
  while(1){
    //gl_clear(GL_BLACK);
    x1 = returnButton1() / 2;
    x2 = returnButton2() / 2;
    /*for(int i = 0; i < 10; i++){
      returnButton1();
      returnButton2();
    }*/
    if (x1 >= 1230 || x2 >= 1230) {
      break;
    }
    gl_draw_rect(x1, y1, 50, 300, GL_BLUE);
    gl_draw_rect(x2, y2, 50, 300, GL_AMBER);
    gl_swap_buffer();
  }
  if (x1 > x2) {
    gl_draw_string(500, 370, "GAME OVER - BLUE WON!", GL_WHITE);
  }
  else {
    gl_draw_string(500, 370, "GAME OVER - ORANGE WON!", GL_WHITE);
  }
  gl_swap_buffer();
}
