/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: enum_like_constants.c
 *
 * This case demonstrates violations where constants that could be
 * enums are implemented as non-const variables.
 */

#include <stdio.h>

void day_of_week(void) {
    /* NON-COMPLIANT: These should be const or enum */
    int SUNDAY = 0;
    int MONDAY = 1;
    int TUESDAY = 2;
    int WEDNESDAY = 3;
    int THURSDAY = 4;
    int FRIDAY = 5;
    int SATURDAY = 6;

    /* NON-COMPLIANT: Day names should be const */
    char *day_names[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    int today = 3;  /* Wednesday */

    printf("Days of the week:\n");
    /* Values are used but never modified */
    printf("  Sunday = %d\n", SUNDAY);
    printf("  Monday = %d\n", MONDAY);
    printf("  Today is %s (day %d)\n", day_names[today], today);

    if (today == WEDNESDAY) {
        printf("  It's the middle of the week!\n");
    }
}

void month_constants(void) {
    /* NON-COMPLIANT: Month constants should be const */
    int JANUARY = 1;
    int FEBRUARY = 2;
    int MARCH = 3;
    int APRIL = 4;
    int MAY = 5;
    int JUNE = 6;
    int JULY = 7;
    int AUGUST = 8;
    int SEPTEMBER = 9;
    int OCTOBER = 10;
    int NOVEMBER = 11;
    int DECEMBER = 12;

    /* NON-COMPLIANT: Days per month should be const */
    int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int current_month = 7;  /* July */

    printf("\nMonth Information:\n");
    printf("  Current month: %d (", current_month);

    /* Constants used for comparison but never modified */
    if (current_month == JANUARY) printf("January");
    else if (current_month == JULY) printf("July");
    else if (current_month == DECEMBER) printf("December");

    printf(")\n");
    printf("  Days in month: %d\n", days_per_month[current_month - 1]);
}

void priority_levels(void) {
    /* NON-COMPLIANT: Priority constants should be const */
    int PRIORITY_CRITICAL = 0;
    int PRIORITY_HIGH = 1;
    int PRIORITY_MEDIUM = 2;
    int PRIORITY_LOW = 3;
    int PRIORITY_TRIVIAL = 4;

    /* NON-COMPLIANT: Priority names should be const */
    char priority_names[][10] = {
        "CRITICAL",
        "HIGH",
        "MEDIUM",
        "LOW",
        "TRIVIAL"
    };

    printf("\nPriority Levels:\n");

    /* Values are used for display but never modified */
    for (int i = PRIORITY_CRITICAL; i <= PRIORITY_TRIVIAL; i++) {
        printf("  Level %d: %s\n", i, priority_names[i]);
    }

    int task_priority = PRIORITY_HIGH;
    printf("Current task priority: %s\n", priority_names[task_priority]);
}

void state_machine_states(void) {
    /* NON-COMPLIANT: State constants should be const or enum */
    int STATE_IDLE = 0;
    int STATE_INIT = 1;
    int STATE_READY = 2;
    int STATE_RUNNING = 3;
    int STATE_PAUSED = 4;
    int STATE_ERROR = 5;
    int STATE_TERMINATED = 6;

    int current_state = STATE_IDLE;
    int next_state = STATE_INIT;

    printf("\nState Machine:\n");
    printf("  Current state: %d\n", current_state);
    printf("  Next state: %d\n", next_state);

    /* State values used for transitions but never modified */
    if (current_state == STATE_IDLE && next_state == STATE_INIT) {
        printf("  Transitioning from IDLE to INIT\n");
        current_state = next_state;
    }

    if (current_state == STATE_INIT) {
        printf("  System is initializing...\n");
        next_state = STATE_READY;
    }
}

int main(void) {
    /* NON-COMPLIANT: Return codes should be const */
    int SUCCESS = 0;
    int FAILURE = -1;
    int ERROR_INVALID_ARG = -2;
    int ERROR_OUT_OF_MEMORY = -3;

    printf("Enum-like Constants Demo\n\n");

    day_of_week();
    month_constants();
    priority_levels();
    state_machine_states();

    /* Return codes used but never modified */
    printf("\nReturn codes:\n");
    printf("  SUCCESS: %d\n", SUCCESS);
    printf("  FAILURE: %d\n", FAILURE);
    printf("  ERROR_INVALID_ARG: %d\n", ERROR_INVALID_ARG);
    printf("  ERROR_OUT_OF_MEMORY: %d\n", ERROR_OUT_OF_MEMORY);

    return SUCCESS;  /* Using the non-const SUCCESS */
}