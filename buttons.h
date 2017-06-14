#ifndef BUTTONS_H
#define BUTTONS_H

void setPin21();
void impossible_vector(unsigned pc);

void interrupt_vector(unsigned pc);
void int_handler(unsigned pc);
void button_test();
char button_read(void);
void setup_interrupts();
void buttons_init(void);
int returnButton1();
int returnButton2();
int isEmpty(unsigned int buttonNum);
#endif
