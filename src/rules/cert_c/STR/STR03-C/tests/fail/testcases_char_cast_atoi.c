/*
 * Rule: STR03-C
 * Source: testcases
 * Status: FAIL - Casting atoi/strtol result to char truncates and loses error info
 */

#include <stdlib.h>

/* (char)atoi() truncation */
char get_char_from_string(const char *str) {
    return (char)atoi(str);
}

/* (char)strtol() truncation */
char get_char_from_strtol(const char *str) {
    return (char)strtol(str, NULL, 10);
}
