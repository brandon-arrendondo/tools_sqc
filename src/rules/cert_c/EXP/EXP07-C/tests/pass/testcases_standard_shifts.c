/*
 * Rule: EXP07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP07-C violation
 * Description: Standard byte-boundary shifts and named constants
 */

#define BLOCK_SHIFT 9

unsigned int blocks_named(unsigned int nbytes) {
    return 1 + ((nbytes - 1) >> BLOCK_SHIFT);
}

unsigned int byte_extract(unsigned int val) {
    return (val >> 8) & 0xFF;   /* Byte boundary shift: 8 */
}

unsigned int word_extract(unsigned int val) {
    return (val >> 16) & 0xFFFF;  /* Byte boundary shift: 16 */
}

unsigned int bit_extract(unsigned int val, int pos) {
    return (val >> pos) & 1;  /* Variable shift, not magic */
}
