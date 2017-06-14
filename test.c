#include "keyboard.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"

/* 
 * This simple program tests the behavior of the asisgn5 keyboard
 * implementation versus the new-improved assign7 version. If using the
 * assign5 implementation, a key press that arrives while the main program
 * is waiting in delay is simply dropped. Once you upgrade your
 * keyboard implementation to be interrupt-driven, those keys should 
 * be queued up and can be read after delay finishes.
 */
void main(void) 
{
    keyboard_init();
    printf_init();
    
    while (1) {
        printf("Test program waiting for you to press a key (q to quit): ");
        uart_flush();
        char ch = keyboard_read_char();
        printf("\nRead: %c\n", ch);
        if (ch == 'q') break;
        printf("Test program will now pause for 1 second... ");
        uart_flush();
        delay(1);
        printf("done.\n");
    }
    printf("\nGoodbye!\n");
}
