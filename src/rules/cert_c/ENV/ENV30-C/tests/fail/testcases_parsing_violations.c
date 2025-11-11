/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: parsing_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Path parsing with modification */
void unsafe_path_parsing(void) {
    char *path = getenv("PATH");
    if (path) {
        /* VIOLATION: Using strtok to parse PATH */
        char *dir = strtok(path, ":");  /* Undefined behavior */
        while (dir) {
            printf("Directory: %s\n", dir);
            dir = strtok(NULL, ":");
        }
    }
}

/* NON-COMPLIANT: Variable list parsing */
void unsafe_var_list_parsing(void) {
    char *vars = getenv("CUSTOM_VARS");
    if (vars) {
        /* VIOLATION: Parsing comma-separated values */
        char *var = strtok(vars, ",");  /* Undefined behavior */
        while (var) {
            printf("Variable: %s\n", var);
            var = strtok(NULL, ",");
        }
    }
}

int main(void) {
    setenv("PATH", "/usr/bin:/bin:/usr/local/bin", 1);
    setenv("CUSTOM_VARS", "VAR1,VAR2,VAR3", 1);

    unsafe_path_parsing();
    unsafe_var_list_parsing();
    return 0;
}