/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ARRAY_SIZE 1000

int shared_array[ARRAY_SIZE];
volatile sig_atomic_t array_index = 0;
volatile sig_atomic_t write_operations = 0;

void array_handler(int sig) {
    write_operations++;

    printf("Handler: Signal %d writing to array\n", sig);

    // Violation: Array operations without proper signal masking
    // can cause index corruption and buffer overflows
    int start_index = array_index;
    int values_to_write = sig * 10;

    printf("Handler: Writing %d values starting at index %d\n",
           values_to_write, start_index);

    for (int i = 0; i < values_to_write; i++) {
        // Vulnerable index calculation
        int current_index = array_index;

        if (current_index >= ARRAY_SIZE) {
            printf("Handler: Index overflow! Resetting to 0\n");
            array_index = 0;
            current_index = 0;
        }

        // Write value
        shared_array[current_index] = sig * 1000 + i;

        // Increment index with vulnerability window
        usleep(1000);
        array_index = current_index + 1;

        // Additional vulnerability - another signal could modify array_index here
        if (array_index != current_index + 1) {
            printf("Handler: ERROR - Index was modified concurrently!\n");
            printf("Expected %d, found %d\n", current_index + 1, array_index);
        }
    }

    printf("Handler: Wrote values, final index = %d\n", array_index);

    // Verify written data
    int corruption_count = 0;
    for (int i = start_index; i < start_index + values_to_write && i < ARRAY_SIZE; i++) {
        int expected = sig * 1000 + (i - start_index);
        if (shared_array[i] != expected) {
            corruption_count++;
            if (corruption_count <= 3) {
                printf("Handler: Corruption at [%d]: expected %d, found %d\n",
                       i, expected, shared_array[i]);
            }
        }
    }

    if (corruption_count > 0) {
        printf("Handler: Found %d corrupted values\n", corruption_count);
    }
}

int main() {
    struct sigaction sa;

    // Initialize array
    memset(shared_array, 0, sizeof(shared_array));

    // Install handler without masking
    sa.sa_handler = array_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Array index operations vulnerable to races
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to corrupt array operations\n");

    while (1) {
        printf("Main: Write operations: %d, current index: %d\n",
               write_operations, array_index);

        // Check array bounds
        if (array_index >= ARRAY_SIZE) {
            printf("Main: ERROR - Array index out of bounds: %d\n", array_index);
            array_index = 0;
        }

        // Main thread also writes to array
        if (array_index < ARRAY_SIZE - 10) {
            for (int i = 0; i < 5; i++) {
                shared_array[array_index + i] = -1; // Marker for main thread
            }
            array_index += 5;
        }

        // Look for corruption patterns
        int zero_count = 0, negative_count = 0, signal_count = 0;
        for (int i = 0; i < ARRAY_SIZE; i++) {
            if (shared_array[i] == 0) zero_count++;
            else if (shared_array[i] < 0) negative_count++;
            else signal_count++;
        }

        printf("Main: Array stats - zeros: %d, negatives: %d, signals: %d\n",
               zero_count, negative_count, signal_count);

        sleep(3);
    }

    return 0;
}