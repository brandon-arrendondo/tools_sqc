/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Volatile access in unsafe macro
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

volatile int sensor_value;

void read_sensor(void) {
    // Volatile access has side effect - read multiple times
    int result = ABS(sensor_value);  // Line 13 - VIOLATION
}

int main(void) {
    read_sensor();
    return 0;
}
