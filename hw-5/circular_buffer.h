#ifndef CIRCULAR_BUFFER_
#define CIRCULAR_BUFFER_

#include <stdlib.h>

typedef struct {
    int left;
    int right;
    int size;
    int count;
    int* memory; // [left, right)
} circular_buffer;


void circular_buffer_init(circular_buffer* buffer, int size);
void circular_buffer_destroy(circular_buffer* buffer);
void circular_buffer_push(circular_buffer* buffer, int val);
int  circular_buffer_pop(circular_buffer* buffer);
int  circular_buffer_empty(circular_buffer* buffer);

void _circular_buffer_next(circular_buffer* buffer, int* ptr);

#endif

