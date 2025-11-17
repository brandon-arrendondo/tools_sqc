/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: final_violations_4.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: License key modification */
void unsafe_license_modification(void) {
    char *license = getenv("LICENSE_KEY");
    if (license && strlen(license) > 8) {
        /* VIOLATION: Masking license key */
        for (int i = 4; i < strlen(license) - 4; i++) {
            license[i] = '*';  /* Undefined behavior */
        }
        printf("Masked license: %s\n", license);
    }
}

/* NON-COMPLIANT: Cache directory modification */
void unsafe_cache_dir(void) {
    char *cache_dir = getenv("CACHE_DIR");
    if (cache_dir) {
        strcat(cache_dir, "/temp");  /* Undefined behavior */
        printf("Temp cache dir: %s\n", cache_dir);
    }
}

int main(void) {
    setenv("LICENSE_KEY", "ABCD-1234-EFGH-5678", 1);
    setenv("CACHE_DIR", "/var/cache", 1);

    unsafe_license_modification();
    unsafe_cache_dir();
    return 0;
}