/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_best_practices.c
 *
 * This case demonstrates best practices for using const qualification
 * effectively while avoiding const poisoning and maintaining readability.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: Const at module level for shared constants */
static const char * const MODULE_NAME = "ConstBestPractices";
static const int MODULE_VERSION = 1;

/* COMPLIANT: Const structure definitions */
struct Configuration {
    const char *server_host;
    const int server_port;
    const int timeout_ms;
    const int max_retries;
};

/* COMPLIANT: Function returning const data */
const char *get_error_message(int error_code) {
    /* COMPLIANT: Static const array for error messages */
    static const char * const error_messages[] = {
        "Success",
        "Invalid parameter",
        "Out of memory",
        "File not found",
        "Network error",
        "Unknown error"
    };
    static const size_t error_count = sizeof(error_messages) / sizeof(error_messages[0]);

    if (error_code >= 0 && (size_t)error_code < error_count) {
        return error_messages[error_code];
    }
    return error_messages[error_count - 1];  /* "Unknown error" */
}

/* COMPLIANT: Progressive const application */
void demonstrate_progressive_const(void) {
    printf("Progressive Const Application:\\n");

    /* Step 1: Start with basic const for obvious constants */
    const double TAX_RATE = 0.08;
    const char * const CURRENCY_SYMBOL = "$";

    /* Step 2: Add const to function parameters that shouldn't change */
    auto void print_price(const double price, const char *item_name) {
        const double total_price = price * (1.0 + TAX_RATE);
        printf("  %s: %s%.2f (includes tax)\\n", item_name, CURRENCY_SYMBOL, total_price);
    }

    /* Step 3: Use const for local variables that don't change */
    const double base_prices[] = {19.99, 29.99, 39.99};
    const char * const item_names[] = {"Widget A", "Widget B", "Widget C"};
    const size_t item_count = sizeof(base_prices) / sizeof(base_prices[0]);

    for (size_t i = 0; i < item_count; i++) {
        print_price(base_prices[i], item_names[i]);
    }
}

/* COMPLIANT: Avoiding const poisoning through careful design */
void demonstrate_const_poisoning_avoidance(void) {
    printf("\\nAvoiding Const Poisoning:\\n");

    /* Strategy 1: Use const only where it adds value */
    const char source_text[] = "Hello World";

    /* Create a modifiable copy when needed */
    char *modifiable_text = malloc(strlen(source_text) + 1);
    if (modifiable_text) {
        strcpy(modifiable_text, source_text);

        printf("  Original (const): %s\\n", source_text);
        printf("  Copy (modifiable): %s\\n", modifiable_text);

        /* Modify the copy */
        for (char *p = modifiable_text; *p; p++) {
            if (*p >= 'a' && *p <= 'z') {
                *p = *p - 'a' + 'A';  /* Convert to uppercase */
            }
        }

        printf("  Modified copy: %s\\n", modifiable_text);
        free(modifiable_text);
    }

    /* Strategy 2: Use wrapper functions to hide const complexity */
    auto const char *safe_string_access(const char *str, size_t index) {
        static const char empty_char = '\\0';
        if (!str || index >= strlen(str)) {
            return &empty_char;
        }
        return &str[index];
    }

    const char *test_string = "Testing";
    printf("  Character at index 2: '%c'\\n", *safe_string_access(test_string, 2));
    printf("  Character at index 99: '%c'\\n", *safe_string_access(test_string, 99));
}

/* COMPLIANT: Const with complex data structures */
void demonstrate_const_with_structures(void) {
    printf("\\nConst with Complex Data Structures:\\n");

    /* COMPLIANT: Const structure initialization */
    const struct Configuration default_config = {
        .server_host = "localhost",
        .server_port = 8080,
        .timeout_ms = 5000,
        .max_retries = 3
    };

    /* COMPLIANT: Array of const structures */
    const struct Configuration environments[] = {
        {"dev.example.com", 8080, 1000, 1},
        {"test.example.com", 8080, 2000, 2},
        {"prod.example.com", 443, 5000, 5}
    };
    const size_t env_count = sizeof(environments) / sizeof(environments[0]);

    printf("  Default configuration:\\n");
    printf("    Host: %s\\n", default_config.server_host);
    printf("    Port: %d\\n", default_config.server_port);
    printf("    Timeout: %d ms\\n", default_config.timeout_ms);
    printf("    Max retries: %d\\n", default_config.max_retries);

    printf("\\n  Environment configurations:\\n");
    for (size_t i = 0; i < env_count; i++) {
        printf("    Env %zu: %s:%d (timeout: %dms, retries: %d)\\n",
               i, environments[i].server_host, environments[i].server_port,
               environments[i].timeout_ms, environments[i].max_retries);
    }
}

