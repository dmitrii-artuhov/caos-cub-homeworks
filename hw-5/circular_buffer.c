#include "circular_buffer.h"

void circular_buffer_init(circular_buffer* buffer, int size) {
    buffer->left = 0;
    buffer->right = 0;
    buffer->count = 0;
    buffer->size = size;
    buffer->memory = (int*)calloc(size, sizeof(int));

    if (buffer->memory == NULL) {
        exit(EXIT_FAILURE);
    }
}

void circular_buffer_destroy(circular_buffer* buffer) {
    free(buffer->memory);
}

void circular_buffer_push(circular_buffer* buffer, int val) {
    buffer->memory[buffer->right] = val;
    _circular_buffer_next(buffer, &buffer->right);
    buffer->count++;
}

int circular_buffer_pop(circular_buffer* buffer) {
    int val = buffer->memory[buffer->left];
    _circular_buffer_next(buffer, &buffer->left);
    buffer->count--;
    return val;
}

int circular_buffer_empty(circular_buffer* buffer) {
    return buffer->count == 0;
}

void _circular_buffer_next(circular_buffer* buffer, int* ptr) {
    *ptr += 1;

    if (*ptr == buffer->size) {
        *ptr = 0;
    }
}
