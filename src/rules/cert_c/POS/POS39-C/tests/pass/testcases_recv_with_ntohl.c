/*
 * Rule: POS39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS39-C violation
 *
 * Network receive with proper byte order conversion
 */

#include <sys/socket.h>
#include <arpa/inet.h>

void receive_data_safe(int sockfd) {
    int net_value;
    recv(sockfd, &net_value, sizeof(net_value), 0);
    /* COMPLIANT: byte order conversion applied */
    int host_value = ntohl(net_value);
    if (host_value > 100) {
        /* ... */
    }
}
