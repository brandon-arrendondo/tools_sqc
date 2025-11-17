/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: conditional_initialization.c
 *
 * This case demonstrates violations where variables are initialized
 * in some code paths but not others, leading to potential reads
 * of uninitialized memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Conditional initialization with missing path */
int process_grade(int score) {
    char grade;  /* Uninitialized */

    if (score >= 90) {
        grade = 'A';
    } else if (score >= 80) {
        grade = 'B';
    } else if (score >= 70) {
        grade = 'C';
    } else if (score >= 60) {
        grade = 'D';
    }
    /* Missing else case for score < 60 */

    return grade;  /* Reading uninitialized memory for scores < 60 */
}

/* NON-COMPLIANT: Complex nested conditions */
void analyze_data(int data[], int size, int threshold) {
    int above_threshold, below_threshold, equal_threshold;
    /* All uninitialized */

    if (size > 0) {
        if (threshold > 0) {
            above_threshold = 0;
            below_threshold = 0;
            equal_threshold = 0;

            for (int i = 0; i < size; i++) {
                if (data[i] > threshold) {
                    above_threshold++;
                } else if (data[i] < threshold) {
                    below_threshold++;
                } else {
                    equal_threshold++;
                }
            }
        }
        /* Missing initialization when threshold <= 0 */
    }
    /* Missing initialization when size <= 0 */

    /* Reading potentially uninitialized variables */
    printf("Above: %d, Below: %d, Equal: %d\n",
           above_threshold, below_threshold, equal_threshold);
}

/* NON-COMPLIANT: Switch statement with missing cases */
int get_month_days(int month, int year) {
    int days;  /* Uninitialized */

    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            days = 31;
            break;
        case 4: case 6: case 9: case 11:
            days = 30;
            break;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                days = 29;
            } else {
                days = 28;
            }
            break;
        /* Missing default case for invalid months */
    }

    return days;  /* Reading uninitialized memory for invalid months */
}

/* NON-COMPLIANT: Error handling path leaves variable uninitialized */
int safe_divide(double numerator, double denominator, double *result) {
    double temp_result;  /* Uninitialized */

    if (result == NULL) {
        return -1;  /* Early return without initializing temp_result */
    }

    if (denominator == 0.0) {
        return -1;  /* Error path - temp_result remains uninitialized */
    }

    temp_result = numerator / denominator;

    /* Later code might read temp_result even on error paths */
    if (temp_result < 0) {  /* This check happens even on error paths */
        printf("Negative result detected\n");
    }

    *result = temp_result;
    return 0;
}

/* NON-COMPLIANT: Loop with conditional initialization */
void process_string_array(char *strings[], int count) {
    char longest_string[256];  /* Uninitialized */
    int max_length;           /* Uninitialized */

    for (int i = 0; i < count; i++) {
        if (strings[i] != NULL) {
            int len = strlen(strings[i]);
            if (len > max_length) {  /* Reading uninitialized max_length on first iteration */
                max_length = len;
                strcpy(longest_string, strings[i]);
            }
        }
    }

    /* Reading potentially uninitialized variables */
    printf("Longest string (%d chars): %s\n", max_length, longest_string);
}

int main(void) {
    printf("=== Conditional Initialization Demo ===\n");

    /* Test 1: Missing grade case */
    printf("Grade for 50: %c\n", process_grade(50));  /* Undefined behavior */

    /* Test 2: Missing threshold condition */
    int data[] = {1, 2, 3, 4, 5};
    analyze_data(data, 5, 0);  /* Undefined behavior */

    /* Test 3: Invalid month */
    printf("Days in month 13: %d\n", get_month_days(13, 2023));  /* Undefined behavior */

    /* Test 4: Error path variable usage */
    double result;
    if (safe_divide(10.0, 0.0, &result) != 0) {
        printf("Division failed\n");
    }

    /* Test 5: Empty string array */
    char *empty_strings[] = {NULL, NULL, NULL};
    process_string_array(empty_strings, 3);  /* Undefined behavior */

    return 0;
}