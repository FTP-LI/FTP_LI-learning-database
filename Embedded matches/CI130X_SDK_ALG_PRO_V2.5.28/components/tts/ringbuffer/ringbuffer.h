#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H


typedef struct __ringbuffer_t
{
    uint32_t total;
    uint32_t available;
    uint8_t *base;
    uint8_t *head;
    uint8_t *tail;
    SemaphoreHandle_t buffer_mutex;
}ringbuffer_t;

// NOTE:
// None of the functions below are thread safe when it comes to accessing the
// *rb pointer. It is *NOT* possible to insert and pop/delete at the same time.
// Callers must protect the *rb pointer separately.

// Create a ringbuffer with the specified size
// Returns NULL if memory allocation failed. Resulting pointer must be freed
// using |ringbuffer_free|.
ringbuffer_t *ringbuffer_init(const uint32_t size);

// Frees the ringbuffer structure and buffer
// Save to call with NULL.
void ringbuffer_free(ringbuffer_t *rb);


void ringbuffer_clear(ringbuffer_t *rb);

// Returns remaining buffer size
uint32_t ringbuffer_available(const ringbuffer_t *rb);

// Returns size of data in buffer
uint32_t ringbuffer_size(const ringbuffer_t *rb);

// Attempts to insert up to |length| bytes of data at |p| into the buffer
// Return actual number of bytes added. Can be less than |length| if buffer
// is full.
uint32_t ringbuffer_insert(ringbuffer_t *rb, const uint8_t *p, uint32_t length);

// Peek |length| number of bytes from the ringbuffer, starting at |offset|,
// into the buffer |p|. Return the actual number of bytes peeked. Can be less
// than |length| if there is less than |length| data available. |offset| must
// be non-negative.
uint32_t ringbuffer_peek(const ringbuffer_t *rb, int offset, uint8_t *p, uint32_t length);

// Does the same as |ringbuffer_peek|, but also advances the ring buffer head
uint32_t ringbuffer_pop(ringbuffer_t *rb, uint8_t *p, uint32_t length);

// Deletes |length| bytes from the ringbuffer starting from the head
// Return actual number of bytes deleted.
uint32_t ringbuffer_delete(ringbuffer_t *rb, uint32_t length);
#endif