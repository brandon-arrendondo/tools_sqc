/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: protocol_handlers_unchecked.c
 *
 * This case demonstrates violations where protocol handling functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Protocol message types */
typedef enum {
    MSG_CONNECT = 1,
    MSG_DISCONNECT = 2,
    MSG_DATA = 3,
    MSG_ACK = 4,
    MSG_ERROR = 5
} MessageType;

/* Protocol header structure */
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t type;
    uint32_t length;
    uint32_t sequence;
    uint32_t checksum;
} ProtocolHeader;

/* Protocol message structure */
typedef struct {
    ProtocolHeader header;
    void *payload;
    size_t payload_size;
} ProtocolMessage;

/* Connection state structure */
typedef struct {
    int socket_fd;
    uint32_t next_sequence;
    int is_connected;
    char *peer_address;
} Connection;

/* NON-COMPLIANT: No validation of message parsing */
ProtocolMessage *parse_message(const void *buffer, size_t buffer_size) {
    /* No validation of buffer or buffer_size */
    ProtocolMessage *message = malloc(sizeof(ProtocolMessage));

    /* No validation of buffer size before copying header */
    memcpy(&message->header, buffer, sizeof(ProtocolHeader));  /* buffer could be NULL or too small */

    /* No validation of header fields */
    message->payload_size = message->header.length;  /* length could be excessive */
    if (message->payload_size > 0) {
        message->payload = malloc(message->payload_size);
        /* No bounds checking for payload copy */
        memcpy(message->payload, (char *)buffer + sizeof(ProtocolHeader),
               message->payload_size);  /* Could read beyond buffer */
    } else {
        message->payload = NULL;
    }

    return message;
}

/* NON-COMPLIANT: No validation of message serialization */
size_t serialize_message(const ProtocolMessage *message, void *buffer, size_t buffer_size) {
    /* No validation of message or buffer */
    size_t total_size = sizeof(ProtocolHeader) + message->payload_size;  /* message could be NULL */

    if (total_size > buffer_size) {  /* No validation of buffer */
        return 0;  /* But we don't validate buffer is not NULL */
    }

    /* No validation before copying */
    memcpy(buffer, &message->header, sizeof(ProtocolHeader));  /* buffer could be NULL */

    if (message->payload_size > 0 && message->payload) {
        memcpy((char *)buffer + sizeof(ProtocolHeader), message->payload, message->payload_size);
    }

    return total_size;
}

/* NON-COMPLIANT: No validation of connection establishment */
Connection *establish_connection(const char *address, int port, int timeout_ms) {
    Connection *conn = malloc(sizeof(Connection));

    /* No validation of address or port */
    conn->peer_address = malloc(strlen(address) + 1);  /* address could be NULL */
    strcpy(conn->peer_address, address);

    conn->socket_fd = port;  /* Mock socket creation, port not validated */
    conn->next_sequence = 1;
    conn->is_connected = 1;

    return conn;
}

/* NON-COMPLIANT: No validation of message sending */
int send_message(Connection *conn, MessageType type, const void *data, size_t data_size) {
    /* No validation of conn or data */
    ProtocolMessage message;
    message.header.magic = 0xDEADBEEF;
    message.header.version = 1;
    message.header.type = type;      /* type not validated */
    message.header.length = data_size;  /* data_size not validated */
    message.header.sequence = conn->next_sequence++;  /* conn could be NULL */
    message.header.checksum = 0;  /* No actual checksum calculation */

    message.payload = (void *)data;  /* data could be NULL */
    message.payload_size = data_size;

    /* Mock sending */
    printf("Sending message type %d with %zu bytes\n", type, data_size);
    return 0;
}

/* NON-COMPLIANT: No validation of message receiving */
ProtocolMessage *receive_message(Connection *conn, int timeout_ms) {
    /* No validation of conn or timeout */
    printf("Receiving message on connection to %s\n", conn->peer_address);  /* conn could be NULL */

    /* Mock message reception */
    ProtocolMessage *message = malloc(sizeof(ProtocolMessage));
    message->header.magic = 0xDEADBEEF;
    message->header.version = 1;
    message->header.type = MSG_DATA;
    message->header.length = 100;
    message->header.sequence = 1;
    message->header.checksum = 0;

    message->payload_size = 100;
    message->payload = malloc(100);
    memset(message->payload, 0, 100);

    return message;
}

