/*
 * Rule: POS39-C
 * Source: testcases
 * Status: FAIL - Should trigger POS39-C violation
 *
 * Network receive into integer without byte order conversion
 */

uint32_t num;

/* VIOLATION: no ntohl after recv into multi-byte integer */
if (recv(sock, (void *)&num, sizeof(uint32_t), 0) < (int)sizeof(uint32_t)) {
    /* Handle error */
}

printf("Received: %u\n", (unsigned int)num);
