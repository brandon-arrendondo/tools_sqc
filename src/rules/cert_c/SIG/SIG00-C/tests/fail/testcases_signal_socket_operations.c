/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

int server_socket = -1;
volatile sig_atomic_t connection_count = 0;

void socket_handler(int sig) {
    connection_count++;
    char response[512];

    printf("Handler: Signal %d, simulating network response\n", sig);

    if (server_socket == -1) {
        printf("Handler: No socket available\n");
        return;
    }

    // Violation: Socket operations without proper signal masking
    // can cause incomplete I/O and connection corruption
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: 50\r\n"
             "\r\n"
             "Signal %d response #%d - This is a test message\r\n",
             sig, connection_count);

    // Simulate sending response (vulnerable to interruption)
    size_t total_length = strlen(response);
    size_t bytes_sent = 0;

    while (bytes_sent < total_length) {
        ssize_t result = send(server_socket, response + bytes_sent,
                             total_length - bytes_sent, MSG_NOSIGNAL);

        if (result == -1) {
            if (errno == EINTR) {
                printf("Handler: Send interrupted by signal\n");
                continue;
            } else if (errno == EPIPE || errno == ECONNRESET) {
                printf("Handler: Connection closed\n");
                return;
            } else {
                perror("Handler: send failed");
                return;
            }
        }

        bytes_sent += result;

        // Create vulnerability window
        usleep(5000);

        printf("Handler: Sent %zu/%zu bytes\n", bytes_sent, total_length);
    }

    printf("Handler: Response sent completely\n");
}

int main() {
    struct sigaction sa;
    struct sockaddr_in server_addr;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(0); // Let system choose port

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Get the assigned port
    socklen_t addr_len = sizeof(server_addr);
    getsockname(server_socket, (struct sockaddr*)&server_addr, &addr_len);

    printf("Socket bound to port %d\n", ntohs(server_addr.sin_port));

    // Install handler without masking
    sa.sa_handler = socket_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Socket I/O can be interrupted and corrupted
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger socket operations in handler\n");
    printf("Socket operations may be interrupted and corrupted\n");

    while (1) {
        printf("Main: Simulating network activity, connections: %d\n",
               connection_count);

        // Simulate some socket activity in main thread
        char test_data[] = "Main thread socket test";
        send(server_socket, test_data, strlen(test_data), MSG_NOSIGNAL);

        sleep(3);
    }

    close(server_socket);
    return 0;
}