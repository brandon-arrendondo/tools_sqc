/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

void read_config(const int config[]) {
    size_t config_size = sizeof(config) / sizeof(config[0]);

    for (size_t i = 0; i < config_size; i++) {
        printf("Config[%zu] = %d\n", i, config[i]);
    }
}

int main() {
    const int settings[20] = {0};

    read_config(settings);

    return 0;
}