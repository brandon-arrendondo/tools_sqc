/*
 * Rule: DCL17-C
 * Source: testcases
 * Status: PASS - Volatile variables accessed through wrapper functions
 */

int vol_read(volatile int *vp) {
    return *vp;
}

void vol_write(volatile int *vp, int val) {
    *vp = val;
}

volatile int sensor;
volatile int actuator;

/* Reading volatile through wrapper function */
void safe_read(void) {
    int local = vol_read(&sensor);
    (void)local;
}

/* Writing volatile through wrapper function */
void safe_write(void) {
    vol_write(&actuator, 100);
}

/* Passing volatile address to function is compliant */
void process_sensor(volatile int *p);
void safe_pass(void) {
    process_sensor(&sensor);
}

/* Non-volatile variables are always fine */
int normal_var;
void normal_access(void) {
    normal_var = 10;
    int x = normal_var;
    (void)x;
}
