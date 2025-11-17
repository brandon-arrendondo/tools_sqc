/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: regex_operations_unchecked.c
 *
 * This case demonstrates violations where regular expression functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

/* NON-COMPLIANT: No validation of regex pattern */
int compile_regex(regex_t *compiled_regex, const char *pattern, int flags) {
    /* No validation of compiled_regex or pattern */
    return regcomp(compiled_regex, pattern, flags);  /* pattern could be NULL */
}

/* NON-COMPLIANT: No validation of regex execution parameters */
int execute_regex(regex_t *compiled_regex, const char *string, regmatch_t *matches, int max_matches) {
    /* No validation of parameters */
    return regexec(compiled_regex, string, max_matches, matches, 0);  /* string could be NULL */
}

/* NON-COMPLIANT: No validation of error buffer */
void get_regex_error(int error_code, regex_t *compiled_regex, char *error_buffer, size_t buffer_size) {
    /* No validation of error_buffer */
    regerror(error_code, compiled_regex, error_buffer, buffer_size);  /* error_buffer could be NULL */
}

/* NON-COMPLIANT: No validation of match results */
void extract_match(const char *string, regmatch_t *match, char *result_buffer) {
    /* No validation of parameters */
    int start = match->rm_so;  /* match could be NULL */
    int end = match->rm_eo;
    int length = end - start;
    strncpy(result_buffer, string + start, length);  /* result_buffer could be NULL */
    result_buffer[length] = '\0';
}

/* NON-COMPLIANT: No validation of replacement parameters */
void regex_replace(const char *input, const char *pattern, const char *replacement, char *output) {
    regex_t regex;
    regmatch_t match;

    /* No validation of input parameters */
    regcomp(&regex, pattern, REG_EXTENDED);  /* pattern could be NULL */

    if (regexec(&regex, input, 1, &match, 0) == 0) {  /* input could be NULL */
        /* Copy prefix */
        strncpy(output, input, match.rm_so);  /* output could be NULL */
        output[match.rm_so] = '\0';

        /* Add replacement */
        strcat(output, replacement);  /* replacement could be NULL */

        /* Add suffix */
        strcat(output, input + match.rm_eo);
    }

    regfree(&regex);
}

/* NON-COMPLIANT: No validation of flags parameter */
int case_insensitive_match(const char *pattern, const char *string) {
    regex_t regex;
    /* No validation of pattern or string */
    int result = regcomp(&regex, pattern, REG_ICASE | REG_EXTENDED);
    if (result == 0) {
        result = regexec(&regex, string, 0, NULL, 0);
        regfree(&regex);
    }
    return result == 0;
}

/* NON-COMPLIANT: No validation of match count */
int count_matches(const char *string, const char *pattern, int max_count) {
    regex_t regex;
    regmatch_t matches[10];  /* Fixed size array without validation */

    /* No validation of max_count against array size */
    regcomp(&regex, pattern, REG_EXTENDED | REG_GLOBAL);
    int count = 0;
    const char *search_start = string;

    /* Could overflow matches array if max_count > 10 */
    while (count < max_count && regexec(&regex, search_start, 1, matches, 0) == 0) {
        count++;
        search_start += matches[0].rm_eo;
    }

    regfree(&regex);
    return count;
}

int main(void) {
    regex_t *null_regex = NULL;
    char *null_string = NULL;
    char *null_buffer = NULL;

    /* Examples of dangerous regex operations */
    // compile_regex(null_regex, null_string, 0);  /* NULL parameters */
    // execute_regex(null_regex, null_string, NULL, 0);  /* NULL parameters */
    // get_regex_error(0, null_regex, null_buffer, 100);  /* NULL buffer */
    // extract_match("test", NULL, null_buffer);  /* NULL match and buffer */
    // regex_replace(null_string, null_string, null_string, null_buffer);  /* All NULL */
    // case_insensitive_match(null_string, null_string);  /* NULL parameters */
    // count_matches("test string", "pattern", 100);  /* max_count exceeds array size */

    printf("Regex functions compiled but lack parameter validation\n");
    return 0;
}