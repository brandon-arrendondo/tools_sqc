/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: format_strings.c
 *
 * This case demonstrates violations where format strings and
 * message templates that never change are not const-qualified.
 */

#include <stdio.h>
#include <time.h>

void logging_functions(void) {
    /* NON-COMPLIANT: Format strings should be const */
    char log_format[] = "[%s] %s: %s\n";
    char time_format[] = "%Y-%m-%d %H:%M:%S";
    char error_format[] = "ERROR: %s (Code: %d)\n";
    char warning_format[] = "WARNING: %s at line %d\n";

    /* Format strings are never modified */
    printf(log_format, "2024-01-01 10:00:00", "INFO", "Application started");
    printf(log_format, "2024-01-01 10:00:01", "DEBUG", "Initializing components");
    printf(error_format, "File not found", 404);
    printf(warning_format, "Deprecated function used", 123);
}

void display_templates(void) {
    /* NON-COMPLIANT: Message templates should be const */
    char header_template[] = "========== %s ==========\n";
    char footer_template[] = "========== END %s ==========\n";
    char item_template[] = "  * Item %d: %s\n";
    char summary_template[] = "Total: %d items, Size: %lu bytes\n";

    /* Templates are used but never modified */
    printf(header_template, "Report");
    printf(item_template, 1, "First item");
    printf(item_template, 2, "Second item");
    printf(summary_template, 2, (unsigned long)1024);
    printf(footer_template, "Report");
}

void sql_queries(void) {
    /* NON-COMPLIANT: SQL query templates should be const */
    char select_query[] = "SELECT * FROM %s WHERE id = %d";
    char insert_query[] = "INSERT INTO %s VALUES (%d, '%s', %f)";
    char update_query[] = "UPDATE %s SET %s = '%s' WHERE id = %d";
    char delete_query[] = "DELETE FROM %s WHERE id = %d";

    printf("\nSQL Query Templates:\n");

    /* Query strings are never modified, only used for formatting */
    printf("Select: %s\n", select_query);
    printf("Insert: %s\n", insert_query);
    printf("Update: %s\n", update_query);
    printf("Delete: %s\n", delete_query);

    /* Simulate using the queries */
    char buffer[256];
    sprintf(buffer, select_query, "users", 1);
    printf("Generated: %s\n", buffer);
}

void url_patterns(void) {
    /* NON-COMPLIANT: URL patterns should be const */
    char base_url[] = "https://api.example.com";
    char endpoint_pattern[] = "%s/v1/%s/%d";
    char query_pattern[] = "%s?page=%d&limit=%d";
    char auth_header[] = "Bearer %s";

    printf("\nURL Patterns:\n");

    /* Patterns are used for formatting but never modified */
    char url[256];
    sprintf(url, endpoint_pattern, base_url, "users", 123);
    printf("API endpoint: %s\n", url);

    sprintf(url, query_pattern, base_url, 1, 20);
    printf("Query URL: %s\n", url);

    char header[100];
    sprintf(header, auth_header, "token123");
    printf("Auth header: %s\n", header);
}

int main(void) {
    /* NON-COMPLIANT: Main format strings should be const */
    char program_header[] = "=== Format String Demo ===\n";
    char section_separator[] = "\n--- %s ---\n";

    printf(program_header);

    printf(section_separator, "Logging");
    logging_functions();

    printf(section_separator, "Templates");
    display_templates();

    printf(section_separator, "SQL");
    sql_queries();

    printf(section_separator, "URLs");
    url_patterns();

    return 0;
}