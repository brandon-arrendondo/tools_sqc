/*
 * Rule: EXP07-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP07-C violation
 * Description: Magic number used in shift operation
 */

unsigned int compute_blocks(unsigned int nbytes) {
    return 1 + ((nbytes - 1) >> 9);  /* Violation: 9 is a magic number */
}

unsigned int extract_field(unsigned int val) {
    return (val >> 12) & 0xF;  /* Violation: 12 is a magic shift */
}
