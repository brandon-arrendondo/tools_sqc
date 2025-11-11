/*
 * Rule: ERR34-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR34-C violation
 */

atoi: (int)strtol(nptr, (char **)NULL, 10)
atol: strtol(nptr, (char **)NULL, 10)
atoll: strtoll(nptr, (char **)NULL, 10)
atof: strtod(nptr, (char **)NULL)