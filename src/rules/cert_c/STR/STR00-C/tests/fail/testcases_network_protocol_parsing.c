/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: network_protocol_parsing.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for network protocol parsing, leading to data
 * interpretation issues and potential security vulnerabilities.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* VIOLATION: Structure using signed char for binary protocol data */
struct protocol_header {
    signed char version;
    signed char type;
    signed char flags;
    signed char length;
};

int main(void) {
    /* VIOLATION: Binary protocol data with signed char */
    signed char packet_data[] = {
        0x01,  /* Version */
        0x02,  /* Type */
        0x80,  /* Flags (high bit set - may be negative) */
        0x10,  /* Length */
        /* Payload follows... */
        0x48, 0x65, 0x6C, 0x6C, 0x6F,  /* "Hello" */
        0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64,  /* " World" */
        0xFF, 0xFE, 0xFD, 0xFC  /* High-bit bytes */
    };

    printf("Network protocol parsing with signed char:\n");

    /* VIOLATION: Header parsing with sign interpretation issues */
    struct protocol_header *header = (struct protocol_header*)packet_data;

    printf("Header fields:\n");
    printf("  Version: %d\n", header->version);
    printf("  Type: %d\n", header->type);
    printf("  Flags: %d (0x%02X)\n", header->flags, (unsigned char)header->flags);
    printf("  Length: %d\n", header->length);

    /* VIOLATION: Flag checking with sign-dependent behavior */
    if (header->flags & 0x80) {  /* High bit check */
        printf("High bit flag is set\n");
    }

    /* VIOLATION: Length validation with sign issues */
    if (header->length > 0) {  /* May fail if length byte > 127 */
        printf("Valid length field\n");
    } else {
        printf("Invalid or zero length\n");
    }

    /* VIOLATION: Payload extraction with character type issues */
    signed char *payload = packet_data + sizeof(struct protocol_header);
    int payload_length = header->length;

    printf("\nPayload analysis:\n");
    printf("Payload bytes: ");
    for (int i = 0; i < payload_length; i++) {
        printf("0x%02X ", (unsigned char)payload[i]);
    }
    printf("\n");

    /* VIOLATION: ASCII payload interpretation */
    printf("Payload as string: ");
    for (int i = 0; i < payload_length; i++) {
        signed char c = payload[i];
        if (c >= 32 && c <= 126) {  /* ASCII printable range */
            printf("%c", c);
        } else {
            printf(".");
        }
    }
    printf("\n");

    /* VIOLATION: HTTP header parsing simulation */
    printf("\nHTTP header parsing:\n");

    signed char http_request[] =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: TestClient/1.0\r\n"
        "Accept: text/html\r\n"
        "\r\n";

    printf("HTTP request: %s", http_request);  /* Warning */

    /* VIOLATION: Header line parsing */
    signed char *line_start = http_request;
    signed char *line_end;

    while ((line_end = strstr(line_start, "\r\n")) != NULL) {  /* Warning */
        /* VIOLATION: Null termination for processing */
        *line_end = '\0';

        printf("Header line: %s\n", line_start);  /* Warning */

        /* Look for colon separator */
        signed char *colon = strchr(line_start, ':');  /* Warning */
        if (colon != NULL) {
            *colon = '\0';
            printf("  Field: %s\n", line_start);     /* Warning */
            printf("  Value: %s\n", colon + 2);      /* Warning (skip ": ") */
        }

        line_start = line_end + 2;  /* Skip \r\n */

        /* Check for end of headers */
        if (*line_start == '\0') {
            break;
        }
    }

    /* VIOLATION: URL parsing with character type issues */
    printf("\nURL parsing:\n");

    signed char url[] = "https://user:pass@example.com:8080/path?param=value#fragment";
    printf("URL: %s\n", url);  /* Warning */

    /* VIOLATION: Protocol extraction */
    signed char *protocol_end = strstr(url, "://");  /* Warning */
    if (protocol_end != NULL) {
        *protocol_end = '\0';
        printf("Protocol: %s\n", url);  /* Warning */

        /* VIOLATION: Authority parsing */
        signed char *authority_start = protocol_end + 3;
        signed char *path_start = strchr(authority_start, '/');  /* Warning */

        if (path_start != NULL) {
            *path_start = '\0';
            printf("Authority: %s\n", authority_start);  /* Warning */

            /* Look for authentication info */
            signed char *auth_end = strchr(authority_start, '@');  /* Warning */
            if (auth_end != NULL) {
                *auth_end = '\0';
                printf("  Auth: %s\n", authority_start);  /* Warning */
                authority_start = auth_end + 1;
            }

            /* Look for port */
            signed char *port_start = strchr(authority_start, ':');  /* Warning */
            if (port_start != NULL) {
                *port_start = '\0';
                printf("  Host: %s\n", authority_start);  /* Warning */
                printf("  Port: %s\n", port_start + 1);  /* Warning */
            } else {
                printf("  Host: %s\n", authority_start);  /* Warning */
            }
        }
    }

    /* VIOLATION: Email address parsing */
    printf("\nEmail parsing:\n");

    signed char email[] = "user.name+tag@subdomain.example.com";
    printf("Email: %s\n", email);  /* Warning */

    /* Find the @ symbol */
    signed char *at_symbol = strchr(email, '@');  /* Warning */
    if (at_symbol != NULL) {
        *at_symbol = '\0';
        printf("Local part: %s\n", email);       /* Warning */
        printf("Domain part: %s\n", at_symbol + 1);  /* Warning */

        /* VIOLATION: Domain validation simulation */
        signed char *domain = at_symbol + 1;
        signed char *dot = strchr(domain, '.');  /* Warning */
        if (dot != NULL) {
            printf("Domain has subdomain\n");
        }
    }

    /* VIOLATION: Base64 encoding simulation */
    printf("\nBase64 encoding simulation:\n");

    signed char input_data[] = "Hello, World!";
    printf("Input: %s\n", input_data);  /* Warning */

    /* Simple encoding simulation (not real Base64) */
    printf("Encoded bytes: ");
    for (size_t i = 0; input_data[i] != '\0'; i++) {
        signed char c = input_data[i];
        /* VIOLATION: Bit manipulation with signed char */
        signed char encoded = (c + 1) ^ 0x55;  /* Simple transformation */
        printf("0x%02X ", (unsigned char)encoded);
    }
    printf("\n");

    /* VIOLATION: JSON parsing simulation */
    printf("\nJSON parsing simulation:\n");

    signed char json_data[] = "{\"name\":\"John\",\"age\":30,\"active\":true}";
    printf("JSON: %s\n", json_data);  /* Warning */

    /* Simple key extraction */
    signed char *key_start = strchr(json_data, '"');  /* Warning */
    while (key_start != NULL) {
        key_start++;  /* Skip opening quote */
        signed char *key_end = strchr(key_start, '"');  /* Warning */
        if (key_end != NULL) {
            *key_end = '\0';
            printf("JSON key: %s\n", key_start);  /* Warning */

            /* Look for value after colon */
            signed char *colon = strchr(key_end + 1, ':');  /* Warning */
            if (colon != NULL) {
                /* Simple value extraction (incomplete) */
                signed char *value_start = colon + 1;
                while (*value_start == ' ') value_start++;  /* Skip spaces */
                printf("Value starts with: %c\n", *value_start);
            }

            /* Find next key */
            key_start = strchr(key_end + 1, '"');  /* Warning */
        } else {
            break;
        }
    }

    return 0;
}