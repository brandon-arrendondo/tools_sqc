/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: database_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Database URL modification */
void unsafe_database_url_modification(void) {
    char *db_url = getenv("DATABASE_URL");
    if (db_url) {
        /* VIOLATION: Adding query parameters */
        strcat(db_url, "?sslmode=require");  /* Undefined behavior */
        printf("DB URL with SSL: %s\n", db_url);
    }
}

/* NON-COMPLIANT: Database name modification */
void unsafe_database_name_modification(void) {
    char *db_name = getenv("DB_NAME");
    if (db_name) {
        /* VIOLATION: Adding suffix */
        strcat(db_name, "_prod");  /* Undefined behavior */
        printf("Production DB: %s\n", db_name);
    }
}

int main(void) {
    setenv("DATABASE_URL", "postgresql://user:pass@localhost/db", 1);
    setenv("DB_NAME", "myapp", 1);

    unsafe_database_url_modification();
    unsafe_database_name_modification();
    return 0;
}