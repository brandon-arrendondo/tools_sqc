/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: safe_network_operations.c
 *
 * This case demonstrates compliant network operations with
 * comprehensive parameter validation and error handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

/* Network operation result structure */
typedef struct {
    int success;
    int socket_fd;
    size_t bytes_transferred;
    char error_message[256];
} NetworkResult;

/* COMPLIANT: Safe socket creation with validation */
NetworkResult safe_create_socket(int domain, int type, int protocol) {
    NetworkResult result = {0, -1, 0, ""};

    /* Validate domain parameter */
    if (domain != AF_INET && domain != AF_INET6 && domain != AF_UNIX) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket domain: %d", domain);
        return result;
    }

    /* Validate type parameter */
    if (type != SOCK_STREAM && type != SOCK_DGRAM && type != SOCK_RAW) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket type: %d", type);
        return result;
    }

    /* Create socket */
    int sock_fd = socket(domain, type, protocol);
    if (sock_fd < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Socket creation failed: %s", strerror(errno));
        return result;
    }

    /* Set socket to non-blocking mode for safety */
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags < 0 || fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(sock_fd);
        snprintf(result.error_message, sizeof(result.error_message),
                "Failed to set non-blocking mode: %s", strerror(errno));
        return result;
    }

    result.success = 1;
    result.socket_fd = sock_fd;
    snprintf(result.error_message, sizeof(result.error_message),
            "Socket created successfully (fd=%d)", sock_fd);

    return result;
}

/* COMPLIANT: Safe socket binding with validation */
NetworkResult safe_bind_socket(int socket_fd, const char *ip_address, int port) {
    NetworkResult result = {0, -1, 0, ""};

    /* Validate socket file descriptor */
    if (socket_fd < 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket file descriptor: %d", socket_fd);
        return result;
    }

    /* Validate IP address parameter */
    if (!ip_address) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL IP address");
        return result;
    }

    /* Validate port range */
    if (port < 0 || port > 65535) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid port number: %d (must be 0-65535)", port);
        return result;
    }

    /* Validate IP address format */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, ip_address, &addr.sin_addr) != 1) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid IP address format: %s", ip_address);
        return result;
    }

    /* Set socket reuse option for development */
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Failed to set SO_REUSEADDR: %s", strerror(errno));
        return result;
    }

    /* Bind socket */
    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Bind failed for %s:%d: %s", ip_address, port, strerror(errno));
        return result;
    }

    result.success = 1;
    result.socket_fd = socket_fd;
    snprintf(result.error_message, sizeof(result.error_message),
            "Socket bound successfully to %s:%d", ip_address, port);

    return result;
}

/* COMPLIANT: Safe socket connection with timeout */
NetworkResult safe_connect_socket(int socket_fd, const char *ip_address, int port, int timeout_seconds) {
    NetworkResult result = {0, -1, 0, ""};

    /* Validate parameters */
    if (socket_fd < 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket file descriptor: %d", socket_fd);
        return result;
    }

    if (!ip_address) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL IP address");
        return result;
    }

    if (port <= 0 || port > 65535) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid port number: %d", port);
        return result;
    }

    if (timeout_seconds < 0 || timeout_seconds > 3600) {  /* Max 1 hour */
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid timeout: %d seconds", timeout_seconds);
        return result;
    }

    /* Prepare address structure */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, ip_address, &addr.sin_addr) != 1) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid IP address: %s", ip_address);
        return result;
    }

    /* Attempt connection (socket should be non-blocking) */
    int connect_result = connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr));

    if (connect_result == 0) {
        /* Immediate connection success */
        result.success = 1;
        result.socket_fd = socket_fd;
        snprintf(result.error_message, sizeof(result.error_message),
                "Connected immediately to %s:%d", ip_address, port);
        return result;
    }

    if (errno != EINPROGRESS) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Connection failed: %s", strerror(errno));
        return result;
    }

    /* Wait for connection to complete with timeout */
    fd_set write_fds;
    struct timeval timeout;

    FD_ZERO(&write_fds);
    FD_SET(socket_fd, &write_fds);

    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;

    int select_result = select(socket_fd + 1, NULL, &write_fds, NULL, &timeout);

    if (select_result < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Select failed: %s", strerror(errno));
        return result;
    }

    if (select_result == 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Connection timeout after %d seconds", timeout_seconds);
        return result;
    }

    /* Check if connection was successful */
    int socket_error = 0;
    socklen_t len = sizeof(socket_error);
    if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &socket_error, &len) < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "getsockopt failed: %s", strerror(errno));
        return result;
    }

    if (socket_error != 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Connection failed: %s", strerror(socket_error));
        return result;
    }

    result.success = 1;
    result.socket_fd = socket_fd;
    snprintf(result.error_message, sizeof(result.error_message),
            "Connected successfully to %s:%d", ip_address, port);

    return result;
}

