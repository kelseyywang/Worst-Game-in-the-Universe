#include "backtrace.h"
#include "printf.h"

// Backtrace fills up f with infomation from stack
int backtrace(struct frame f[], int max_frames)
{
  // Getting fp address
  unsigned fp_addr;
  __asm__("mov %0, fp" : "=r" (fp_addr));
  int count = 0;
  while (fp_addr != 0 && count < max_frames) {
    // Looping through past calls
    struct frame f1;
    unsigned lr_addr = fp_addr - 4;
    unsigned resume_val = *((unsigned int*) lr_addr);
    // Resume is the saved lr
    f1.fn_resume = resume_val;
    unsigned saved_fp_addr = fp_addr - 12;
    unsigned fp_val = *((unsigned int*) saved_fp_addr);
    unsigned pc_val = *((unsigned int*) fp_val);
    unsigned first_val = pc_val - 12;
    // Position of first instruction
    f1.fn_first = first_val;
    f[count] = f1;
    fp_addr = fp_val;
    count++;
  }
    return count;
}

// Goes through f and prints relevant backtrace info
void print_frames(struct frame f[], int n)
{
  char *name;
    for (int i = 0; i < n; i++) {
        unsigned int *pc_minus_16 = (unsigned int *) (f[i].fn_first - 4);
        if ((*pc_minus_16 & 0xff000000) == 0xff000000) {
          int length = *pc_minus_16 & ~0xff000000;
          // Gets address where name is stored
          char *name_addr = (char *) (f[i].fn_first - 4 - length);
          name = name_addr;
        }
        else {
          // No name found
          name = "???";
        }
        // Difference between resume and first
        int delta = f[i].fn_resume - f[i].fn_first;

        // Don't change this print format!
        printf("#%d 0x%x at %s+%d\n", i, f[i].fn_resume, name, delta);
    }
}

// Print frames up to 10
void print_backtrace(void)
{
  struct frame f[10];
  backtrace(f, 10);
  print_frames(f, 10);
}
