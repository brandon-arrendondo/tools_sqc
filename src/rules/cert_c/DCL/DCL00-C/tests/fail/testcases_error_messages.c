/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: error_messages.c
 *
 * This case demonstrates violations where error messages and
 * diagnostic strings are not const-qualified.
 */

#include <stdio.h>

void system_errors(void) {
    /* NON-COMPLIANT: System error messages should be const */
    char err_out_of_memory[] = "Out of memory";
    char err_file_not_found[] = "File not found";
    char err_access_denied[] = "Access denied";
    char err_disk_full[] = "Disk full";
    char err_network_timeout[] = "Network timeout";
    char err_invalid_parameter[] = "Invalid parameter";

    /* NON-COMPLIANT: Error codes should be const */
    int ERR_SUCCESS = 0;
    int ERR_GENERAL = -1;
    int ERR_OUT_OF_MEMORY = -2;
    int ERR_FILE_NOT_FOUND = -3;
    int ERR_ACCESS_DENIED = -4;
    int ERR_DISK_FULL = -5;
    int ERR_NETWORK_TIMEOUT = -6;

    printf("System Error Messages:\\n");
    printf("  Code %d: %s\\n", ERR_OUT_OF_MEMORY, err_out_of_memory);
    printf("  Code %d: %s\\n", ERR_FILE_NOT_FOUND, err_file_not_found);
    printf("  Code %d: %s\\n", ERR_ACCESS_DENIED, err_access_denied);
    printf("  Code %d: %s\\n", ERR_DISK_FULL, err_disk_full);
    printf("  Code %d: %s\\n", ERR_NETWORK_TIMEOUT, err_network_timeout);

    /* Error messages used for logging but never modified */
    int error_code = ERR_FILE_NOT_FOUND;
    if (error_code == ERR_FILE_NOT_FOUND) {
        printf("  Error occurred: %s\\n", err_file_not_found);
    }
}

void validation_errors(void) {
    /* NON-COMPLIANT: Validation error messages should be const */
    char err_required_field[] = "This field is required";
    char err_invalid_email[] = "Invalid email address format";
    char err_password_too_short[] = "Password must be at least 8 characters";
    char err_password_too_weak[] = "Password must contain uppercase, lowercase, and numbers";
    char err_invalid_date[] = "Invalid date format (use YYYY-MM-DD)";
    char err_value_too_large[] = "Value exceeds maximum allowed";
    char err_value_too_small[] = "Value below minimum required";

    /* NON-COMPLIANT: Field names should be const */
    char field_email[] = "email";
    char field_password[] = "password";
    char field_birthdate[] = "birthdate";
    char field_age[] = "age";
    char field_phone[] = "phone";

    printf("\\nValidation Error Messages:\\n");
    printf("  %s: %s\\n", field_email, err_invalid_email);
    printf("  %s: %s\\n", field_password, err_password_too_short);
    printf("  %s: %s\\n", field_password, err_password_too_weak);
    printf("  %s: %s\\n", field_birthdate, err_invalid_date);
    printf("  %s: %s\\n", field_age, err_value_too_small);

    /* Error messages used for validation reporting but never modified */
    char current_field[] = "email";
    if (strcmp(current_field, field_email) == 0) {
        printf("  Validating %s: %s\\n", current_field, err_invalid_email);
    }
}

void database_errors(void) {
    /* NON-COMPLIANT: Database error messages should be const */
    char err_connection_failed[] = "Database connection failed";
    char err_query_timeout[] = "Query execution timeout";
    char err_duplicate_key[] = "Duplicate key violation";
    char err_foreign_key[] = "Foreign key constraint violation";
    char err_table_not_found[] = "Table does not exist";
    char err_column_not_found[] = "Column does not exist";
    char err_transaction_rollback[] = "Transaction was rolled back";

    /* NON-COMPLIANT: SQL state codes should be const */
    char sqlstate_success[] = "00000";
    char sqlstate_warning[] = "01000";
    char sqlstate_no_data[] = "02000";
    char sqlstate_connection_error[] = "08000";
    char sqlstate_feature_not_supported[] = "0A000";
    char sqlstate_syntax_error[] = "42000";

    printf("\\nDatabase Error Messages:\\n");
    printf("  %s: %s\\n", sqlstate_connection_error, err_connection_failed);
    printf("  %s: %s\\n", sqlstate_syntax_error, err_query_timeout);
    printf("  %s: %s\\n", sqlstate_success, "Query executed successfully");

    printf("\\nSQL Error Details:\\n");
    printf("  Duplicate key: %s\\n", err_duplicate_key);
    printf("  Foreign key: %s\\n", err_foreign_key);
    printf("  Table missing: %s\\n", err_table_not_found);

    /* SQL states used for error handling but never modified */
    char current_sqlstate[] = "42000";
    if (strcmp(current_sqlstate, sqlstate_syntax_error) == 0) {
        printf("  SQL syntax error detected\\n");
    }
}

void network_errors(void) {
    /* NON-COMPLIANT: Network error messages should be const */
    char err_host_unreachable[] = "Host unreachable";
    char err_connection_refused[] = "Connection refused";
    char err_ssl_handshake_failed[] = "SSL handshake failed";
    char err_certificate_invalid[] = "Invalid SSL certificate";
    char err_protocol_error[] = "Protocol error";
    char err_request_timeout[] = "Request timeout";
    char err_server_overloaded[] = "Server overloaded";

    /* NON-COMPLIANT: HTTP error descriptions should be const */
    char desc_400[] = "Bad Request - Invalid syntax";
    char desc_401[] = "Unauthorized - Authentication required";
    char desc_403[] = "Forbidden - Access not allowed";
    char desc_404[] = "Not Found - Resource does not exist";
    char desc_500[] = "Internal Server Error - Server malfunction";
    char desc_502[] = "Bad Gateway - Invalid response from upstream";
    char desc_503[] = "Service Unavailable - Server temporarily unavailable";

    printf("\\nNetwork Error Messages:\\n");
    printf("  Connection: %s\\n", err_connection_refused);
    printf("  SSL: %s\\n", err_ssl_handshake_failed);
    printf("  Protocol: %s\\n", err_protocol_error);
    printf("  Timeout: %s\\n", err_request_timeout);

    printf("\\nHTTP Error Descriptions:\\n");
    printf("  400: %s\\n", desc_400);
    printf("  401: %s\\n", desc_401);
    printf("  404: %s\\n", desc_404);
    printf("  500: %s\\n", desc_500);

    /* Error descriptions used for response generation but never modified */
    int status_code = 404;
    if (status_code == 404) {
        printf("  Response: %s\\n", desc_404);
    }
}

int main(void) {
    /* NON-COMPLIANT: Generic error templates should be const */
    char error_template[] = "Error %d: %s";
    char warning_template[] = "Warning: %s";
    char info_template[] = "Info: %s";
    char debug_template[] = "DEBUG: %s at %s:%d";

    /* NON-COMPLIANT: Log level names should be const */
    char level_error[] = "ERROR";
    char level_warning[] = "WARNING";
    char level_info[] = "INFO";
    char level_debug[] = "DEBUG";

    printf("Error Message Templates:\\n");
    printf("  Error: %s\\n", error_template);
    printf("  Warning: %s\\n", warning_template);
    printf("  Info: %s\\n", info_template);
    printf("  Debug: %s\\n", debug_template);

    printf("\\nLog Levels: %s, %s, %s, %s\\n",
           level_error, level_warning, level_info, level_debug);

    system_errors();
    validation_errors();
    database_errors();
    network_errors();

    return 0;
}