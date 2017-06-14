#include "shell.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "malloc.h"
#include "console.h"
#include "buttons.h"

int (*shell_printf)(const char * format, ...);
void setPills(int *dispenseArray, int pillFreq);
char button_read(void);
void setPillAmt(int *pill1Amt);
void runMotor(int * dispenseArray, int numWeeks);

//Initializes components
int shell_init(int graphical) {
  if (graphical == 1) {
    printf_init();
    shell_printf = console_printf;
  }
  else {
    printf_init();
    shell_printf = printf;
  }
  return 0;
}

//Handles running of shell. Prompts user for number of
//medication times per day, number of pills per medication
//time, and fills in array of dispense amounts
void shell_run() {
  shell_printf("Welcome to PillPal!\nNeed PILLZ? We yo PAL!\n\n");
  int pillFreq = 1;
  shell_printf("How often do you take your medications during the day? (press SET to change, ENTER to submit)\n");
  shell_printf("1 time(s) per day.");
  while (1) {
    char currCharInLine = button_read();
    if (currCharInLine == '0') {
      pillFreq = pillFreq % 3;
      //Make next option be on same line... jank level 99999. No ragrets laugh all you want ;)
      shell_printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%d time(s) per day.", pillFreq + 1);
      pillFreq = pillFreq + 1;
    }
    else if (currCharInLine == '1') {
      shell_printf("\nFrequency set to %d medication time(s) per day.\n\n", pillFreq);
      break;
    }
    else {
      //Read pin we haven't configured... should not happen
      shell_printf("Error: read invalid code %c\n", currCharInLine);
    }
  }
  shell_printf("Set the following medications for each day (up to 5 pills)\nPress SET to increase, ENTER to submit:\n");
  int dispenseArray[21];
  setPills(dispenseArray, pillFreq);

  runMotor(dispenseArray, 4); //dispense for 4 weeks.
  shell_printf("\n\nPillPal out.");
//  stop_motor();
}

void runMotor(int *dispenseArray, int numWeeks) {
  for (int wks = 0; wks < numWeeks; wks++) {
    for (int i = 0; i < 21; i++) {
      delay(2);
    }
  }
}

//Helper function to handle filling in dispenseArray
//with amounts of pills to dispense at any given time.
void setPills(int *dispenseArray, int pillFreq) {
  int *pill1Amt = malloc(sizeof(int));
  for (int i = 0; i < 7; i++) {
    shell_printf("\nMedication for Day %d, Meal 1:\n", i + 1);
    *pill1Amt = 0;
    setPillAmt(pill1Amt);
    dispenseArray[i * 3] = *pill1Amt;
    if (pillFreq >= 2) {
      shell_printf("\nMedication for Day %d, Meal 2:\n", i + 1);
      *pill1Amt = 0;
      setPillAmt(pill1Amt);
      if (pillFreq == 2) {
        //2 medication times/day, dispense only at Meal 1 and Meal 3
        dispenseArray[i * 3 + 1] = 0;
        dispenseArray[i * 3 + 2] = *pill1Amt;
      }
      else {
        //Dispense at Meals 1, 2, and 3
        dispenseArray[i * 3 + 1] = *pill1Amt;
        shell_printf("\nMedication for Day %d, Meal 3:\n", i + 1);
        *pill1Amt = 0;
        setPillAmt(pill1Amt);
        dispenseArray[i * 3 + 2] = *pill1Amt;
      }
    }
    else {
      //Only 1 medication time/day, dispense only at Meal 1
      dispenseArray[i * 3 + 1] = 0;
      dispenseArray[i * 3 + 2] = 0;
    }
  }
}

//Helper function to set pill amount for one meal
void setPillAmt(int *pill1Amt) {
  shell_printf("0 pill(s)");
  while (1) {
    char currCharInLine = button_read();
    if (currCharInLine == '0') {
      *pill1Amt = *pill1Amt % 5;
      (*pill1Amt)++;
      //Same jankiness as above ;)
      shell_printf("\b\b\b\b\b\b\b\b\b%d pill(s)", *pill1Amt);
    }
    else if (currCharInLine == '1') {
      break;
    }
    else {
      shell_printf("Error: %c ", currCharInLine);
    }
  }
}
