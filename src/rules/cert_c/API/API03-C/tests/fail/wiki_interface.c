/*
 * Rule: API03-C
 * Source: wiki
 * Status: FAIL - Should trigger API03-C violation
 */

int fputs(const char * restrict s, FILE * restrict stream);

int fprintf(FILE * restrict stream, const char * restrict format, ...);