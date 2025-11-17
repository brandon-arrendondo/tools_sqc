/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Protocol parser with char type fails on binary protocols
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char header;
    unsigned char length;
    unsigned char data[256];
} Packet;

int parse_packet(FILE *file, Packet *packet) {
    char c; // VIOLATION: char type cannot handle all protocol bytes

    // Parse header - will fail if header byte is 0xFF
    if ((c = fgetc(file)) == EOF) {
        return 0; // No more packets
    }
    packet->header = (unsigned char)c;

    // Parse length
    if ((c = fgetc(file)) == EOF) {
        return -1; // Incomplete packet
    }
    packet->length = (unsigned char)c;

    // Parse data - will fail if data contains 0xFF bytes
    for (int i = 0; i < packet->length; i++) {
        if ((c = fgetc(file)) == EOF) {
            return -1; // Incomplete packet
        }
        packet->data[i] = (unsigned char)c;
    }

    return 1; // Success
}

int main() {
    FILE *file = fopen("protocol_data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open protocol file\n");
        return 1;
    }

    Packet packet;
    int result;
    int packet_count = 0;

    // Parse protocol packets - will miss packets with 0xFF bytes
    while ((result = parse_packet(file, &packet)) > 0) {
        printf("Packet %d: Header=0x%02X, Length=%d\n",
               ++packet_count, packet.header, packet.length);
    }

    if (result < 0) {
        printf("Error: Incomplete packet\n");
    }

    printf("Total packets parsed: %d\n", packet_count);

    fclose(file);
    return 0;
}