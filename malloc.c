#include "malloc.h"
#include <stddef.h> // for NULL

extern int __bss_end__;

// Simple function to round up x to multiple of n.
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))


static void *heap, *heap_max;
struct header {
    size_t payload_size;
    int status;       // 0 if free, 1 if in use
};
struct header *rover;

// Initializes malloc by creating initial header
void malloc_init(size_t total)
{
    heap = &__bss_end__;
    struct header *h = (struct header *) heap;
    h->payload_size = total - 8;
    h->status = 0;
    // Set rover to first header
    rover = h;
    heap_max = (char *)heap + total;
}

// Allocates memory in the heap for a given size
void *malloc(size_t sz)
{
  sz = roundup(sz, 8);
  struct header *currHeader = (struct header *) rover;
  // Loops through headers until it finds one that is free and big enough
  while (currHeader->status == 1 || currHeader->payload_size < sz) {
    currHeader = (struct header *)((char *)currHeader + currHeader->payload_size + 8);
    if (currHeader > (struct header *) heap_max) {
      // Wrap around if you reach the heap max
      currHeader = (struct header *) heap;
    }
    if (currHeader == rover) {
      // If it hits the original header again, it didn't
      // find any appropriate free space and returns null
      return NULL;
    }
  }
  // Creating new header
  int origSize = currHeader->payload_size;
  struct header *newh = currHeader;
  newh->payload_size = sz;
  newh->status = 1;
  rover = newh;
  if (origSize != sz) {
    // Creates header for split block
    struct header *splith = (struct header *)((char *)currHeader + sz + 8);
    splith->payload_size = origSize - sz - 8;
    splith->status = 0;
  }
  return (void *)((char *) newh + 8);
}

// Frees memory and combines free blocks of memory
void free(void *ptr)
{
  struct header *thish = (struct header *) ((char *) ptr - 8);
  struct header *nexth = (struct header *)((char *)thish + thish->payload_size + 8);
  // Combining blocks by looping through consecutive free blocks
  while (nexth <= (struct header*) heap_max && nexth->status == 0) {
    thish->payload_size = thish->payload_size + nexth->payload_size + 8;
    nexth = (struct header *)((char *)thish + thish->payload_size + 8);
    if (nexth == rover) {
      rover = thish;
    }
  }
  // Free this block
  thish->status = 0;
}

/*
#ifdef LEAK_DETECTOR

// This function will only be included if you compile with -DLEAK_DETECTOR.
void leak_report()
{
    // FIXME: Fill in your own code here for the extension.
}

#endif
*/
