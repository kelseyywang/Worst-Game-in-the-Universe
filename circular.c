#include "circular.h"
#include "malloc.h"
#include "assert.h"

#define CAPACITY 128

struct cir {
  int buf[CAPACITY];
  unsigned head;
  unsigned tail;
};

/* Allocates a new circular buffer, initializes it, and returns a
 pointer to it. */
cir_t *cir_new() {
  cir_t *newCir;
  newCir = malloc(sizeof(struct cir));
  newCir->head = 0;
  newCir->tail = 0;
  return newCir;
}

/* Returns 1 if the buffer is empty, 0 otherwise */
int cir_empty(cir_t *cir) {
  if (cir->head == cir->tail) {
    return 1;
  }
  return 0;
}

/* Returns 1 if the buffer is full (no more elements could fit), 0 otherwise */
int cir_full(cir_t *cir) {
  if (cir->head == (cir->tail + 1) % CAPACITY) {
    return 1;
  }
  return 0;
}

/* Adds an element to the ring buffer
 Asserts an error if the buffer is full */
void cir_enqueue(cir_t *cir, int x) {
  cir->buf[cir->tail] = x;
  cir->tail = (cir->tail + 1) % CAPACITY;
}

/* Removes an element from the buffer
 Blocks if the buffer is empty */
int cir_dequeue(cir_t *cir) {
  //hang if circular array is empty
  while (cir_empty(cir) == 1) {}
  int retval = cir->buf[cir->head];
  cir->head = (cir->head + 1) % CAPACITY;
  return retval;
}
