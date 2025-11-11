/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int socket_fd;
    char remote_host[64];
    int remote_port;
    int connection_state;  /* 0=disconnected, 1=connecting, 2=connected */
    int bytes_sent;
    int bytes_received;
    char last_error[128];
    double connection_time;
} network_connection_t;

typedef struct {
    network_connection_t connections[5];
    int active_connections;
    int total_connections_made;
    int connection_failures;
    char network_interface[32];
    int bandwidth_usage;
} network_state_t;

network_state_t global_network_state = {0};

void init_connection(network_connection_t *conn, int index) {
    conn->socket_fd = -1;
    sprintf(conn->remote_host, "server%d.example.com", index);
    conn->remote_port = 8080 + index;
    conn->connection_state = 0;
    conn->bytes_sent = 0;
    conn->bytes_received = 0;
    strcpy(conn->last_error, "No error");
    conn->connection_time = 0.0;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing global network connection state in signal handler */

    if (sig == SIGUSR1) {
        /* Emergency disconnect all connections */
        for (int i = 0; i < 5; i++) {
            network_connection_t *conn = &global_network_state.connections[i];
            if (conn->connection_state == 2) {  /* Connected */
                conn->connection_state = 0;     /* Disconnect */
                sprintf(conn->last_error, "Emergency disconnect by signal %d", sig);
                if (conn->socket_fd >= 0) {
                    /* In real code, this would be close(conn->socket_fd) */
                    conn->socket_fd = -1;
                }
                global_network_state.active_connections--;
            }
        }
        strcpy(global_network_state.network_interface, "emergency_mode");
    } else if (sig == SIGUSR2) {
        /* Update network statistics */
        global_network_state.connection_failures += sig % 3;
        global_network_state.bandwidth_usage += 1024;

        for (int i = 0; i < 5; i++) {
            network_connection_t *conn = &global_network_state.connections[i];
            if (conn->connection_state == 2) {
                conn->bytes_sent += 256;
                conn->bytes_received += 128;
                conn->connection_time += 0.1;
            }
        }
    }

    printf("Handler: active=%d, total=%d, failures=%d, bandwidth=%d, interface=%s\n",
           global_network_state.active_connections,
           global_network_state.total_connections_made,
           global_network_state.connection_failures,
           global_network_state.bandwidth_usage,
           global_network_state.network_interface);
}

int main() {
    printf("Demonstrating unsafe network connection state access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize network state */
    strcpy(global_network_state.network_interface, "eth0");
    global_network_state.active_connections = 0;
    global_network_state.total_connections_made = 0;
    global_network_state.connection_failures = 0;
    global_network_state.bandwidth_usage = 0;

    for (int i = 0; i < 5; i++) {
        init_connection(&global_network_state.connections[i], i);
    }

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        int conn_index = i % 5;
        network_connection_t *conn = &global_network_state.connections[conn_index];

        /* Simulate connection state changes */
        switch (i % 4) {
            case 0:  /* Connecting */
                if (conn->connection_state == 0) {
                    conn->connection_state = 1;
                    sprintf(conn->last_error, "Connecting to %s:%d", conn->remote_host, conn->remote_port);
                }
                break;
            case 1:  /* Connected */
                if (conn->connection_state == 1) {
                    conn->connection_state = 2;
                    conn->socket_fd = 100 + conn_index;  /* Simulated fd */
                    strcpy(conn->last_error, "Connected successfully");
                    global_network_state.active_connections++;
                    global_network_state.total_connections_made++;
                }
                break;
            case 2:  /* Data transfer */
                if (conn->connection_state == 2) {
                    conn->bytes_sent += 512;
                    conn->bytes_received += 256;
                    conn->connection_time += 0.05;
                    global_network_state.bandwidth_usage += 768;
                }
                break;
            case 3:  /* Disconnect */
                if (conn->connection_state == 2) {
                    conn->connection_state = 0;
                    conn->socket_fd = -1;
                    strcpy(conn->last_error, "Normal disconnect");
                    global_network_state.active_connections--;
                }
                break;
        }

        printf("Main: active=%d, total=%d, failures=%d, conn%d_state=%d, sent=%d\n",
               global_network_state.active_connections,
               global_network_state.total_connections_made,
               global_network_state.connection_failures,
               conn_index, conn->connection_state, conn->bytes_sent);

        usleep(100000);
    }

    return 0;
}