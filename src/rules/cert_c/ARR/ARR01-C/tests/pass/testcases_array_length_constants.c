/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>

#define BUFFER_SIZE 256
#define MAX_ITEMS 50

void fill_buffer(char buffer[], size_t buffer_size, char value) {
    for (size_t i = 0; i < buffer_size - 1; i++) {
        buffer[i] = value;
    }
    buffer[buffer_size - 1] = '\0';
}

void init_items(int items[], size_t max_count) {
    for (size_t i = 0; i < max_count; i++) {
        items[i] = i + 1;
    }
}

int main() {
    char text[BUFFER_SIZE];
    int values[MAX_ITEMS];

    fill_buffer(text, BUFFER_SIZE, 'A');
    init_items(values, MAX_ITEMS);

    printf("Buffer: %.10s...\n", text);
    printf("First few values: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    return 0;
}