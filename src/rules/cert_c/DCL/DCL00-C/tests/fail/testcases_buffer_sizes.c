/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: buffer_sizes.c
 *
 * This case demonstrates violations where buffer sizes and limits
 * that never change are not const-qualified.
 */

#include <stdio.h>
#include <string.h>

void string_buffers(void) {
    /* NON-COMPLIANT: Buffer sizes should be const */
    int MAX_NAME_LENGTH = 50;
    int MAX_PATH_LENGTH = 256;
    int MAX_LINE_LENGTH = 1024;
    int MAX_URL_LENGTH = 2048;

    char name[50];
    char path[256];
    char line[1024];
    char url[2048];

    /* Sizes are used for bounds checking but never modified */
    strncpy(name, "John Doe", MAX_NAME_LENGTH - 1);
    name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(path, "/usr/local/bin", MAX_PATH_LENGTH - 1);
    path[MAX_PATH_LENGTH - 1] = '\0';

    printf("Buffer Size Constants:\n");
    printf("  MAX_NAME_LENGTH: %d\n", MAX_NAME_LENGTH);
    printf("  MAX_PATH_LENGTH: %d\n", MAX_PATH_LENGTH);
    printf("  MAX_LINE_LENGTH: %d\n", MAX_LINE_LENGTH);
    printf("  MAX_URL_LENGTH: %d\n", MAX_URL_LENGTH);

    printf("\nBuffer contents:\n");
    printf("  Name: %s (max %d)\n", name, MAX_NAME_LENGTH);
    printf("  Path: %s (max %d)\n", path, MAX_PATH_LENGTH);
}

void network_buffers(void) {
    /* NON-COMPLIANT: Network buffer sizes should be const */
    int MTU_SIZE = 1500;
    int PACKET_HEADER_SIZE = 20;
    int MAX_PAYLOAD_SIZE = 1480;
    int MIN_PACKET_SIZE = 64;

    /* NON-COMPLIANT: Protocol constants should be const */
    int TCP_WINDOW_SIZE = 65535;
    int UDP_MAX_SIZE = 65507;
    int ETHERNET_FRAME_SIZE = 1518;

    printf("\nNetwork Buffer Sizes:\n");
    printf("  MTU Size: %d bytes\n", MTU_SIZE);
    printf("  Packet Header: %d bytes\n", PACKET_HEADER_SIZE);
    printf("  Max Payload: %d bytes\n", MAX_PAYLOAD_SIZE);
    printf("  Min Packet: %d bytes\n", MIN_PACKET_SIZE);

    /* Values used for calculations but never modified */
    int actual_payload = MTU_SIZE - PACKET_HEADER_SIZE;
    printf("  Calculated payload: %d bytes\n", actual_payload);

    printf("\nProtocol Limits:\n");
    printf("  TCP Window: %d bytes\n", TCP_WINDOW_SIZE);
    printf("  UDP Max: %d bytes\n", UDP_MAX_SIZE);
    printf("  Ethernet Frame: %d bytes\n", ETHERNET_FRAME_SIZE);
}

void memory_pools(void) {
    /* NON-COMPLIANT: Memory pool sizes should be const */
    int SMALL_BLOCK_SIZE = 32;
    int MEDIUM_BLOCK_SIZE = 128;
    int LARGE_BLOCK_SIZE = 512;
    int HUGE_BLOCK_SIZE = 4096;

    /* NON-COMPLIANT: Pool counts should be const */
    int SMALL_POOL_COUNT = 100;
    int MEDIUM_POOL_COUNT = 50;
    int LARGE_POOL_COUNT = 20;
    int HUGE_POOL_COUNT = 5;

    printf("\nMemory Pool Configuration:\n");

    /* Sizes and counts used for calculations but never modified */
    int small_total = SMALL_BLOCK_SIZE * SMALL_POOL_COUNT;
    int medium_total = MEDIUM_BLOCK_SIZE * MEDIUM_POOL_COUNT;
    int large_total = LARGE_BLOCK_SIZE * LARGE_POOL_COUNT;
    int huge_total = HUGE_BLOCK_SIZE * HUGE_POOL_COUNT;

    printf("  Small:  %d x %d bytes = %d bytes\n",
           SMALL_POOL_COUNT, SMALL_BLOCK_SIZE, small_total);
    printf("  Medium: %d x %d bytes = %d bytes\n",
           MEDIUM_POOL_COUNT, MEDIUM_BLOCK_SIZE, medium_total);
    printf("  Large:  %d x %d bytes = %d bytes\n",
           LARGE_POOL_COUNT, LARGE_BLOCK_SIZE, large_total);
    printf("  Huge:   %d x %d bytes = %d bytes\n",
           HUGE_POOL_COUNT, HUGE_BLOCK_SIZE, huge_total);

    int total_memory = small_total + medium_total + large_total + huge_total;
    printf("  Total pool memory: %d bytes\n", total_memory);
}

int main(void) {
    /* NON-COMPLIANT: Cache sizes should be const */
    int L1_CACHE_SIZE = 32768;    /* 32 KB */
    int L2_CACHE_SIZE = 262144;   /* 256 KB */
    int L3_CACHE_SIZE = 8388608;  /* 8 MB */
    int CACHE_LINE_SIZE = 64;

    printf("Cache Configuration:\n");
    printf("  L1 Cache: %d bytes (%d KB)\n", L1_CACHE_SIZE, L1_CACHE_SIZE / 1024);
    printf("  L2 Cache: %d bytes (%d KB)\n", L2_CACHE_SIZE, L2_CACHE_SIZE / 1024);
    printf("  L3 Cache: %d bytes (%d MB)\n", L3_CACHE_SIZE, L3_CACHE_SIZE / 1048576);
    printf("  Cache Line: %d bytes\n", CACHE_LINE_SIZE);

    /* Sizes used for calculations but never modified */
    int l1_lines = L1_CACHE_SIZE / CACHE_LINE_SIZE;
    printf("  L1 cache lines: %d\n", l1_lines);

    string_buffers();
    network_buffers();
    memory_pools();

    return 0;
}