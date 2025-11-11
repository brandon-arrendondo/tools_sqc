/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: configuration_values.c
 *
 * This case demonstrates violations where configuration values
 * that remain constant throughout execution are not const-qualified.
 */

#include <stdio.h>
#include <stdbool.h>

void server_configuration(void) {
    /* NON-COMPLIANT: Configuration values should be const */
    int port_number = 8080;
    char server_name[] = "localhost";
    int max_connections = 100;
    int timeout_seconds = 30;
    
    printf("Server Configuration:\n");
    printf("  Name: %s\n", server_name);
    printf("  Port: %d\n", port_number);
    printf("  Max connections: %d\n", max_connections);
    printf("  Timeout: %d seconds\n", timeout_seconds);
    
    /* These values are never modified after initialization */
    for (int i = 0; i < max_connections; i++) {
        /* Simulate connection handling */
        if (i % 10 == 0) {
            printf("Handled %d connections (port %d)\n", i, port_number);
        }
    }
}

void application_settings(void) {
    /* NON-COMPLIANT: Application settings should be const */
    bool debug_mode = false;
    bool verbose_logging = true;
    char log_file[] = "/var/log/app.log";
    int log_level = 3;  /* 0=ERROR, 1=WARN, 2=INFO, 3=DEBUG */
    
    printf("\nApplication Settings:\n");
    printf("  Debug mode: %s\n", debug_mode ? "ON" : "OFF");
    printf("  Verbose logging: %s\n", verbose_logging ? "ON" : "OFF");
    printf("  Log file: %s\n", log_file);
    printf("  Log level: %d\n", log_level);
    
    /* Settings are read but never modified */
    if (verbose_logging && log_level >= 3) {
        printf("Verbose debug logging enabled to %s\n", log_file);
    }
}

int main(void) {
    /* NON-COMPLIANT: Build information should be const */
    char build_version[] = "2.1.0";
    int build_number = 1234;
    char build_date[] = "2024-01-01";
    
    printf("Build Information:\n");
    printf("  Version: %s\n", build_version);
    printf("  Build: %d\n", build_number);
    printf("  Date: %s\n", build_date);
    
    server_configuration();
    application_settings();
    
    return 0;
}