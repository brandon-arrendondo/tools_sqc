/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: final_violations_1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Memory zeroing */
void unsafe_memory_zeroing(void) {
    char *sensitive = getenv("SECRET_KEY");
    if (sensitive) {
        /* VIOLATION: Attempting to clear sensitive data */
        memset(sensitive, 0, strlen(sensitive));  /* Undefined behavior */
        printf("Cleared secret\n");
    }
}

/* NON-COMPLIANT: URL parameter modification */
void unsafe_url_params(void) {
    char *api_url = getenv("API_ENDPOINT");
    if (api_url) {
        strcat(api_url, "?format=json");  /* Undefined behavior */
        printf("API URL: %s\n", api_url);
    }
}

int main(void) {
    setenv("SECRET_KEY", "abc123", 1);
    setenv("API_ENDPOINT", "https://api.com/data", 1);

    unsafe_memory_zeroing();
    unsafe_url_params();
    return 0;
}