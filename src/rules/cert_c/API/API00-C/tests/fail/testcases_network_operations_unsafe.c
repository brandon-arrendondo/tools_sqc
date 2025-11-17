/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: network_operations_unsafe.c
 *
 * This case demonstrates violations where network operation functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* NON-COMPLIANT: No validation of socket descriptor */
int send_data(int socket_fd, const char *data, size_t length) {
    /* Sending without validating socket or data */
    return send(socket_fd, data, length, 0);  /* socket_fd could be invalid, data could be NULL */
}

/* NON-COMPLIANT: No validation of buffer parameters */
int receive_data(int socket_fd, char *buffer, size_t buffer_size) {
    /* Receiving without validation */
    return recv(socket_fd, buffer, buffer_size, 0);  /* buffer could be NULL */
}

/* NON-COMPLIANT: No validation of address structure */
int connect_to_server(int socket_fd, struct sockaddr_in *server_addr) {
    /* Connecting without validation */
    return connect(socket_fd, (struct sockaddr *)server_addr,
                  sizeof(*server_addr));  /* server_addr could be NULL */
}

/* NON-COMPLIANT: No validation of port number */
int bind_to_port(int socket_fd, int port) {
    struct sockaddr_in addr;
    /* No validation of port range */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);  /* port could be invalid (0 or > 65535) */
    addr.sin_addr.s_addr = INADDR_ANY;

    return bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
}

/* NON-COMPLIANT: No validation of IP address string */
int set_server_address(struct sockaddr_in *addr, const char *ip_string, int port) {
    /* No NULL check for parameters */
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_aton(ip_string, &addr->sin_addr);  /* ip_string could be NULL or invalid */
    return 0;
}

/* NON-COMPLIANT: No validation of backlog parameter */
int start_listening(int socket_fd, int backlog) {
    /* No validation of backlog value */
    return listen(socket_fd, backlog);  /* backlog could be negative or too large */
}

/* NON-COMPLIANT: No validation of client address structure */
int accept_connection(int socket_fd, struct sockaddr_in *client_addr) {
    socklen_t addr_len = sizeof(*client_addr);
    /* No validation of client_addr */
    return accept(socket_fd, (struct sockaddr *)client_addr,
                 &addr_len);  /* client_addr could be NULL */
}

/* NON-COMPLIANT: No validation of option parameters */
int set_socket_option(int socket_fd, int level, int option, void *value, socklen_t value_len) {
    /* No validation of value pointer or length */
    return setsockopt(socket_fd, level, option, value, value_len);  /* value could be NULL */
}

int main(void) {
    int invalid_socket = -1;
    char *null_buffer = NULL;
    struct sockaddr_in *null_addr = NULL;

    /* Examples of dangerous network operations */
    // send_data(invalid_socket, null_buffer, 100);  /* Invalid socket and NULL data */
    // receive_data(invalid_socket, null_buffer, 1024);  /* Invalid socket and NULL buffer */
    // connect_to_server(invalid_socket, null_addr);  /* Invalid socket and NULL address */
    // bind_to_port(invalid_socket, 70000);  /* Invalid port number */
    // set_server_address(null_addr, NULL, -1);  /* NULL parameters */
    // start_listening(invalid_socket, -5);  /* Negative backlog */
    // accept_connection(invalid_socket, null_addr);  /* Invalid socket and NULL address */
    // set_socket_option(invalid_socket, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(int));

    printf("Network functions compiled but lack parameter validation\n");
    return 0;
}