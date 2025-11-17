/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>
#include <string.h>

void process_string(char str[]) {
    size_t length = sizeof(str);

    for (size_t i = 0; i < length; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    char message[100] = "Hello, World!";

    process_string(message);

    return 0;
}