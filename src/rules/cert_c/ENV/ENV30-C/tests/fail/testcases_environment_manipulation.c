/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: environment_manipulation.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Environment variable expansion */
void unsafe_env_expansion(void) {
    char *var = getenv("EXPAND_ME");
    if (var) {
        /* VIOLATION: In-place expansion attempt */
        if (strstr(var, "$HOME")) {
            /* This is a simplified violation - real expansion would be more complex */
            strcpy(var, "/home/user/expanded");  /* Undefined behavior */
        }
        printf("Expanded: %s\n", var);
    }
}

/* NON-COMPLIANT: Variable substitution */
void unsafe_variable_substitution(void) {
    char *template = getenv("TEMPLATE");
    if (template) {
        /* VIOLATION: Template substitution in place */
        char *placeholder = strstr(template, "${USER}");
        if (placeholder) {
            strcpy(placeholder, "admin");  /* Undefined behavior */
        }
        printf("Substituted: %s\n", template);
    }
}

int main(void) {
    setenv("EXPAND_ME", "Path is $HOME/docs", 1);
    setenv("TEMPLATE", "Welcome ${USER} to the system", 1);

    unsafe_env_expansion();
    unsafe_variable_substitution();
    return 0;
}