/* COMPLIANT: Safe data transmission with validation */
NetworkResult safe_send_data(int socket_fd, const void *data, size_t data_size, int timeout_seconds) {
    NetworkResult result = {0, -1, 0, ""};

    /* Validate parameters */
    if (socket_fd < 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket file descriptor: %d", socket_fd);
        return result;
    }

    if (!data) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL data pointer");
        return result;
    }

    if (data_size == 0) {
        result.success = 1;
        result.socket_fd = socket_fd;
        result.bytes_transferred = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                "No data to send (zero length)");
        return result;
    }

    /* Validate data size is reasonable */
    const size_t MAX_SEND_SIZE = 10 * 1024 * 1024;  /* 10 MB */
    if (data_size > MAX_SEND_SIZE) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Data size too large: %zu bytes", data_size);
        return result;
    }

    if (timeout_seconds < 0 || timeout_seconds > 300) {  /* Max 5 minutes */
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid timeout: %d seconds", timeout_seconds);
        return result;
    }

    /* Send data with timeout */
    const char *data_ptr = (const char *)data;
    size_t bytes_sent = 0;
    time_t start_time = time(NULL);

    while (bytes_sent < data_size) {
        /* Check timeout */
        if (difftime(time(NULL), start_time) > timeout_seconds) {
            snprintf(result.error_message, sizeof(result.error_message),
                    "Send timeout: sent %zu of %zu bytes", bytes_sent, data_size);
            return result;
        }

        /* Wait for socket to be ready for writing */
        fd_set write_fds;
        struct timeval timeout;

        FD_ZERO(&write_fds);
        FD_SET(socket_fd, &write_fds);

        timeout.tv_sec = 1;  /* 1 second timeout for select */
        timeout.tv_usec = 0;

        int select_result = select(socket_fd + 1, NULL, &write_fds, NULL, &timeout);

        if (select_result < 0) {
            snprintf(result.error_message, sizeof(result.error_message),
                    "Select failed during send: %s", strerror(errno));
            return result;
        }

        if (select_result == 0) {
            continue;  /* Timeout, try again */
        }

        /* Send data */
        ssize_t sent = send(socket_fd, data_ptr + bytes_sent, data_size - bytes_sent, 0);

        if (sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;  /* Try again */
            }
            snprintf(result.error_message, sizeof(result.error_message),
                    "Send failed: %s", strerror(errno));
            return result;
        }

        if (sent == 0) {
            snprintf(result.error_message, sizeof(result.error_message),
                    "Connection closed during send");
            return result;
        }

        bytes_sent += (size_t)sent;
    }

    result.success = 1;
    result.socket_fd = socket_fd;
    result.bytes_transferred = bytes_sent;
    snprintf(result.error_message, sizeof(result.error_message),
            "Sent %zu bytes successfully", bytes_sent);

    return result;
}

/* COMPLIANT: Safe data reception with validation */
NetworkResult safe_receive_data(int socket_fd, void *buffer, size_t buffer_size, int timeout_seconds) {
    NetworkResult result = {0, -1, 0, ""};

    /* Validate parameters */
    if (socket_fd < 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid socket file descriptor: %d", socket_fd);
        return result;
    }

    if (!buffer) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL buffer pointer");
        return result;
    }

    if (buffer_size == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Zero buffer size");
        return result;
    }

    if (timeout_seconds < 0 || timeout_seconds > 300) {  /* Max 5 minutes */
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid timeout: %d seconds", timeout_seconds);
        return result;
    }

    /* Wait for data with timeout */
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);

    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;

    int select_result = select(socket_fd + 1, &read_fds, NULL, NULL, &timeout);

    if (select_result < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Select failed: %s", strerror(errno));
        return result;
    }

    if (select_result == 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Receive timeout after %d seconds", timeout_seconds);
        return result;
    }

    /* Receive data */
    ssize_t received = recv(socket_fd, buffer, buffer_size, 0);

    if (received < 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Receive failed: %s", strerror(errno));
        return result;
    }

    if (received == 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Connection closed by peer");
        return result;
    }

    result.success = 1;
    result.socket_fd = socket_fd;
    result.bytes_transferred = (size_t)received;
    snprintf(result.error_message, sizeof(result.error_message),
            "Received %zu bytes successfully", (size_t)received);

    return result;
}

