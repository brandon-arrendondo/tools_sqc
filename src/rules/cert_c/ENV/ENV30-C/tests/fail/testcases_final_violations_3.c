/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: final_violations_3.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Plugin path modification */
void unsafe_plugin_path(void) {
    char *plugin_dir = getenv("PLUGIN_DIR");
    if (plugin_dir) {
        strcat(plugin_dir, "/extensions");  /* Undefined behavior */
        printf("Plugin path: %s\n", plugin_dir);
    }
}

/* NON-COMPLIANT: Service name modification */
void unsafe_service_name(void) {
    char *service = getenv("SERVICE_NAME");
    if (service) {
        strcat(service, "-prod");  /* Undefined behavior */
        printf("Production service: %s\n", service);
    }
}

int main(void) {
    setenv("PLUGIN_DIR", "/opt/plugins", 1);
    setenv("SERVICE_NAME", "webserver", 1);

    unsafe_plugin_path();
    unsafe_service_name();
    return 0;
}