/*
 * Rule: POS02-C
 * Source: testcases
 * Status: FAIL - Should trigger POS02-C violation
 *
 * Privileged bind without dropping privileges before fork
 */

#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    struct sockaddr_in sa;
    int s;

    sa.sin_port = htons(8080);

    /* VIOLATION: bind + fork without dropping privileges */
    bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in));

    switch (fork()) {
        case -1: break;
        case 0:  break;
        default: break;
    }
    return 0;
}
