/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Configuration value used as format string
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    char output_format[100];
} Config;

void load_config(Config *cfg) {
    printf("Enter output format: ");
    fgets(cfg->output_format, sizeof(cfg->output_format), stdin);
}

int main() {
    Config app_config;
    char data[] = "sample data";

    load_config(&app_config);

    // VULNERABLE: config value as format string
    printf(app_config.output_format, data);

    return 0;
}