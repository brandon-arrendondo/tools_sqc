/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Path components combined exceed buffer size
 */

#include <stdio.h>
#include <string.h>

int main() {
    char dir[] = "/very/long/directory/path/with/many/subdirectories";
    char file[] = "/very_long_filename_that_exceeds_limits.txt";
    char path[50];  // Too small for combined path

    strcpy(path, dir);
    strcat(path, file);  // Combined length > 50 bytes
    printf("Path: %s\n", path);

    return 0;
}