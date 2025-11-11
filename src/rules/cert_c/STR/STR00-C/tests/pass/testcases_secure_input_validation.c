/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: secure_input_validation.c
 *
 * This case demonstrates compliant code that uses appropriate character
 * types for secure input validation, buffer management, and string
 * processing operations that are common in security-critical applications.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

/* COMPLIANT: Secure string length calculation with bounds checking */
size_t safe_strlen(const char *str, size_t max_length) {
    if (str == NULL) return 0;

    size_t length = 0;
    while (length < max_length && str[length] != '\0') {
        length++;
    }

    return length;
}

/* COMPLIANT: Secure input reading with proper character handling */
int read_secure_input(char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) return -1;

    size_t pos = 0;
    int ch;

    /* COMPLIANT: Using int for character input to properly handle EOF */
    while (pos < buffer_size - 1 && (ch = getchar()) != EOF) {
        if (ch == '\n') {
            break;
        }

        /* COMPLIANT: Validate character is printable before storing */
        if (isprint((unsigned char)ch)) {
            buffer[pos++] = (char)ch;
        }
        /* Skip non-printable characters for security */
    }

    buffer[pos] = '\0';
    return (int)pos;
}

/* COMPLIANT: Secure password validation with proper character types */
int validate_password(const char *password) {
    if (password == NULL) return 0;

    size_t length = safe_strlen(password, 128);  /* Reasonable max length */

    if (length < 8 || length > 64) {
        return 0;  /* Invalid length */
    }

    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;

    for (size_t i = 0; i < length; i++) {
        /* COMPLIANT: Proper casting for ctype functions */
        int ch = (unsigned char)password[i];

        /* Check for required character classes */
        if (isupper(ch)) {
            has_upper = 1;
        } else if (islower(ch)) {
            has_lower = 1;
        } else if (isdigit(ch)) {
            has_digit = 1;
        } else if (ispunct(ch) || ch == ' ') {
            has_special = 1;
        } else {
            /* Reject passwords with invalid characters */
            return 0;
        }
    }

    /* Require at least 3 of the 4 character classes */
    int class_count = has_upper + has_lower + has_digit + has_special;
    return (class_count >= 3) ? 1 : 0;
}

/* COMPLIANT: Secure email validation with proper character handling */
int validate_email(const char *email) {
    if (email == NULL) return 0;

    size_t length = safe_strlen(email, 254);  /* RFC 5321 limit */

    if (length < 3 || length > 254) {
        return 0;
    }

    const char *at_pos = strchr(email, '@');
    if (at_pos == NULL) {
        return 0;  /* No @ symbol */
    }

    /* Check for exactly one @ symbol */
    if (strchr(at_pos + 1, '@') != NULL) {
        return 0;  /* Multiple @ symbols */
    }

    /* Validate local part (before @) */
    for (const char *p = email; p < at_pos; p++) {
        int ch = (unsigned char)*p;
        if (!isalnum(ch) && ch != '.' && ch != '-' && ch != '_') {
            return 0;
        }
    }

    /* Validate domain part (after @) */
    for (const char *p = at_pos + 1; *p != '\0'; p++) {
        int ch = (unsigned char)*p;
        if (!isalnum(ch) && ch != '.' && ch != '-') {
            return 0;
        }
    }

    return 1;
}

/* COMPLIANT: Secure string sanitization for output */
char *sanitize_for_output(const char *input, size_t max_output_length) {
    if (input == NULL || max_output_length == 0) return NULL;

    size_t input_length = safe_strlen(input, max_output_length * 2);
    char *output = malloc(max_output_length + 1);

    if (output == NULL) return NULL;

    size_t output_pos = 0;

    for (size_t i = 0; i < input_length && output_pos < max_output_length; i++) {
        int ch = (unsigned char)input[i];

        /* COMPLIANT: Only allow safe printable characters */
        if (isprint(ch) && ch != '<' && ch != '>' && ch != '&' &&
            ch != '"' && ch != '\'' && ch != '\\') {
            output[output_pos++] = (char)ch;
        } else if (output_pos < max_output_length - 1) {
            /* Replace unsafe characters with underscore */
            output[output_pos++] = '_';
        }
    }

    output[output_pos] = '\0';
    return output;
}

/* COMPLIANT: Secure username validation */
int validate_username(const char *username) {
    if (username == NULL) return 0;

    size_t length = safe_strlen(username, 32);

    if (length < 3 || length > 32) {
        return 0;
    }

    /* First character must be alphabetic */
    int first_char = (unsigned char)username[0];
    if (!isalpha(first_char)) {
        return 0;
    }

    /* Remaining characters must be alphanumeric or underscore */
    for (size_t i = 1; i < length; i++) {
        int ch = (unsigned char)username[i];
        if (!isalnum(ch) && ch != '_') {
            return 0;
        }
    }

    return 1;
}

