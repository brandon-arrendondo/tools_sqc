/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t capacity;
    size_t used;
    int *data;
} dynamic_buffer;

dynamic_buffer* create_buffer(size_t initial_size) {
    dynamic_buffer *buf = malloc(sizeof(dynamic_buffer));
    if (!buf) return NULL;

    buf->data = malloc(initial_size * sizeof(int));
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->capacity = initial_size;
    buf->used = 0;
    return buf;
}

void destroy_buffer(dynamic_buffer *buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

void add_element(dynamic_buffer *buf, int value) {
    if (buf && buf->used < buf->capacity) {
        buf->data[buf->used++] = value;
    }
}

int main() {
    dynamic_buffer *buffer = create_buffer(10);
    if (buffer) {
        for (int i = 0; i < 8; i++) {
            add_element(buffer, i * i);
        }

        printf("Buffer contents: ");
        for (size_t i = 0; i < buffer->used; i++) {
            printf("%d ", buffer->data[i]);
        }
        printf("\n");

        printf("Used: %zu/%zu elements\n", buffer->used, buffer->capacity);

        destroy_buffer(buffer);
    }

    return 0;
}