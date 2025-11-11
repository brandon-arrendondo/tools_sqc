/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: regex_pattern_matching.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for regular expression pattern matching and string
 * validation operations, leading to character encoding issues.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Simple pattern matching functions using wrong character types */

/* VIOLATION: Function using signed char for pattern matching */
int simple_wildcard_match(signed char *pattern, signed char *text) {
    printf("Matching pattern '%s' against text '%s'\n", pattern, text);  /* Warning */

    while (*pattern && *text) {
        if (*pattern == '*') {
            /* Skip multiple asterisks */
            while (*pattern == '*') pattern++;
            if (!*pattern) return 1;  /* Pattern ends with *, match */

            /* Try to match remaining pattern */
            while (*text) {
                if (simple_wildcard_match(pattern, text)) return 1;
                text++;
            }
            return 0;
        } else if (*pattern == '?' || *pattern == *text) {
            pattern++;
            text++;
        } else {
            return 0;  /* No match */
        }
    }

    /* Handle remaining asterisks */
    while (*pattern == '*') pattern++;
    return !*pattern && !*text;
}

/* VIOLATION: Email validation with unsigned char */
int validate_email_unsigned(unsigned char *email) {
    printf("Validating email: %s\n", email);  /* Warning */

    /* VIOLATION: Character classification with unsigned char */
    unsigned char *at_pos = strchr(email, '@');  /* Warning */
    if (!at_pos) return 0;

    /* Check local part */
    for (unsigned char *p = email; p < at_pos; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-') {
            return 0;
        }
    }

    /* Check domain part */
    unsigned char *domain = at_pos + 1;
    unsigned char *dot_pos = strchr(domain, '.');  /* Warning */
    if (!dot_pos) return 0;

    return 1;  /* Simplified validation */
}

