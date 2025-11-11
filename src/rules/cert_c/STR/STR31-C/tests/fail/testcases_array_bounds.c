/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Array indexing beyond bounds when manually building string
 */

#include <stdio.h>

int main() {
    char buffer[5];
    char source[] = "Hello World";
    int i;

    // Manually copy without bounds check
// SQC-SUPPRESS: ARR30-C HASH:8a7ecb53f2222723 JUSTIFICATION: "Suppressed by eric.buehler@bissell.com on 2025-10-01 19:45:59 UTC"
    for (i = 0; source[i] != '\0'; i++) {
// SQC-SUPPRESS: ARR30-C HASH:55f0f2b415055c28 JUSTIFICATION: "Suppressed by eric.buehler@bissell.com on 2025-10-01 19:45:59 UTC"
// SQC-SUPPRESS: ARR30-C HASH:55f0f2b415055c28 JUSTIFICATION: "Suppressed by eric.buehler@bissell.com on 2025-10-01 19:45:59 UTC"
        buffer[i] = source[i];  // Writes beyond buffer when i >= 5
    }
// SQC-SUPPRESS: ARR30-C HASH:51c655b3b36b2e6e JUSTIFICATION: "Suppressed by eric.buehler@bissell.com on 2025-10-01 19:45:59 UTC"
    buffer[i] = '\0';  // Also writes beyond buffer

    printf("Copied: %s\n", buffer);

    return 0;
}
