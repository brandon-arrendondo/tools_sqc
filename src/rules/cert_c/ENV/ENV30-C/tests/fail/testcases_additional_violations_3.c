/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_3.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* NON-COMPLIANT: Timezone modification */
void unsafe_timezone_modification(void) {
    char *tz = getenv("TZ");
    if (tz) {
        strcpy(tz, "UTC");  /* Undefined behavior */
        printf("Changed timezone: %s\n", tz);
    }
}

/* NON-COMPLIANT: Pager modification */
void unsafe_pager_modification(void) {
    char *pager = getenv("PAGER");
    if (pager) {
        strcat(pager, " -n");  /* Undefined behavior */
        printf("Enhanced pager: %s\n", pager);
    }
}

/* NON-COMPLIANT: Browser modification */
void unsafe_browser_modification(void) {
    char *browser = getenv("BROWSER");
    if (browser) {
        strcat(browser, " --private");  /* Undefined behavior */
        printf("Private browser: %s\n", browser);
    }
}

int main(void) {
    setenv("TZ", "America/New_York", 1);
    setenv("PAGER", "less", 1);
    setenv("BROWSER", "firefox", 1);

    unsafe_timezone_modification();
    unsafe_pager_modification();
    unsafe_browser_modification();
    return 0;
}