/* COMPLIANT: Const in API design */
typedef struct {
    const char *name;
    const double (*calculate)(const double input);
    const char *description;
} MathFunction;

/* Mathematical functions with const signatures */
const double square_function(const double x) {
    return x * x;
}

const double cube_function(const double x) {
    return x * x * x;
}

const double sqrt_function(const double x) {
    return (x >= 0.0) ? sqrt(x) : 0.0;
}

void demonstrate_const_api_design(void) {
    printf("\\nConst in API Design:\\n");

    /* COMPLIANT: Const function table */
    const MathFunction math_functions[] = {
        {"square", square_function, "Calculate x²"},
        {"cube", cube_function, "Calculate x³"},
        {"sqrt", sqrt_function, "Calculate √x"}
    };
    const size_t function_count = sizeof(math_functions) / sizeof(math_functions[0]);

    const double test_value = 4.0;

    printf("  Mathematical functions for input %.1f:\\n", test_value);
    for (size_t i = 0; i < function_count; i++) {
        const double result = math_functions[i].calculate(test_value);
        printf("    %s: %.3f (%s)\\n",
               math_functions[i].name, result, math_functions[i].description);
    }
}

/* COMPLIANT: Const for immutable state validation */
void demonstrate_const_validation(void) {
    printf("\\nConst for Validation:\\n");

    /* COMPLIANT: Validation rules as const */
    const struct {
        const char *field_name;
        const int min_length;
        const int max_length;
        const char *pattern_description;
    } validation_rules[] = {
        {"username", 3, 32, "alphanumeric and underscore only"},
        {"password", 8, 128, "mixed case, numbers, and symbols"},
        {"email", 5, 254, "valid email format"},
        {"phone", 10, 15, "digits and optional formatting"}
    };
    const size_t rule_count = sizeof(validation_rules) / sizeof(validation_rules[0]);

    printf("  Validation rules:\\n");
    for (size_t i = 0; i < rule_count; i++) {
        printf("    %s: %d-%d chars (%s)\\n",
               validation_rules[i].field_name,
               validation_rules[i].min_length,
               validation_rules[i].max_length,
               validation_rules[i].pattern_description);
    }

    /* Example validation using const rules */
    const char *test_username = "user123";
    const size_t username_len = strlen(test_username);
    const struct {
        const char *field_name;
        const int min_length;
        const int max_length;
        const char *pattern_description;
    } *username_rule = &validation_rules[0];

    printf("\\n  Validating username '%s':\\n", test_username);
    if (username_len >= (size_t)username_rule->min_length &&
        username_len <= (size_t)username_rule->max_length) {
        printf("    Length validation: PASS\\n");
    } else {
        printf("    Length validation: FAIL\\n");
    }
}

/* COMPLIANT: Error handling with const */
void demonstrate_const_error_handling(void) {
    printf("\\nConst Error Handling:\\n");

    /* Test different error scenarios */
    const int test_error_codes[] = {0, 1, 2, 3, 4, 99};
    const size_t test_count = sizeof(test_error_codes) / sizeof(test_error_codes[0]);

    printf("  Error code translations:\\n");
    for (size_t i = 0; i < test_count; i++) {
        const int error_code = test_error_codes[i];
        const char *error_message = get_error_message(error_code);
        printf("    Code %d: %s\\n", error_code, error_message);
    }
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_TITLE = "Const Best Practices Demonstration";
    const char * const VERSION_INFO = "Module: %s v%d";

    printf("=== %s ===\\n", PROGRAM_TITLE);
    printf(VERSION_INFO, MODULE_NAME, MODULE_VERSION);
    printf("\\n\\n");

    demonstrate_progressive_const();
    demonstrate_const_poisoning_avoidance();
    demonstrate_const_with_structures();
    demonstrate_const_api_design();
    demonstrate_const_validation();
    demonstrate_const_error_handling();

    printf("\\n=== Best practices demonstration completed ===\\n");

    return 0;
}