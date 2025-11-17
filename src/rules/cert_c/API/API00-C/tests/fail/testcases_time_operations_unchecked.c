/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: time_operations_unchecked.c
 *
 * This case demonstrates violations where time-related functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

/* NON-COMPLIANT: No validation of time structure */
void print_time(struct tm *time_struct) {
    /* Direct use without NULL check */
    printf("Time: %02d:%02d:%02d\n",
           time_struct->tm_hour,  /* Could dereference NULL */
           time_struct->tm_min,
           time_struct->tm_sec);
}

/* NON-COMPLIANT: No validation of time_t pointer */
double calculate_time_difference(time_t *start, time_t *end) {
    /* Direct use without NULL checks */
    return difftime(*end, *start);  /* Could dereference NULL pointers */
}

/* NON-COMPLIANT: No validation of buffer for time formatting */
void format_time_string(char *buffer, size_t buffer_size, struct tm *time_struct) {
    /* No validation of buffer or time_struct */
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_struct);  /* NULL parameters */
}

/* NON-COMPLIANT: No validation of sleep duration */
void sleep_milliseconds(int milliseconds) {
    /* No validation of milliseconds value */
    usleep(milliseconds * 1000);  /* Could overflow or be negative */
}

/* NON-COMPLIANT: No validation of timer parameters */
void set_timer_interval(int seconds, int microseconds) {
    struct timeval interval;
    /* No validation of time values */
    interval.tv_sec = seconds;  /* Could be negative */
    interval.tv_usec = microseconds;  /* Could be > 999999 or negative */
    /* Would use this interval without validation */
}

/* NON-COMPLIANT: No validation of date components */
time_t create_timestamp(int year, int month, int day, int hour, int minute, int second) {
    struct tm time_struct;
    /* No validation of date/time components */
    time_struct.tm_year = year - 1900;  /* Year could be invalid */
    time_struct.tm_mon = month - 1;  /* Month could be out of range */
    time_struct.tm_mday = day;  /* Day could be invalid for month */
    time_struct.tm_hour = hour;  /* Hour could be > 23 */
    time_struct.tm_min = minute;  /* Minute could be > 59 */
    time_struct.tm_sec = second;  /* Second could be > 59 */
    time_struct.tm_isdst = -1;

    return mktime(&time_struct);
}

/* NON-COMPLIANT: No validation of timezone string */
void set_timezone(const char *tz_string) {
    /* No validation of tz_string */
    setenv("TZ", tz_string, 1);  /* tz_string could be NULL or invalid */
    tzset();
}

/* NON-COMPLIANT: No validation of clock ID */
void get_clock_time(clockid_t clock_id, struct timespec *time_spec) {
    /* No validation of time_spec pointer */
    clock_gettime(clock_id, time_spec);  /* time_spec could be NULL */
}

int main(void) {
    struct tm *null_time = NULL;
    time_t *null_time_t = NULL;
    char *null_buffer = NULL;

    /* Examples of dangerous time operations */
    // print_time(null_time);  /* NULL pointer dereference */
    // calculate_time_difference(null_time_t, null_time_t);  /* NULL pointers */
    // format_time_string(null_buffer, 100, null_time);  /* NULL parameters */
    // sleep_milliseconds(-1000);  /* Negative sleep duration */
    // set_timer_interval(-5, 2000000);  /* Invalid time values */
    // create_timestamp(1900, 13, 32, 25, 61, 70);  /* Invalid date/time */
    // set_timezone(NULL);  /* NULL timezone */
    // get_clock_time(CLOCK_REALTIME, NULL);  /* NULL timespec */

    printf("Time functions compiled but lack parameter validation\n");
    return 0;
}