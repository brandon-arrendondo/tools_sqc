/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: validation_rules.c
 *
 * This case demonstrates violations where validation rules and
 * constraints that never change are not const-qualified.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void email_validation(void) {
    /* NON-COMPLIANT: Email validation rules should be const */
    int MIN_EMAIL_LENGTH = 5;
    int MAX_EMAIL_LENGTH = 254;
    int MIN_LOCAL_PART = 1;
    int MAX_LOCAL_PART = 64;
    int MIN_DOMAIN_PART = 1;
    int MAX_DOMAIN_PART = 253;

    /* NON-COMPLIANT: Validation characters should be const */
    char at_symbol = '@';
    char dot_symbol = '.';
    char hyphen_symbol = '-';
    char underscore_symbol = '_';

    printf("Email Validation Rules:\n");
    printf("  Total length: %d-%d characters\n", MIN_EMAIL_LENGTH, MAX_EMAIL_LENGTH);
    printf("  Local part: %d-%d characters\n", MIN_LOCAL_PART, MAX_LOCAL_PART);
    printf("  Domain part: %d-%d characters\n", MIN_DOMAIN_PART, MAX_DOMAIN_PART);
    printf("  Required symbols: %c, %c\n", at_symbol, dot_symbol);
    printf("  Allowed symbols: %c, %c\n", hyphen_symbol, underscore_symbol);

    /* Rules used for validation but never modified */
    char test_email[] = "user@example.com";
    int email_length = strlen(test_email);

    if (email_length >= MIN_EMAIL_LENGTH && email_length <= MAX_EMAIL_LENGTH) {
        printf("  Email length valid: %s (%d chars)\n", test_email, email_length);
    }
}

void password_validation(void) {
    /* NON-COMPLIANT: Password rules should be const */
    int MIN_PASSWORD_LENGTH = 8;
    int MAX_PASSWORD_LENGTH = 128;
    int MIN_UPPERCASE_COUNT = 1;
    int MIN_LOWERCASE_COUNT = 1;
    int MIN_DIGIT_COUNT = 1;
    int MIN_SPECIAL_COUNT = 1;

    /* NON-COMPLIANT: Character sets should be const */
    char special_chars[] = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    char forbidden_chars[] = " \\\"'`";

    printf("\nPassword Validation Rules:\n");
    printf("  Length: %d-%d characters\n", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
    printf("  Minimum uppercase: %d\n", MIN_UPPERCASE_COUNT);
    printf("  Minimum lowercase: %d\n", MIN_LOWERCASE_COUNT);
    printf("  Minimum digits: %d\n", MIN_DIGIT_COUNT);
    printf("  Minimum special: %d\n", MIN_SPECIAL_COUNT);
    printf("  Special chars: %s\n", special_chars);
    printf("  Forbidden chars: %s\n", forbidden_chars);

    /* Rules used for password checking but never modified */
    char test_password[] = "MyPass123!";
    int pwd_length = strlen(test_password);

    if (pwd_length >= MIN_PASSWORD_LENGTH && pwd_length <= MAX_PASSWORD_LENGTH) {
        printf("  Password length valid: %d chars\n", pwd_length);
    }
}

void input_sanitization(void) {
    /* NON-COMPLIANT: Sanitization rules should be const */
    int MAX_INPUT_LENGTH = 1000;
    int MAX_FIELD_LENGTH = 255;
    int MAX_FILENAME_LENGTH = 255;
    int MAX_URL_LENGTH = 2048;

    /* NON-COMPLIANT: Dangerous character sets should be const */
    char sql_injection_chars[] = "';\"\\";
    char xss_chars[] = "<>&\"'";
    char path_traversal_chars[] = "../";
    char command_injection_chars[] = "|;&`$()";

    printf("\nInput Sanitization Rules:\n");
    printf("  Max input length: %d\n", MAX_INPUT_LENGTH);
    printf("  Max field length: %d\n", MAX_FIELD_LENGTH);
    printf("  Max filename length: %d\n", MAX_FILENAME_LENGTH);
    printf("  Max URL length: %d\n", MAX_URL_LENGTH);

    printf("  SQL injection chars: %s\n", sql_injection_chars);
    printf("  XSS chars: %s\n", xss_chars);
    printf("  Path traversal: %s\n", path_traversal_chars);
    printf("  Command injection: %s\n", command_injection_chars);

    /* Rules used for input validation but never modified */
    char user_input[] = "normal_input_text";
    int input_length = strlen(user_input);

    if (input_length <= MAX_INPUT_LENGTH) {
        printf("  Input length acceptable: %d chars\n", input_length);
    }
}

void numeric_validation(void) {
    /* NON-COMPLIANT: Numeric ranges should be const */
    int MIN_AGE = 0;
    int MAX_AGE = 150;
    int MIN_PERCENTAGE = 0;
    int MAX_PERCENTAGE = 100;
    int MIN_PORT = 1;
    int MAX_PORT = 65535;

    /* NON-COMPLIANT: Precision constraints should be const */
    int MAX_DECIMAL_PLACES = 2;
    double MIN_PRICE = 0.01;
    double MAX_PRICE = 99999.99;
    double EPSILON = 0.001;

    printf("\nNumeric Validation Rules:\n");
    printf("  Age range: %d-%d years\n", MIN_AGE, MAX_AGE);
    printf("  Percentage range: %d-%d%%\n", MIN_PERCENTAGE, MAX_PERCENTAGE);
    printf("  Port range: %d-%d\n", MIN_PORT, MAX_PORT);
    printf("  Price range: $%.2f-$%.2f\n", MIN_PRICE, MAX_PRICE);
    printf("  Max decimal places: %d\n", MAX_DECIMAL_PLACES);
    printf("  Comparison epsilon: %f\n", EPSILON);

    /* Ranges used for validation but never modified */
    int test_age = 25;
    int test_port = 8080;
    double test_price = 19.99;

    if (test_age >= MIN_AGE && test_age <= MAX_AGE) {
        printf("  Age %d is valid\n", test_age);
    }
    if (test_port >= MIN_PORT && test_port <= MAX_PORT) {
        printf("  Port %d is valid\n", test_port);
    }
    if (test_price >= MIN_PRICE && test_price <= MAX_PRICE) {
        printf("  Price $%.2f is valid\n", test_price);
    }
}

int main(void) {
    /* NON-COMPLIANT: General validation limits should be const */
    int MAX_ARRAY_SIZE = 1000;
    int MAX_STRING_LENGTH = 4096;
    int MAX_RECURSION_DEPTH = 100;
    int MAX_RETRY_ATTEMPTS = 3;

    printf("General Validation Limits:\n");
    printf("  Max array size: %d elements\n", MAX_ARRAY_SIZE);
    printf("  Max string length: %d chars\n", MAX_STRING_LENGTH);
    printf("  Max recursion depth: %d levels\n", MAX_RECURSION_DEPTH);
    printf("  Max retry attempts: %d\n", MAX_RETRY_ATTEMPTS);

    email_validation();
    password_validation();
    input_sanitization();
    numeric_validation();

    return 0;
}