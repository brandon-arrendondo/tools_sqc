/*
 * Rule: EXP37-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP37-C violation
 */

fd = open(ms, O_CREAT | O_EXCL | O_WRONLY | O_TRUNC);