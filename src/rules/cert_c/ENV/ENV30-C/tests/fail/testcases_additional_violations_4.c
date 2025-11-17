/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_4.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Proxy modification */
void unsafe_proxy_modification(void) {
    char *http_proxy = getenv("HTTP_PROXY");
    if (http_proxy) {
        strcat(http_proxy, ":8080");  /* Undefined behavior */
        printf("Enhanced proxy: %s\n", http_proxy);
    }
}

/* NON-COMPLIANT: Email modification */
void unsafe_email_modification(void) {
    char *email = getenv("EMAIL");
    if (email) {
        char *at = strchr(email, '@');
        if (at) {
            strcpy(at + 1, "company.com");  /* Undefined behavior */
        }
        printf("Company email: %s\n", email);
    }
}

/* NON-COMPLIANT: Version modification */
void unsafe_version_modification(void) {
    char *version = getenv("APP_VERSION");
    if (version) {
        strcat(version, "-release");  /* Undefined behavior */
        printf("Release version: %s\n", version);
    }
}

int main(void) {
    setenv("HTTP_PROXY", "http://proxy.example.com", 1);
    setenv("EMAIL", "user@example.com", 1);
    setenv("APP_VERSION", "1.0.0", 1);

    unsafe_proxy_modification();
    unsafe_email_modification();
    unsafe_version_modification();
    return 0;
}