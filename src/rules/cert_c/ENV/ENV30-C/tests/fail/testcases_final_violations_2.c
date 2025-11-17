/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: final_violations_2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Backup filename creation */
void unsafe_backup_filename(void) {
    char *config_file = getenv("CONFIG_FILE");
    if (config_file) {
        strcat(config_file, ".bak");  /* Undefined behavior */
        printf("Backup file: %s\n", config_file);
    }
}

/* NON-COMPLIANT: Log level modification */
void unsafe_log_level(void) {
    char *log_level = getenv("LOG_LEVEL");
    if (log_level && strcmp(log_level, "INFO") == 0) {
        strcpy(log_level, "DEBUG");  /* Undefined behavior */
        printf("Enhanced log level: %s\n", log_level);
    }
}

int main(void) {
    setenv("CONFIG_FILE", "/etc/app.conf", 1);
    setenv("LOG_LEVEL", "INFO", 1);

    unsafe_backup_filename();
    unsafe_log_level();
    return 0;
}