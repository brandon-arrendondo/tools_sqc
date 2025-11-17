/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: configuration_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Configuration path modification */
void unsafe_config_path_modification(void) {
    char *config_dir = getenv("CONFIG_DIR");

    if (config_dir != NULL) {
        /* VIOLATION: Appending filename to config directory */
        strcat(config_dir, "/app.conf");  /* Undefined behavior */
        printf("Config file path: %s\n", config_dir);
    }
}

/* NON-COMPLIANT: Debug flag modification */
void unsafe_debug_flag_modification(void) {
    char *debug_level = getenv("DEBUG_LEVEL");

    if (debug_level != NULL) {
        /* VIOLATION: Incrementing debug level */
        if (debug_level[0] >= '0' && debug_level[0] < '9') {
            debug_level[0]++;  /* Undefined behavior */
        }
        printf("Incremented debug level: %s\n", debug_level);
    }
}

/* NON-COMPLIANT: URL modification */
void unsafe_url_modification(void) {
    char *base_url = getenv("API_URL");

    if (base_url != NULL) {
        /* VIOLATION: Appending endpoint */
        strcat(base_url, "/v1/users");  /* Undefined behavior */
        printf("Full API URL: %s\n", base_url);
    }
}

int main(void) {
    setenv("CONFIG_DIR", "/etc/myapp", 1);
    setenv("DEBUG_LEVEL", "2", 1);
    setenv("API_URL", "https://api.example.com", 1);

    unsafe_config_path_modification();
    unsafe_debug_flag_modification();
    unsafe_url_modification();
    return 0;
}