int main(void) {
    printf("Secure input validation with appropriate character types:\n\n");

    /* COMPLIANT: Test secure input reading */
    printf("Testing secure input reading:\n");
    printf("Enter some text (non-printable characters will be filtered): ");

    char input_buffer[256];
    int chars_read = read_secure_input(input_buffer, sizeof(input_buffer));

    if (chars_read >= 0) {
        printf("Read %d characters: '%s'\n", chars_read, input_buffer);
    } else {
        printf("Input reading failed\n");
    }

    /* COMPLIANT: Test password validation */
    printf("\nPassword validation tests:\n");

    const char *test_passwords[] = {
        "weak",                    /* Too short */
        "WeakPassword",           /* Missing digits and special chars */
        "Strong123!",             /* Valid */
        "VeryStrong456#",         /* Valid */
        "toolongpasswordthatexceedslimits123456789", /* Too long */
        "Good2Go!",               /* Valid */
        "bad\x80\x90pass"         /* Contains non-printable chars */
    };

    for (size_t i = 0; i < 7; i++) {
        printf("Password '%s': %s\n", test_passwords[i],
               validate_password(test_passwords[i]) ? "VALID" : "INVALID");
    }

    /* COMPLIANT: Test email validation */
    printf("\nEmail validation tests:\n");

    const char *test_emails[] = {
        "user@example.com",       /* Valid */
        "invalid.email",          /* No @ */
        "user@domain@extra.com",  /* Multiple @ */
        "user@domain.co.uk",      /* Valid */
        "test@localhost",         /* Valid */
        "user@",                  /* Missing domain */
        "@domain.com",            /* Missing local part */
        "user@domain.c",          /* Valid (short TLD) */
        "a@b.co"                  /* Valid (minimal) */
    };

    for (size_t i = 0; i < 9; i++) {
        printf("Email '%s': %s\n", test_emails[i],
               validate_email(test_emails[i]) ? "VALID" : "INVALID");
    }

    /* COMPLIANT: Test username validation */
    printf("\nUsername validation tests:\n");

    const char *test_usernames[] = {
        "validuser",              /* Valid */
        "user123",                /* Valid */
        "valid_user_name",        /* Valid */
        "123invalid",             /* Starts with digit */
        "sh",                     /* Too short */
        "_invalidstart",          /* Starts with underscore */
        "user-invalid",           /* Contains hyphen */
        "ValidUser",              /* Valid */
        "user_with_numbers_123"   /* Valid */
    };

    for (size_t i = 0; i < 9; i++) {
        printf("Username '%s': %s\n", test_usernames[i],
               validate_username(test_usernames[i]) ? "VALID" : "INVALID");
    }

    /* COMPLIANT: Test string sanitization */
    printf("\nString sanitization tests:\n");

    const char *test_strings[] = {
        "Hello, World!",
        "<script>alert('xss')</script>",
        "User input with \"quotes\" and 'apostrophes'",
        "Path with \\ backslashes",
        "Normal text 123",
        "Text with &amp; entities"
    };

    for (size_t i = 0; i < 6; i++) {
        char *sanitized = sanitize_for_output(test_strings[i], 100);
        if (sanitized != NULL) {
            printf("Original: %s\n", test_strings[i]);
            printf("Sanitized: %s\n\n", sanitized);
            free(sanitized);
        }
    }

    /* COMPLIANT: Demonstrate secure character comparison */
    printf("Secure character comparison:\n");

    const char *secret = "SecretKey123";
    const char *attempt1 = "SecretKey123";
    const char *attempt2 = "WrongKey456";

    /* Time-constant comparison to prevent timing attacks */
    int result1 = 1, result2 = 1;
    size_t secret_len = strlen(secret);

    /* Compare attempt1 */
    size_t attempt1_len = strlen(attempt1);
    if (secret_len != attempt1_len) result1 = 0;

    for (size_t i = 0; i < secret_len; i++) {
        int c1 = (unsigned char)secret[i];
        int c2 = (i < attempt1_len) ? (unsigned char)attempt1[i] : 0;
        if (c1 != c2) result1 = 0;
    }

    /* Compare attempt2 */
    size_t attempt2_len = strlen(attempt2);
    if (secret_len != attempt2_len) result2 = 0;

    for (size_t i = 0; i < secret_len; i++) {
        int c1 = (unsigned char)secret[i];
        int c2 = (i < attempt2_len) ? (unsigned char)attempt2[i] : 0;
        if (c1 != c2) result2 = 0;
    }

    printf("Attempt 1 '%s': %s\n", attempt1, result1 ? "MATCH" : "NO MATCH");
    printf("Attempt 2 '%s': %s\n", attempt2, result2 ? "MATCH" : "NO MATCH");

    /* COMPLIANT: Demonstrate secure random string generation */
    printf("\nSecure character generation:\n");

    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char random_string[17];  /* 16 chars + null terminator */

    /* Simple pseudo-random generation for demonstration */
    srand(12345);  /* Fixed seed for reproducible output */

    for (int i = 0; i < 16; i++) {
        int index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    random_string[16] = '\0';

    printf("Generated string: %s\n", random_string);

    /* Validate the generated string */
    int valid_generated = 1;
    for (int i = 0; i < 16; i++) {
        int ch = (unsigned char)random_string[i];
        if (!isalnum(ch)) {
            valid_generated = 0;
            break;
        }
    }

    printf("Generated string validation: %s\n", valid_generated ? "VALID" : "INVALID");

    return 0;
}