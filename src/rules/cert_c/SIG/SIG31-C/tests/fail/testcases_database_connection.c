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

typedef struct {
    int connection_id;
    char host[64];
    int port;
    int is_connected;
    int transaction_count;
    char last_query[256];
    double connection_time;
} db_connection_t;

typedef struct {
    db_connection_t connections[10];
    int active_connections;
    int total_queries;
    int failed_queries;
    char database_name[64];
    int connection_pool_size;
} db_state_t;

db_state_t global_db_state = {0};

void init_db_connection(db_connection_t *conn, int id) {
    conn->connection_id = id;
    sprintf(conn->host, "db-server-%d", id);
    conn->port = 5432 + id;
    conn->is_connected = 1;
    conn->transaction_count = 0;
    strcpy(conn->last_query, "CONNECT");
    conn->connection_time = 0.0;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing global database connections and handles in signal handler */

    /* Emergency database operations in signal handler - very dangerous */
    if (sig == SIGUSR1) {
        /* Force close all connections */
        for (int i = 0; i < global_db_state.connection_pool_size; i++) {
            if (global_db_state.connections[i].is_connected) {
                global_db_state.connections[i].is_connected = 0;
                strcpy(global_db_state.connections[i].last_query, "EMERGENCY_CLOSE");
                global_db_state.active_connections--;
            }
        }
        strcpy(global_db_state.database_name, "emergency_mode");
    } else if (sig == SIGUSR2) {
        /* Try to execute emergency query */
        for (int i = 0; i < global_db_state.connection_pool_size; i++) {
            if (global_db_state.connections[i].is_connected) {
                global_db_state.connections[i].transaction_count++;
                sprintf(global_db_state.connections[i].last_query,
                        "EMERGENCY_QUERY_SIG_%d", sig);
                global_db_state.total_queries++;
            }
        }
    }

    global_db_state.failed_queries += sig % 3;

    printf("Handler: active_conn=%d, total_queries=%d, failed=%d, db=%s\n",
           global_db_state.active_connections, global_db_state.total_queries,
           global_db_state.failed_queries, global_db_state.database_name);
}

int main() {
    printf("Demonstrating unsafe database connection access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize database state */
    strcpy(global_db_state.database_name, "production_db");
    global_db_state.connection_pool_size = 5;
    global_db_state.active_connections = 5;

    for (int i = 0; i < global_db_state.connection_pool_size; i++) {
        init_db_connection(&global_db_state.connections[i], i);
    }

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Simulate database operations */
        int conn_index = i % global_db_state.connection_pool_size;
        db_connection_t *conn = &global_db_state.connections[conn_index];

        if (conn->is_connected) {
            conn->transaction_count++;
            sprintf(conn->last_query, "SELECT * FROM table_%d WHERE id = %d", conn_index, i);
            conn->connection_time += 0.05;
            global_db_state.total_queries++;
        } else {
            global_db_state.failed_queries++;
        }

        /* Reconnection logic */
        if (i % 8 == 7) {
            for (int j = 0; j < global_db_state.connection_pool_size; j++) {
                if (!global_db_state.connections[j].is_connected) {
                    init_db_connection(&global_db_state.connections[j], j);
                    global_db_state.active_connections++;
                }
            }
        }

        printf("Main: active_conn=%d, total_queries=%d, failed=%d, conn%d_trans=%d\n",
               global_db_state.active_connections, global_db_state.total_queries,
               global_db_state.failed_queries, conn_index, conn->transaction_count);

        usleep(120000);
    }

    return 0;
}