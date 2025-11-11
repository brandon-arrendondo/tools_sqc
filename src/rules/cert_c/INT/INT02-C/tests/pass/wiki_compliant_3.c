/*
 * Rule: INT02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT02-C violation
 */

uint8_t port = 0x5a;
uint8_t result_8 = (uint8_t) (~port) >> 4;