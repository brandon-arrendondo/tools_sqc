/*
 * Rule: ERR00-C
 * Source: wiki
 * Status: FAIL - Return value from fopen not checked
 */

#include <stdio.h>

void read_data(void) {
    int value;
    FILE *fp = fopen("data.txt", "r");
    fscanf(fp, "%d", &value);
    fclose(fp);
}
