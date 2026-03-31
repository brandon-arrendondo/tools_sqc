/*
 * Rule: FIO09-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO09-C violation
 * Description: Binary fwrite of struct data is non-portable
 */

#include <stdio.h>

struct sensor_data {
    int sensor_id;
    double reading;
    char label[16];
};

void save_binary(FILE *fp, const struct sensor_data *data) {
    fwrite(data, sizeof(struct sensor_data), 1, fp);  /* Violation */
}