/* NON-COMPLIANT: No validation of checksum calculation */
uint32_t calculate_checksum(const void *data, size_t data_size) {
    /* No validation of data */
    uint32_t checksum = 0;
    const uint8_t *bytes = (const uint8_t *)data;  /* data could be NULL */

    for (size_t i = 0; i < data_size; i++) {
        checksum += bytes[i];  /* Could access invalid memory */
    }

    return checksum;
}

/* NON-COMPLIANT: No validation of protocol version handling */
int handle_version_negotiation(Connection *conn, uint16_t client_version) {
    /* No validation of conn or version */
    printf("Negotiating version %d on connection %s\n",
           client_version, conn->peer_address);  /* conn could be NULL */

    /* No validation of version compatibility */
    if (client_version > 10) {  /* Arbitrary validation without proper bounds */
        return -1;
    }

    return 0;
}

/* NON-COMPLIANT: No validation of fragmentation handling */
ProtocolMessage *reassemble_fragments(ProtocolMessage **fragments, size_t fragment_count) {
    /* No validation of fragments array or count */
    size_t total_payload_size = 0;

    for (size_t i = 0; i < fragment_count; i++) {
        total_payload_size += fragments[i]->payload_size;  /* fragments could be NULL array */
    }

    ProtocolMessage *complete_message = malloc(sizeof(ProtocolMessage));
    complete_message->payload = malloc(total_payload_size);  /* Could be huge allocation */
    complete_message->payload_size = total_payload_size;

    size_t offset = 0;
    for (size_t i = 0; i < fragment_count; i++) {
        memcpy((char *)complete_message->payload + offset,
               fragments[i]->payload,    /* fragments[i] could be NULL */
               fragments[i]->payload_size);
        offset += fragments[i]->payload_size;
    }

    /* Copy header from first fragment without validation */
    complete_message->header = fragments[0]->header;  /* fragments[0] could be NULL */

    return complete_message;
}

/* NON-COMPLIANT: No validation of encryption parameters */
void encrypt_payload(ProtocolMessage *message, const void *key, size_t key_size) {
    /* No validation of message, key, or key_size */
    printf("Encrypting payload of size %zu with key size %zu\n",
           message->payload_size, key_size);  /* message could be NULL */

    /* Mock encryption */
    uint8_t *payload_bytes = (uint8_t *)message->payload;
    const uint8_t *key_bytes = (const uint8_t *)key;  /* key could be NULL */

    for (size_t i = 0; i < message->payload_size; i++) {
        payload_bytes[i] ^= key_bytes[i % key_size];  /* Division by zero if key_size is 0 */
    }
}

/* NON-COMPLIANT: No validation of compression */
size_t compress_payload(const void *input, size_t input_size, void *output, size_t output_size) {
    /* No validation of any parameters */
    printf("Compressing %zu bytes\n", input_size);

    /* Mock compression - simple copy */
    size_t compressed_size = input_size / 2;  /* Simulated compression ratio */

    if (compressed_size > output_size) {  /* No validation of output buffer */
        compressed_size = output_size;
    }

    memcpy(output, input, compressed_size);  /* Both could be NULL */
    return compressed_size;
}

int main(void) {
    Connection *null_conn = NULL;
    ProtocolMessage *null_message = NULL;
    void *null_buffer = NULL;
    ProtocolMessage **null_fragments = NULL;

    /* Examples of dangerous protocol operations */
    // parse_message(null_buffer, 0);  /* NULL buffer */
    // serialize_message(null_message, null_buffer, 0);  /* NULL parameters */
    // establish_connection(NULL, -1, -1000);  /* NULL address and invalid parameters */
    // send_message(null_conn, 999, null_buffer, SIZE_MAX);  /* NULL parameters and invalid values */
    // receive_message(null_conn, -5000);  /* NULL connection and negative timeout */
    // calculate_checksum(null_buffer, 1000);  /* NULL buffer */
    // handle_version_negotiation(null_conn, 65535);  /* NULL connection */
    // reassemble_fragments(null_fragments, 100);  /* NULL fragments array */
    // encrypt_payload(null_message, null_buffer, 0);  /* NULL parameters and zero key size */
    // compress_payload(null_buffer, 100, null_buffer, 0);  /* NULL parameters */

    printf("Protocol functions compiled but lack parameter validation\n");
    return 0;
}