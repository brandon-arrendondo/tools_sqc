/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM02-C violation
 */

p = malloc(sizeof(gadget)); /* Imminent problem */