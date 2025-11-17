/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Network data used as format string
 */

#include <stdio.h>
#include <string.h>

void process_network_data(const char *data) {
    // Simulate processing network data
    char buffer[200];

    // VULNERABLE: network data used as format string
    sprintf(buffer, data);
    printf("Processed: %s\n", buffer);
}

int main() {
    char network_input[100];

    printf("Simulating network input: ");
    fgets(network_input, sizeof(network_input), stdin);

    // Remove newline
    network_input[strcspn(network_input, "\n")] = 0;

    process_network_data(network_input);
    return 0;
}