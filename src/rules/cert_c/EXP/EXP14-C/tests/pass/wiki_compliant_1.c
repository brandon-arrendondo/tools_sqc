/*
 * Rule: EXP14-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP14-C violation
 */

uint8_t port = 0x5a;
uint8_t result_8 = (uint8_t) (~port) >> 4;