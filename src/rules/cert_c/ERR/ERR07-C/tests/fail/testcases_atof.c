/*
 * Rule: ERR07-C
 * Status: FAIL - atof lacks error checking, prefer strtod
 */

double atof(const char *str);

void f(const char *str) {
    double val = atof(str);  /* VIOLATION: use strtod instead */
}
