/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: FAIL - Function declarations with likely-readonly parameter names
 */

/* src parameter should be const */
void copy_data(char *dest, char *src);

/* source parameter should be const */
void transform_data(int *output, int *source);

/* input parameter should be const */
void process(int *input);

/* read parameter should be const */
void log_read_data(char *read_buffer);

/* s2 parameter (common convention) should be const */
void my_strcat(char *s1, char *s2);