int main(void) {
    printf("Pattern matching with inappropriate character types:\n\n");

    /* VIOLATION: Wildcard pattern matching with signed char */
    signed char patterns[][20] = {
        "*.txt", "file?.dat", "test*", "*.*"
    };
    signed char filenames[][30] = {
        "document.txt", "file1.dat", "test123", "readme.md"
    };

    printf("Wildcard matching results:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int match = simple_wildcard_match(patterns[i], filenames[j]);
            printf("Pattern '%s' vs '%s': %s\n",  /* Warning */
                   patterns[i], filenames[j], match ? "MATCH" : "NO MATCH");
        }
    }

    /* VIOLATION: Phone number validation with character type issues */
    printf("\nPhone number validation:\n");

    char phone_numbers[][20] = {
        "+1-555-123-4567",
        "(555) 123-4567",
        "555.123.4567",
        "5551234567"
    };

    for (int i = 0; i < 4; i++) {
        printf("Validating phone: %s\n", phone_numbers[i]);

        signed char *phone = (signed char*)phone_numbers[i];
        int digit_count = 0;
        int has_country_code = 0;

        /* VIOLATION: Character analysis with signed char */
        for (size_t j = 0; phone[j] != '\0'; j++) {
            signed char c = phone[j];

            if (isdigit(c)) {  /* Potential undefined behavior */
                digit_count++;
            } else if (c == '+' && j == 0) {
                has_country_code = 1;
            } else if (c != '-' && c != '.' && c != '(' && c != ')' && c != ' ') {
                printf("  Invalid character: %c\n", c);
            }
        }

        printf("  Digits: %d, Country code: %s\n",
               digit_count, has_country_code ? "yes" : "no");
    }

    /* VIOLATION: URL validation with mixed character types */
    printf("\nURL validation:\n");

    unsigned char urls[][50] = {
        "https://www.example.com/path?param=value",
        "ftp://files.example.org/file.zip",
        "mailto:user@domain.com",
        "file:///home/user/document.pdf"
    };

    for (int i = 0; i < 4; i++) {
        printf("Validating URL: %s\n", urls[i]);  /* Warning */

        /* VIOLATION: Protocol extraction */
        unsigned char *protocol_end = strstr(urls[i], "://");  /* Warning */
        if (protocol_end) {
            *protocol_end = '\0';
            printf("  Protocol: %s\n", urls[i]);  /* Warning */
            *protocol_end = ':';  /* Restore */

            /* VIOLATION: Host extraction */
            unsigned char *host_start = protocol_end + 3;
            unsigned char *path_start = strchr(host_start, '/');  /* Warning */
            if (path_start) {
                printf("  Has path component\n");
            }
        }
    }

    /* VIOLATION: Email validation with different types */
    printf("\nEmail validation:\n");

    unsigned char emails[][40] = {
        "user@example.com",
        "first.last@domain.org",
        "invalid.email",
        "test@sub.domain.net"
    };

    for (int i = 0; i < 4; i++) {
        int valid = validate_email_unsigned(emails[i]);
        printf("Email '%s': %s\n", emails[i], valid ? "VALID" : "INVALID");  /* Warning */
    }

    /* VIOLATION: Credit card number validation */
    printf("\nCredit card validation:\n");

    signed char card_numbers[][20] = {
        "4111111111111111",  /* Visa test number */
        "5555555555554444",  /* MasterCard test number */
        "378282246310005",   /* American Express test number */
        "invalid123"
    };

    for (int i = 0; i < 4; i++) {
        printf("Validating card: %s\n", card_numbers[i]);  /* Warning */

        signed char *card = card_numbers[i];
        int length = strlen((char*)card);
        int digit_count = 0;

        /* VIOLATION: Digit validation with signed char */
        for (int j = 0; j < length; j++) {
            if (isdigit(card[j])) {  /* Potential undefined behavior */
                digit_count++;
            }
        }

        printf("  Length: %d, All digits: %s\n",
               length, (digit_count == length) ? "yes" : "no");

        /* Simple issuer detection based on first digit */
        if (length > 0 && isdigit(card[0])) {
            switch (card[0]) {
                case '4':
                    printf("  Issuer: Visa\n");
                    break;
                case '5':
                    printf("  Issuer: MasterCard\n");
                    break;
                case '3':
                    printf("  Issuer: American Express\n");
                    break;
                default:
                    printf("  Issuer: Unknown\n");
            }
        }
    }

    /* VIOLATION: Password strength validation */
    printf("\nPassword strength validation:\n");

    char passwords[][30] = {
        "password123",
        "StrongP@ssw0rd!",
        "weak",
        "Complex123!@#"
    };

    for (int i = 0; i < 4; i++) {
        printf("Checking password: %s\n", passwords[i]);

        unsigned char *pwd = (unsigned char*)passwords[i];
        int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
        int length = strlen((char*)pwd);

        /* VIOLATION: Character classification with type conversion */
        for (int j = 0; j < length; j++) {
            unsigned char c = pwd[j];

            if (isupper(c)) has_upper = 1;
            else if (islower(c)) has_lower = 1;
            else if (isdigit(c)) has_digit = 1;
            else if (ispunct(c)) has_special = 1;
        }

        int strength_score = has_upper + has_lower + has_digit + has_special;
        if (length >= 8) strength_score++;

        printf("  Length: %d, Score: %d/5\n", length, strength_score);
        printf("  Strength: ");
        if (strength_score >= 4) printf("Strong\n");
        else if (strength_score >= 2) printf("Medium\n");
        else printf("Weak\n");
    }

    /* VIOLATION: IPv4 address validation */
    printf("\nIPv4 address validation:\n");

    signed char ip_addresses[][20] = {
        "192.168.1.1",
        "10.0.0.255",
        "256.1.1.1",     /* Invalid */
        "192.168.1"      /* Incomplete */
    };

    for (int i = 0; i < 4; i++) {
        printf("Validating IP: %s\n", ip_addresses[i]);  /* Warning */

        signed char *ip_copy = malloc(strlen((char*)ip_addresses[i]) + 1);
        if (!ip_copy) continue;

        strcpy(ip_copy, ip_addresses[i]);  /* Warning */

        int octet_count = 0;
        int valid = 1;

        /* VIOLATION: Tokenization with signed char */
        signed char *octet = strtok(ip_copy, ".");  /* Warning */
        while (octet && octet_count < 4) {
            int value = atoi((char*)octet);
            if (value < 0 || value > 255) {
                valid = 0;
                break;
            }
            octet_count++;
            octet = strtok(NULL, ".");  /* Warning */
        }

        if (octet_count != 4) valid = 0;

        printf("  Valid: %s\n", valid ? "yes" : "no");
        free(ip_copy);
    }

    return 0;
}