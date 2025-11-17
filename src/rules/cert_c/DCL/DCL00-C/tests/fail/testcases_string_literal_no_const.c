/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: string_literal_no_const.c
 *
 * This case demonstrates a violation where pointers to string literals
 * are not const-qualified, allowing potential modification attempts.
 */

#include <stdio.h>
#include <string.h>

void display_message(void) {
    /* NON-COMPLIANT: Pointer to string literal should be const */
    char *message = "Hello, World!";
    
    /* This could lead to undefined behavior if modified */
    printf("Message: %s\n", message);
    
    /* Dangerous: Compiler may allow this but it's undefined behavior */
    /* message[0] = 'h'; */ /* Would crash at runtime */
}

void print_error_codes(void) {
    /* NON-COMPLIANT: Array of string literals should use const */
    char *error_messages[] = {
        "Success",
        "File not found",
        "Permission denied",
        "Invalid argument"
    };
    
    for (int i = 0; i < 4; i++) {
        printf("Error %d: %s\n", i, error_messages[i]);
    }
}

int main(void) {
    /* NON-COMPLIANT: String literal pointer without const */
    char *program_name = "TestProgram";
    
    printf("Running %s\n", program_name);
    
    display_message();
    print_error_codes();
    
    return 0;
}