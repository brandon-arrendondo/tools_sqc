/*
 * Rule: FIO09-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO09-C violation
 * Description: Text-based I/O for structured data is portable
 */

#include <stdio.h>
#include <stdlib.h>

struct sensor_data {
    int sensor_id;
    double reading;
    char label[16];
};

void save_text(FILE *fp, const struct sensor_data *data) {
    fprintf(fp, "%d %f %s\n", data->sensor_id, data->reading, data->label);
}

int load_text(FILE *fp, struct sensor_data *data) {
    char buf[128];
    if (fgets(buf, sizeof(buf), fp) == NULL) return -1;
    return 0;
}
