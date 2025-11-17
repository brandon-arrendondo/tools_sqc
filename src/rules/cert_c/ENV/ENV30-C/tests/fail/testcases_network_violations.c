/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: network_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Hostname modification */
void unsafe_hostname_modification(void) {
    char *hostname = getenv("HOSTNAME");
    if (hostname) {
        /* VIOLATION: Adding domain suffix */
        strcat(hostname, ".local");  /* Undefined behavior */
        printf("FQDN: %s\n", hostname);
    }
}

/* NON-COMPLIANT: Port modification */
void unsafe_port_modification(void) {
    char *port = getenv("PORT");
    if (port) {
        /* VIOLATION: Incrementing port number */
        int port_num = atoi(port);
        sprintf(port, "%d", port_num + 1);  /* Undefined behavior */
        printf("New port: %s\n", port);
    }
}

int main(void) {
    setenv("HOSTNAME", "server", 1);
    setenv("PORT", "8080", 1);

    unsafe_hostname_modification();
    unsafe_port_modification();
    return 0;
}