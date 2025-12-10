/*
 * Rule: DCL02-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL02-C violation
 *
 * These two identifiers differ only by visually similar characters:
 * - id_O uses capital letter O
 * - id_0 uses numeric digit zero
 */

int id_O; /* (Capital letter O) */
int id_0; /* (Numeric digit zero) */
