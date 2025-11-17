/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

void create_vla(int size) {
    int vla[size];

    for (int i = 0; i < size; i++) {
        vla[i] = i;
    }
}

int main() {
    int user_size;
    scanf("%d", &user_size);

    create_vla(user_size);

    return 0;
}