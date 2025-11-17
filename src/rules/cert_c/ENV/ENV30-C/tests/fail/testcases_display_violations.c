/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: display_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Display variable modification */
void unsafe_display_modification(void) {
    char *display = getenv("DISPLAY");
    if (display) {
        /* VIOLATION: Changing display number */
        char *colon = strchr(display, ':');
        if (colon && *(colon + 1)) {
            *(colon + 1) = '1';  /* Undefined behavior */
        }
        printf("Modified display: %s\n", display);
    }
}

/* NON-COMPLIANT: Term modification */
void unsafe_term_modification(void) {
    char *term = getenv("TERM");
    if (term) {
        /* VIOLATION: Adding color capability */
        strcat(term, "-256color");  /* Undefined behavior */
        printf("Enhanced term: %s\n", term);
    }
}

int main(void) {
    setenv("DISPLAY", ":0.0", 1);
    setenv("TERM", "xterm", 1);

    unsafe_display_modification();
    unsafe_term_modification();
    return 0;
}