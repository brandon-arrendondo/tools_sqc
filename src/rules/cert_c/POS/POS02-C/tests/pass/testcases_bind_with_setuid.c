/*
 * Rule: POS02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS02-C violation
 *
 * Privileged operation followed by privilege drop
 */

#include <sys/socket.h>
#include <unistd.h>

void start_server_safe(int sockfd, struct sockaddr *addr, uid_t uid, gid_t gid) {
    bind(sockfd, addr, sizeof(*addr));
    listen(sockfd, 10);
    /* COMPLIANT: privileges dropped after bind */
    setgid(gid);
    setuid(uid);
}
