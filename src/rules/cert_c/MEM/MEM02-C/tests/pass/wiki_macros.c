/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 */

#define MALLOC(type) ((type *)malloc(sizeof(type)))