/* COMPLIANT: Safe socket cleanup */
void safe_close_socket(int socket_fd) {
    if (socket_fd >= 0) {
        /* Shutdown both directions */
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);
    }
}

int main(void) {
    printf("=== Safe Network Operations Demo ===\n\n");

    /* Test socket creation */
    printf("1. Creating TCP socket:\n");
    NetworkResult create_result = safe_create_socket(AF_INET, SOCK_STREAM, 0);
    if (create_result.success) {
        printf("   %s\n", create_result.error_message);
    } else {
        printf("   Error: %s\n", create_result.error_message);
        return 1;
    }

    int sock_fd = create_result.socket_fd;

    /* Test parameter validation with invalid inputs */
    printf("\n2. Parameter validation tests:\n");

    NetworkResult invalid_socket = safe_create_socket(999, SOCK_STREAM, 0);
    if (!invalid_socket.success) {
        printf("   Correctly rejected invalid domain: %s\n", invalid_socket.error_message);
    }

    NetworkResult invalid_bind = safe_bind_socket(-1, "127.0.0.1", 8080);
    if (!invalid_bind.success) {
        printf("   Correctly rejected invalid socket: %s\n", invalid_bind.error_message);
    }

    NetworkResult invalid_ip = safe_bind_socket(sock_fd, "invalid.ip.address", 8080);
    if (!invalid_ip.success) {
        printf("   Correctly rejected invalid IP: %s\n", invalid_ip.error_message);
    }

    NetworkResult invalid_port = safe_bind_socket(sock_fd, "127.0.0.1", 99999);
    if (!invalid_port.success) {
        printf("   Correctly rejected invalid port: %s\n", invalid_port.error_message);
    }

    /* Test bind operation */
    printf("\n3. Binding socket:\n");
    NetworkResult bind_result = safe_bind_socket(sock_fd, "127.0.0.1", 0);  /* Port 0 = any available */
    if (bind_result.success) {
        printf("   %s\n", bind_result.error_message);
    } else {
        printf("   Warning: %s\n", bind_result.error_message);
    }

    /* Test data operations with validation */
    printf("\n4. Data operation validation:\n");
    char test_data[] = "Hello, Network!";
    char receive_buffer[1024];

    NetworkResult send_null = safe_send_data(sock_fd, NULL, 100, 5);
    if (!send_null.success) {
        printf("   Correctly rejected NULL data: %s\n", send_null.error_message);
    }

    NetworkResult recv_null = safe_receive_data(sock_fd, NULL, 100, 5);
    if (!recv_null.success) {
        printf("   Correctly rejected NULL buffer: %s\n", recv_null.error_message);
    }

    NetworkResult send_huge = safe_send_data(sock_fd, test_data, SIZE_MAX, 5);
    if (!send_huge.success) {
        printf("   Correctly rejected oversized data: %s\n", send_huge.error_message);
    }

    NetworkResult invalid_timeout = safe_send_data(sock_fd, test_data, strlen(test_data), -10);
    if (!invalid_timeout.success) {
        printf("   Correctly rejected invalid timeout: %s\n", invalid_timeout.error_message);
    }

    /* Test connection validation */
    printf("\n5. Connection validation:\n");
    NetworkResult connect_invalid = safe_connect_socket(-1, "127.0.0.1", 80, 10);
    if (!connect_invalid.success) {
        printf("   Correctly rejected invalid socket: %s\n", connect_invalid.error_message);
    }

    NetworkResult connect_bad_ip = safe_connect_socket(sock_fd, "999.999.999.999", 80, 10);
    if (!connect_bad_ip.success) {
        printf("   Correctly rejected invalid IP: %s\n", connect_bad_ip.error_message);
    }

    NetworkResult connect_bad_port = safe_connect_socket(sock_fd, "127.0.0.1", -1, 10);
    if (!connect_bad_port.success) {
        printf("   Correctly rejected invalid port: %s\n", connect_bad_port.error_message);
    }

    NetworkResult connect_bad_timeout = safe_connect_socket(sock_fd, "127.0.0.1", 80, 9999);
    if (!connect_bad_timeout.success) {
        printf("   Correctly rejected invalid timeout: %s\n", connect_bad_timeout.error_message);
    }

    /* Clean up */
    printf("\n6. Cleanup:\n");
    safe_close_socket(sock_fd);
    printf("   Socket closed safely\n");

    printf("\n=== Network operations demo completed ===\n");
    return 0;
}