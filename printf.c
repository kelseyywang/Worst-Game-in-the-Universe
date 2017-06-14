#include <stdarg.h>
#include "printf.h"
#include "uart.h"
#include "assert.h"

int unsigned_to_base(char *buf, int n, unsigned int val, int base, int width);
void convertWidth(char *buf, int arraySize, int width);
void copyStringReverse(char *str, char *destination, int strSize, int destSize);
int signed_to_base(char *buf, int n, int val, int base, int width);
int strlen(const char *str);
void strcpy(char *dst, const char *src);
void strcpyIndex(char *dst, const char *src, int start);
int snprintf(char *s, int n, const char *format, ...);
int vsnprintf(char *s, int n, const char *format, va_list args);
void getExpression(char *retval, va_list list, char *allArgs, int numArgs, int width);
int charToInt(char *arr);
int printf(const char *format, ...);

const char char_ints[16] = {'0', '1', '2', '3', '4', '5',
'6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
void printf_init() {
  uart_init();
}

// Converts to base, calls helper functions to
// pad char array, reverse, and truncate
int unsigned_to_base(char *buf, int n, unsigned int val, int base, int width) {
  int quotient;
  int remaining;
  char temp[1024];
  int currentNumber = val;
  int index = 0;
  // Converting to base
  while (currentNumber > 0) {
    quotient = currentNumber / base;
    remaining = currentNumber % base;
    temp[index] = char_ints[remaining];
    currentNumber = quotient;
    index++;
  }
  //Padding arrays that are too small
  if (index < width) {
    convertWidth(temp, index, width);
    index = width;
    copyStringReverse(temp, buf, index, n);
  }
  else {
    // Don't need to change width
    copyStringReverse(temp, buf, index, n);
  }
  return index;
}

//Pads too-small arrays with zeroes in the end
//(later reversed by copyStringReverse
void convertWidth(char *buf, int arraySize, int width) {
    for (int i = 0; i < width - arraySize; i++) {
      buf[arraySize + i] = '0';
    }
}

//Copies char array to destination backwards and
//within bounds
void copyStringReverse(char *str, char *destination, int strSize, int destSize) {
  int index = 0;
  while (index < strSize && index < destSize - 1) {
    destination[index] = str[strSize - index - 1];
    index++;
  }
  destination[index] = '\0';
}

//Calls unsigned_to_base normally for positive values,
//and altered with negative sign for negative values
int signed_to_base(char *buf, int n, int val, int base, int width) {
  if (val >= 0) {
    //Postive value
    return unsigned_to_base(buf, n, val, base, width);
  }
  else {
    //Negative value, add - sign and shift array properties
    buf[0] = '-';
    return unsigned_to_base(&buf[1], n - 1, (unsigned int) (-1 * val),
    base, width - 1) + 1;
  }
}

//Returns the length of array excluding null terminator
int strlen(const char *str) {
    const char *start = str;
    while (*str != '\0')
      str++;
    return str - start;
}

//Copies source array to destination array, inclusive of
//null terminator
void strcpy(char *dst, const char *src) {
  for (int i = 0; i <= strlen(src); i++) {
    dst[i] = src[i];
  }
}

//Copies source array to destination array starting from
//given index in destination, exclusive of null terminator
void strcpyIndex(char *dst, const char *src, int start) {
  for (int i = 0; i < strlen(src); i++) {
    dst[i + start] = src[i];
  }
}

//Calls vsnprintf to print out string in given format
//and return size of string
int snprintf(char *s, int n, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int retval = vsnprintf(s, n, format, ap);
  return retval;
}

//Parses through format and adds in arguments as appropriate
int vsnprintf(char *s, int n, const char *format, va_list args) {
  char temp[1024];
  int argIndex = 0;
  char allArgs[1024];
  //index is position in the printed statement
  int index = 0;
  //i is position in the format string
  int i = 0;
  while (i < strlen(format)) {
    if (format[i] == '%') {
      //Look for format string after %
      if (format[i + 1] == 'd' || format[i + 1] == 'x' || format[i + 1] == 'b' ||
    format[i + 1] == 's' || format[i + 1] == 'c') {
        //Add this format to allArgs
        allArgs[argIndex] = format[i + 1];
        //expr is the expression from the argument
        char expr[1024];
        getExpression(expr, args, allArgs, argIndex + 1, 0);
        strcpyIndex(temp, expr, index);
        index += strlen(expr) - 1;
        i += 1;
        argIndex++;
      }
      else if (format[i + 1] == '0') {
        //Formatting contains a width
        char widthInt[1024];
        int widthIndex = 0;
        //Loop until we have the whole width
        while (format[i + widthIndex + 2] != 'd' &&
        format[i + widthIndex + 2] != 'x' &&
        format[i + widthIndex + 2] != 'b') {
          widthInt[widthIndex] = format[i + widthIndex + 2];
          widthIndex++;
        }
        //Null terminate the width
        widthInt[widthIndex] = '\0';
        int widthIntInInt = charToInt(widthInt);
        allArgs[argIndex] = format[i + widthIndex + 2];
        //Get expr with width as parameter
        char expr[1024];
        getExpression(expr, args, allArgs, argIndex + 1, widthIntInInt);
        strcpyIndex(temp, expr, index);
        index += strlen(expr) - 1;
        i += widthIndex + 2;
        argIndex++;
      }
      else {
        //The % sign is just in the text, not as a formatter
        temp[index] = (char) format[i];
      }
    }
    else {
      //Copy text from format to temp array
      temp[index] = (char) format[i];
    }
    index++;
    i++;
  }
  temp[index] = '\0';
  va_end(args);
  strcpy(s, temp);
  int length = strlen(s);
  if (length > n) {
    //Terminate array early if greater than n
    s[n - 1] = '\0';
  }
  return length;
}

// Returns appropriate argument in va_list
void getExpression(char *retval, va_list list, char *allArgs, int numArgs, int width) {
  //Iterating through all other args to get to the current one
  for (int i = 0; i < numArgs - 1; i++) {
    if (allArgs[i] == 's') {
      va_arg(list, char*);
    }
    else {
      //int can represent int and char here
      va_arg(list, int);
    }
  }
  //Identifying current argument
  char format = allArgs[numArgs - 1];
  if (format == 'd') {
    int number = va_arg(list, int);
    signed_to_base(retval, 1024, number, 10, width);
  }
  else if (format == 'x') {
    int number = va_arg(list, int);
    unsigned_to_base(retval, 1024, number, 16, width);
  }
  else if (format == 'b') {
    int number = va_arg(list, int);
    unsigned_to_base(retval, 1024, number, 2, width);
  }
  else if (format == 'c') {
    //Need to get argument as int, then cast to char
    int charInt = va_arg(list, int);
    char tempArr[2];
    tempArr[0] = (char) charInt;
    tempArr[1] = '\0';
    strcpy(retval, tempArr);
  }
  else if (format == 's') {
    char *str = va_arg(list, char*);
    str[strlen(str)] = '\0';
    strcpy(retval, str);
  }
}

//Converts a char to an integer value
int charToInt(char *arr) {
  int length = strlen(arr);
  int retval = 0;
  for (int i = 0; i < length; i++) {
    int digit = arr[length - i - 1] - '0';
    int multiplier = 1;
    for (int j = 0; j < i; j++) {
      //Multiplier is being exponentiated
      multiplier = multiplier * 10;
    }
    retval += digit * multiplier;
  }
  return retval;
}

//Uses vsnprintf and parses through print string
//value to printf
int printf(const char *format, ...) {
  char toPrint[1024];
  va_list list;
  va_start(list, format);
  int length = vsnprintf(toPrint, 1024, format, list);
  for (int i = 0; i < strlen(toPrint); i++) {
    uart_putc(toPrint[i]);
  }
  return length;
}
