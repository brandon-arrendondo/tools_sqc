/*
 * Rule: DCL02-C
 * Source: testcases
 * Status: FAIL - Visually similar identifiers in same scope
 */

/* 1/I confusion at file scope */
int var1;
int varI;

/* 5/S confusion at file scope */
int data5;
int dataS;

/* 8/B confusion at file scope */
int buf8;
int bufB;

/* n/h confusion at file scope */
int count_n;
int count_h;
