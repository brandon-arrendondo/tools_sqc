/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_global_constants.c
 *
 * This case demonstrates compliant code that properly const-qualifies
 * global constants, configuration data, and shared immutable resources.
 */

#include <stdio.h>
#include <string.h>

/* COMPLIANT: Global const mathematical constants */
const double MATH_PI = 3.141592653589793;
const double MATH_E = 2.718281828459045;
const double MATH_SQRT_2 = 1.414213562373095;
const double MATH_GOLDEN_RATIO = 1.618033988749895;

/* COMPLIANT: Global const physical constants */
const double SPEED_OF_LIGHT = 299792458.0;        /* m/s */
const double GRAVITATIONAL_CONSTANT = 6.674e-11;  /* m³/kg⋅s² */
const double PLANCK_CONSTANT = 6.62607015e-34;    /* J⋅s */
const double AVOGADRO_NUMBER = 6.02214076e23;     /* mol⁻¹ */

/* COMPLIANT: Global const configuration */
const int DEFAULT_BUFFER_SIZE = 4096;
const int MAX_FILENAME_LENGTH = 255;
const int MAX_PATH_LENGTH = 4096;
const int DEFAULT_TIMEOUT_MS = 5000;

/* COMPLIANT: Global const string arrays */
const char * const ERROR_LEVEL_NAMES[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

const char * const HTTP_METHOD_NAMES[] = {
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "PATCH",
    "HEAD",
    "OPTIONS"
};

/* COMPLIANT: Global const lookup tables */
const int DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char * const MONTH_NAMES[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

/* COMPLIANT: Global const version information */
const char * const APPLICATION_NAME = "Global Constants Demo";
const char * const APPLICATION_VERSION = "2.1.0";
const char * const BUILD_DATE = __DATE__;
const char * const BUILD_TIME = __TIME__;
const int VERSION_MAJOR = 2;
const int VERSION_MINOR = 1;
const int VERSION_PATCH = 0;

/* COMPLIANT: Global const protocol definitions */
const int TCP_PROTOCOL = 6;
const int UDP_PROTOCOL = 17;
const int ICMP_PROTOCOL = 1;

const int HTTP_PORT = 80;
const int HTTPS_PORT = 443;
const int FTP_PORT = 21;
const int SSH_PORT = 22;
const int TELNET_PORT = 23;
const int SMTP_PORT = 25;
const int DNS_PORT = 53;

/* COMPLIANT: Global const color definitions */
const unsigned int COLOR_RED = 0xFF0000;
const unsigned int COLOR_GREEN = 0x00FF00;
const unsigned int COLOR_BLUE = 0x0000FF;
const unsigned int COLOR_WHITE = 0xFFFFFF;
const unsigned int COLOR_BLACK = 0x000000;
const unsigned int COLOR_YELLOW = 0xFFFF00;
const unsigned int COLOR_CYAN = 0x00FFFF;
const unsigned int COLOR_MAGENTA = 0xFF00FF;

void demonstrate_mathematical_constants(void) {
    printf("Mathematical Constants:\\n");
    printf("  π = %.15f\\n", MATH_PI);
    printf("  e = %.15f\\n", MATH_E);
    printf("  √2 = %.15f\\n", MATH_SQRT_2);
    printf("  φ (Golden Ratio) = %.15f\\n", MATH_GOLDEN_RATIO);

    /* Using global const in calculations */
    const double radius = 10.0;
    const double area = MATH_PI * radius * radius;
    const double circumference = 2.0 * MATH_PI * radius;

    printf("\\nCircle calculations (radius = %.1f):\\n", radius);
    printf("  Area: %.2f\\n", area);
    printf("  Circumference: %.2f\\n", circumference);
}

void demonstrate_physical_constants(void) {
    printf("\\nPhysical Constants:\\n");
    printf("  Speed of light: %.0f m/s\\n", SPEED_OF_LIGHT);
    printf("  Gravitational constant: %.3e m³/kg⋅s²\\n", GRAVITATIONAL_CONSTANT);
    printf("  Planck constant: %.3e J⋅s\\n", PLANCK_CONSTANT);
    printf("  Avogadro number: %.3e mol⁻¹\\n", AVOGADRO_NUMBER);

    /* Using physical constants in calculations */
    const double mass = 2.0;  /* kg */
    const double energy = mass * SPEED_OF_LIGHT * SPEED_OF_LIGHT;

    printf("\\nE=mc² calculation:\\n");
    printf("  Mass: %.1f kg\\n", mass);
    printf("  Energy: %.3e J\\n", energy);
}

void demonstrate_configuration_constants(void) {
    printf("\\nConfiguration Constants:\\n");
    printf("  Default buffer size: %d bytes\\n", DEFAULT_BUFFER_SIZE);
    printf("  Max filename length: %d chars\\n", MAX_FILENAME_LENGTH);
    printf("  Max path length: %d chars\\n", MAX_PATH_LENGTH);
    printf("  Default timeout: %d ms\\n", DEFAULT_TIMEOUT_MS);

    /* Using configuration constants */
    char buffer[DEFAULT_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Allocated buffer of %d bytes", DEFAULT_BUFFER_SIZE);
    printf("  %s\\n", buffer);
}

void demonstrate_string_arrays(void) {
    printf("\\nString Array Constants:\\n");

    const size_t error_level_count = sizeof(ERROR_LEVEL_NAMES) / sizeof(ERROR_LEVEL_NAMES[0]);
    printf("  Error levels (%zu): ", error_level_count);
    for (size_t i = 0; i < error_level_count; i++) {
        printf("%s ", ERROR_LEVEL_NAMES[i]);
    }
    printf("\\n");

    const size_t http_method_count = sizeof(HTTP_METHOD_NAMES) / sizeof(HTTP_METHOD_NAMES[0]);
    printf("  HTTP methods (%zu): ", http_method_count);
    for (size_t i = 0; i < http_method_count; i++) {
        printf("%s ", HTTP_METHOD_NAMES[i]);
    }
    printf("\\n");
}

void demonstrate_lookup_tables(void) {
    printf("\\nLookup Table Constants:\\n");

    const size_t month_count = sizeof(MONTH_NAMES) / sizeof(MONTH_NAMES[0]);
    printf("  Calendar data:\\n");
    for (size_t i = 0; i < month_count; i++) {
        printf("    %s: %d days\\n", MONTH_NAMES[i], DAYS_IN_MONTH[i]);
    }

    /* Using lookup tables */
    const int current_month = 6;  /* July (0-indexed) */
    printf("\\n  Current month: %s (%d days)\\n",
           MONTH_NAMES[current_month], DAYS_IN_MONTH[current_month]);
}

void demonstrate_version_info(void) {
    printf("\\nVersion Information:\\n");
    printf("  Application: %s\\n", APPLICATION_NAME);
    printf("  Version: %s (%d.%d.%d)\\n", APPLICATION_VERSION,
           VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("  Built: %s %s\\n", BUILD_DATE, BUILD_TIME);

    /* Version comparison example */
    const int required_major = 2;
    const int required_minor = 0;

    if (VERSION_MAJOR > required_major ||
        (VERSION_MAJOR == required_major && VERSION_MINOR >= required_minor)) {
        printf("  Version requirement satisfied\\n");
    } else {
        printf("  Version requirement not met\\n");
    }
}

void demonstrate_protocol_constants(void) {
    printf("\\nNetwork Protocol Constants:\\n");
    printf("  Protocol numbers: TCP=%d, UDP=%d, ICMP=%d\\n",
           TCP_PROTOCOL, UDP_PROTOCOL, ICMP_PROTOCOL);

    printf("  Well-known ports:\\n");
    printf("    HTTP: %d, HTTPS: %d\\n", HTTP_PORT, HTTPS_PORT);
    printf("    FTP: %d, SSH: %d\\n", FTP_PORT, SSH_PORT);
    printf("    SMTP: %d, DNS: %d\\n", SMTP_PORT, DNS_PORT);

    /* Using protocol constants */
    const int service_port = HTTPS_PORT;
    printf("\\n  Service running on port %d (%s)\\n",
           service_port, (service_port == HTTPS_PORT) ? "HTTPS" : "Other");
}

void demonstrate_color_constants(void) {
    printf("\\nColor Constants:\\n");
    printf("  Primary colors:\\n");
    printf("    Red: 0x%06X\\n", COLOR_RED);
    printf("    Green: 0x%06X\\n", COLOR_GREEN);
    printf("    Blue: 0x%06X\\n", COLOR_BLUE);

    printf("  Composite colors:\\n");
    printf("    Yellow: 0x%06X\\n", COLOR_YELLOW);
    printf("    Cyan: 0x%06X\\n", COLOR_CYAN);
    printf("    Magenta: 0x%06X\\n", COLOR_MAGENTA);

    printf("  Monochrome:\\n");
    printf("    White: 0x%06X\\n", COLOR_WHITE);
    printf("    Black: 0x%06X\\n", COLOR_BLACK);

    /* Using color constants */
    const unsigned int background_color = COLOR_WHITE;
    const unsigned int text_color = COLOR_BLACK;
    printf("\\n  Display theme: Background=0x%06X, Text=0x%06X\\n",
           background_color, text_color);
}

int main(void) {
    /* COMPLIANT: Local const using global const */
    const char * const SEPARATOR = "========================================";

    printf("%s\\n", SEPARATOR);
    printf("%s v%s\\n", APPLICATION_NAME, APPLICATION_VERSION);
    printf("%s\\n", SEPARATOR);

    demonstrate_mathematical_constants();
    demonstrate_physical_constants();
    demonstrate_configuration_constants();
    demonstrate_string_arrays();
    demonstrate_lookup_tables();
    demonstrate_version_info();
    demonstrate_protocol_constants();
    demonstrate_color_constants();

    printf("\\n%s\\n", SEPARATOR);
    printf("All global constants demonstrated successfully\\n");
    printf("%s\\n", SEPARATOR);

    return 0;
}