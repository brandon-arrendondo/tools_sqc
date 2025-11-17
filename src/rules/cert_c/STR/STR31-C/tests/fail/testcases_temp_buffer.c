/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Temporary buffer sized inadequately for string manipulation
 */

#include <stdio.h>
#include <string.h>

char* create_filename(const char* base, int number) {
    static char temp[12];  // Static buffer - dangerous for multiple calls

    sprintf(temp, "%s_%05d.txt", base, number);  // base could be long
    return temp;
}

int main() {
    char* file1 = create_filename("very_long_basename", 12345);
    char* file2 = create_filename("short", 67890);

    printf("File 1: %s\n", file1);  // Might be corrupted
    printf("File 2: %s\n", file2);

    return 0;
}