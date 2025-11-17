/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: PASS
 * Reason: Volatile value read before macro call
 */

#define ABS(x) (((x) < 0) ? -(x) : (x))  /* UNSAFE */

volatile int sensor_value;

void read_sensor(void) {
    // Read volatile once, then use in macro - COMPLIANT
    int value = sensor_value;
    int result = ABS(value);
}

int main(void) {
    read_sensor();
    return 0;
}
