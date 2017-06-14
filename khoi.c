#include timer.h
#include gpio.h

#define MOTOR_FORWARD 1
// test commit

void pulse(void){
  while(1){
    gpio_write(GPIO_PIN18, 1);
    delay_ms(1);
    gpio_write(GPIO_PIN18, 0);
    delay_ms(19);
  }
}

void spinForward(){
    driveMotor(MOTOR_FORWARD, 1);
}

void main(void){
  gpio_init();
  timer_init();
  gpio_set_output(GPIO_PIN18);
  pulse();
}
