/*
 * Rule: DCL17-C
 * Source: testcases
 * Status: FAIL - Direct access to volatile-qualified variables
 */

volatile int sensor_value;

/* Direct read of volatile in assignment */
void direct_read(void) {
    int local = sensor_value;
    (void)local;
}

/* Direct write to volatile */
volatile int output_reg;
void direct_write(void) {
    output_reg = 42;
}

/* Volatile used in binary expression */
volatile int counter;
void volatile_in_expression(void) {
    if (counter < 100) {
        counter = counter + 1;
    }
}

/* Volatile used in return statement */
volatile int status_reg;
int read_status(void) {
    return status_reg;
}
