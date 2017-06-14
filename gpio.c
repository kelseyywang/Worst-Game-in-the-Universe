#include "gpio.h"

/*
 * initializes gpio - does not need to anything
 */
void gpio_init(void) {
}

/*
 * Set GPIO pin number `pin` to the GPIO function `function`. Does not affect
 * other pins.
 *
 * @param pin the pin number to initialize
 * @param function the GPIO function to set for the pin
 */
// Sets pin to specified function
void gpio_set_function(unsigned int pin, unsigned int function) {
  if (pin > GPIO_MAX || function >= GPIO_FUNC_INVALID) {
    return;
  }
  // Addresses increment by 4, allowing us to do the following
  unsigned int* FSELaddress = (unsigned int*) (0x20200000 + (pin / 10) * 4);
  // Must keep track of which registers are 1, preventing overriding
  int shiftAmount = 3 * (pin % 10);
  unsigned int allOthersOnes = ~(7 << shiftAmount);
  *FSELaddress = *FSELaddress & allOthersOnes;
  *FSELaddress = *FSELaddress | (function << shiftAmount);
}

// Sets pin to input
void gpio_set_input(unsigned int pin) {
  gpio_set_function(pin, 0);
}

// Sets pin to output
void gpio_set_output(unsigned int pin) {
  gpio_set_function(pin, 1);
}

/*
 * Get the function for GPIO pin number `pin`. Should not affect
 * any registers.
 *
 * @param pin the pin number to initialize
 */
 // Returns what pin's function (usually 0 or 1 for input/output)
unsigned int gpio_get_function(unsigned int pin) {
  if (pin > GPIO_MAX) {
    return 0;
  }
  int shiftAmount = 3 * (pin % 10);
  unsigned int allOthersZeroes = 7 << shiftAmount;
  unsigned int* FSELaddress = (unsigned int*) (0x20200000 + (pin / 10) * 4);
  *FSELaddress = *FSELaddress & allOthersZeroes;
  return (*FSELaddress >> shiftAmount);
}

/*
 * Sets GPIO pin number `pin` to the value `value`. The GPIO pin should be set
 * the output mode for this call to function correctly.
 *
 * @param pin the pin number to set or clear
 * @param value 1 if the pin should be set, 0 otherwise
 */
 // Writes pin to specified value
void gpio_write(unsigned int pin, unsigned int value) {
  if (value == 1) {
    // Depending on the pin and the register, writes to the appropriate
    // set register
    if (pin < 32) {
    *(unsigned int *) 0x2020001C = 1 << pin;
    }
    else {
    *(unsigned int *) 0x20200020 = 1 << (pin % 32);
    }
  }
  else {
    // Writes to the appropriate clear register
    if (pin < 32) {
      *(unsigned int *) 0x20200028 = 1 << pin;
    }
    else {
      *(unsigned int *) 0x2020002C = 1 << (pin % 32);
    }
  }
}

/*
 * Read GPIO pin number `pin`.
 *
 * @param pin the pin number to be read
 */
 // Reads appropriate level register for pin
unsigned int gpio_read(unsigned int pin) {
  unsigned int ones = 1 << (pin % 32);
  if (pin < 32) {
    return ((*((unsigned int *) 0x20200034) & ones) >> pin);
  }
  else {
    return ((*((unsigned int *) 0x20200038) & ones) >> (pin % 32));
  }
}
