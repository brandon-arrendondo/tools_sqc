/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM02-C violation
 */

widget *p;

/* ... */

p = (gadget *)malloc(sizeof(gadget)); /* Invalid assignment */