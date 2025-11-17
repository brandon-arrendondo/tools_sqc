/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

void network_handler(int sig) {
    // VIOLATION: socket() is not async-safe
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != -1) {
        close(sockfd);
    }

    // VIOLATION: getaddrinfo() is not async-safe
    struct addrinfo hints, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo("localhost", "80", &hints, &result);
    if (status == 0) {
        freeaddrinfo(result);  // Also not async-safe
    }

    // VIOLATION: gethostbyname() is not async-safe
    struct hostent *host = gethostbyname("localhost");

    // VIOLATION: inet_ntoa() is not async-safe
    struct in_addr addr;
    addr.s_addr = inet_addr("127.0.0.1");
    char *ip_str = inet_ntoa(addr);
}

int main() {
    printf("Demonstrating unsafe network functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, network_handler);

    printf("Send SIGUSR1 to trigger unsafe network operations\n");

    while (1) {
        pause();
    }

    return 0;
}