/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define SOCKET_PATH "/tmp/normal_communication_socket"

// Signal handler only for critical system events
void critical_handler(int sig) {
    if (sig == SIGTERM) {
        printf("CRITICAL: System shutdown signal received\n");
        unlink(SOCKET_PATH);
        exit(0);
    }
}

int main() {
    printf("Using Unix domain sockets for normal communication, signals only for critical events (GOOD)\n");

    // Set up signal handler only for critical system events
    signal(SIGTERM, critical_handler);

    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[256];

    // Remove existing socket file
    unlink(SOCKET_PATH);

    // Create server socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, 1) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process - client
        close(server_sock);
        sleep(1);  // Give server time to start

        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("client socket");
            exit(EXIT_FAILURE);
        }

        // Connect to server
        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            exit(EXIT_FAILURE);
        }

        printf("Client: Connected to server via socket\n");

        // Send data ready message
        strcpy(buffer, "DATA_READY");
        write(sock, buffer, strlen(buffer) + 1);
        printf("Client: Sent DATA_READY message\n");

        sleep(2);

        // Send completion message
        strcpy(buffer, "PROCESS_COMPLETE");
        write(sock, buffer, strlen(buffer) + 1);
        printf("Client: Sent PROCESS_COMPLETE message\n");

        close(sock);
        exit(0);
    } else {
        // Parent process - server
        printf("Server: Waiting for client connection\n");

        client_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Server: Client connected\n");

        // Receive data ready message
        if (read(client_sock, buffer, sizeof(buffer)) > 0) {
            printf("Server: Received message: %s\n", buffer);
            if (strcmp(buffer, "DATA_READY") == 0) {
                printf("Server: Processing data...\n");
                sleep(1);
            }
        }

        // Receive completion message
        if (read(client_sock, buffer, sizeof(buffer)) > 0) {
            printf("Server: Received message: %s\n", buffer);
            if (strcmp(buffer, "PROCESS_COMPLETE") == 0) {
                printf("Server: All processing complete\n");
            }
        }

        close(client_sock);
        close(server_sock);
        wait(NULL);

        // Cleanup
        unlink(SOCKET_PATH);
        printf("Socket communication completed successfully\n");
    }

    return 0;
}