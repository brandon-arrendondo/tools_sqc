/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Path buffer is sized to accommodate maximum possible path length
 */

#include <stdio.h>
#include <string.h>

#define MAX_PATH 260

int main() {
    char directory[] = "/home/user";
    char filename[] = "/document.txt";
    char fullpath[MAX_PATH];

    if (strlen(directory) + strlen(filename) + 1 <= sizeof(fullpath)) {
        strcpy(fullpath, directory);
        strcat(fullpath, filename);
        printf("Full path: %s\n", fullpath);
    }

    return 0;
}