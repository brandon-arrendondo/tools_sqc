/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_arrays_structures.c
 *
 * This case demonstrates compliant code that properly const-qualifies
 * arrays and structures that are not modified after initialization.
 */

#include <stdio.h>
#include <string.h>

/* COMPLIANT: Global const arrays */
const int FIBONACCI_SEQUENCE[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144};
const double TEMPERATURE_CONVERSIONS[] = {1.8, 32.0, 273.15, 459.67};  /* C to F factors, K offset, R offset */

/* COMPLIANT: Const structure definition */
struct Config {
    const char *name;
    const int value;
    const double factor;
};

/* COMPLIANT: Const array of structures */
const struct Config SYSTEM_CONFIGS[] = {
    {"buffer_size", 4096, 1.0},
    {"max_connections", 100, 1.0},
    {"timeout_ms", 5000, 1.0},
    {"retry_count", 3, 1.0},
    {"cache_size", 1048576, 1.024}  /* 1MB in bytes, KB factor */
};

void demonstrate_const_arrays(void) {
    /* COMPLIANT: Local const arrays that are never modified */
    const char VOWELS[] = {'a', 'e', 'i', 'o', 'u'};
    const int PERFECT_SQUARES[] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};
    const char * const DAY_ABBREVIATIONS[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    printf("Constant Arrays Demonstration:\\n");

    printf("  Vowels: ");
    for (size_t i = 0; i < sizeof(VOWELS)/sizeof(VOWELS[0]); i++) {
        printf("%c ", VOWELS[i]);
    }
    printf("\\n");

    printf("  Perfect squares: ");
    for (size_t i = 0; i < sizeof(PERFECT_SQUARES)/sizeof(PERFECT_SQUARES[0]); i++) {
        printf("%d ", PERFECT_SQUARES[i]);
    }
    printf("\\n");

    printf("  Day abbreviations: ");
    for (size_t i = 0; i < sizeof(DAY_ABBREVIATIONS)/sizeof(DAY_ABBREVIATIONS[0]); i++) {
        printf("%s ", DAY_ABBREVIATIONS[i]);
    }
    printf("\\n");

    /* COMPLIANT: Using const arrays for validation */
    char test_char = 'e';
    int is_vowel = 0;
    for (size_t i = 0; i < sizeof(VOWELS)/sizeof(VOWELS[0]); i++) {
        if (test_char == VOWELS[i]) {
            is_vowel = 1;
            break;
        }
    }
    printf("  '%c' is %sa vowel\\n", test_char, is_vowel ? "" : "not ");
}

void demonstrate_const_structures(void) {
    /* COMPLIANT: Local const structures */
    const struct {
        const char *protocol;
        const int default_port;
        const int secure;
    } NETWORK_PROTOCOLS[] = {
        {"HTTP", 80, 0},
        {"HTTPS", 443, 1},
        {"FTP", 21, 0},
        {"FTPS", 990, 1},
        {"SSH", 22, 1},
        {"TELNET", 23, 0}
    };

    /* COMPLIANT: Const structure with array member */
    const struct {
        const char *name;
        const unsigned char rgb[3];
        const unsigned int hex;
    } STANDARD_COLORS[] = {
        {"Red", {255, 0, 0}, 0xFF0000},
        {"Green", {0, 255, 0}, 0x00FF00},
        {"Blue", {0, 0, 255}, 0x0000FF},
        {"Yellow", {255, 255, 0}, 0xFFFF00},
        {"Cyan", {0, 255, 255}, 0x00FFFF},
        {"Magenta", {255, 0, 255}, 0xFF00FF},
        {"White", {255, 255, 255}, 0xFFFFFF},
        {"Black", {0, 0, 0}, 0x000000}
    };

    printf("\\nConstant Structures Demonstration:\\n");

    printf("  Network Protocols:\\n");
    for (size_t i = 0; i < sizeof(NETWORK_PROTOCOLS)/sizeof(NETWORK_PROTOCOLS[0]); i++) {
        printf("    %s: port %d (%s)\\n",
               NETWORK_PROTOCOLS[i].protocol,
               NETWORK_PROTOCOLS[i].default_port,
               NETWORK_PROTOCOLS[i].secure ? "secure" : "insecure");
    }

    printf("  Standard Colors:\\n");
    for (size_t i = 0; i < sizeof(STANDARD_COLORS)/sizeof(STANDARD_COLORS[0]); i++) {
        printf("    %-8s: RGB(%3d,%3d,%3d) = 0x%06X\\n",
               STANDARD_COLORS[i].name,
               STANDARD_COLORS[i].rgb[0],
               STANDARD_COLORS[i].rgb[1],
               STANDARD_COLORS[i].rgb[2],
               STANDARD_COLORS[i].hex);
    }
}

void demonstrate_function_parameters(void) {
    /* COMPLIANT: Function using const parameters */
    auto void print_array(const int *arr, const size_t size, const char *label) {
        printf("  %s: ", label);
        for (size_t i = 0; i < size; i++) {
            printf("%d ", arr[i]);
        }
        printf("\\n");
    }

    auto int find_in_array(const int *arr, const size_t size, const int target) {
        for (size_t i = 0; i < size; i++) {
            if (arr[i] == target) {
                return (int)i;
            }
        }
        return -1;
    }

    printf("\\nConstant Function Parameters:\\n");

    /* Using const arrays with const parameters */
    const int SAMPLE_DATA[] = {10, 20, 30, 40, 50};
    const size_t DATA_SIZE = sizeof(SAMPLE_DATA) / sizeof(SAMPLE_DATA[0]);

    print_array(SAMPLE_DATA, DATA_SIZE, "Sample data");

    const int SEARCH_VALUE = 30;
    int position = find_in_array(SAMPLE_DATA, DATA_SIZE, SEARCH_VALUE);
    printf("  Value %d found at position: %d\\n", SEARCH_VALUE, position);
}

void demonstrate_global_const_usage(void) {
    printf("\\nGlobal Constant Usage:\\n");

    printf("  Fibonacci sequence: ");
    for (size_t i = 0; i < sizeof(FIBONACCI_SEQUENCE)/sizeof(FIBONACCI_SEQUENCE[0]); i++) {
        printf("%d ", FIBONACCI_SEQUENCE[i]);
    }
    printf("\\n");

    printf("  System configurations:\\n");
    for (size_t i = 0; i < sizeof(SYSTEM_CONFIGS)/sizeof(SYSTEM_CONFIGS[0]); i++) {
        printf("    %s: %d (factor: %.3f)\\n",
               SYSTEM_CONFIGS[i].name,
               SYSTEM_CONFIGS[i].value,
               SYSTEM_CONFIGS[i].factor);
    }

    /* COMPLIANT: Using global const in calculations */
    const double celsius = 25.0;
    const double fahrenheit = celsius * TEMPERATURE_CONVERSIONS[0] + TEMPERATURE_CONVERSIONS[1];
    const double kelvin = celsius + TEMPERATURE_CONVERSIONS[2];

    printf("  Temperature conversions:\\n");
    printf("    %.1f°C = %.1f°F = %.1fK\\n", celsius, fahrenheit, kelvin);
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_NAME = "Const Arrays and Structures Demo";
    const char * const VERSION = "1.0";

    printf("=== %s v%s ===\\n\\n", PROGRAM_NAME, VERSION);

    demonstrate_const_arrays();
    demonstrate_const_structures();
    demonstrate_function_parameters();
    demonstrate_global_const_usage();

    printf("\\n=== Demo completed ===\\n");
    return 0;
}