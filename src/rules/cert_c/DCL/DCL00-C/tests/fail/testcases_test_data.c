/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: test_data.c
 *
 * This case demonstrates violations where test data and
 * testing constants are not const-qualified.
 */

#include <stdio.h>
#include <string.h>

void unit_test_data(void) {
    /* NON-COMPLIANT: Test input data should be const */
    int test_integers[] = {0, 1, -1, 42, 100, -100, 32767, -32768};
    double test_doubles[] = {0.0, 1.0, -1.0, 3.14159, 2.71828, 1e10, 1e-10};
    char test_strings[][20] = {"", "hello", "world", "test123", "UPPERCASE", "lowercase"};

    /* NON-COMPLIANT: Expected results should be const */
    int expected_sums[] = {0, 1, 0, 42, 100, 0, 32767, -1};
    int expected_products[] = {0, 1, 1, 42, 100, 100, 32767, 32768};
    char expected_uppercase[][20] = {"", "HELLO", "WORLD", "TEST123", "UPPERCASE", "LOWERCASE"};

    printf("Unit Test Data:\\n");
    printf("  Test integers: ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", test_integers[i]);
    }
    printf("\\n");

    printf("  Test doubles: ");
    for (int i = 0; i < 7; i++) {
        printf("%.2f ", test_doubles[i]);
    }
    printf("\\n");

    printf("  Test strings: ");
    for (int i = 0; i < 6; i++) {
        printf("'%s' ", test_strings[i]);
    }
    printf("\\n");

    /* Test data used for validation but never modified */
    for (int i = 0; i < 6; i++) {
        printf("  Test: '%s' -> expected: '%s'\\n", test_strings[i], expected_uppercase[i]);
    }
}

void boundary_test_values(void) {
    /* NON-COMPLIANT: Boundary values should be const */
    int int_min_value = -2147483648;
    int int_max_value = 2147483647;
    unsigned int uint_max_value = 4294967295U;
    short short_min_value = -32768;
    short short_max_value = 32767;

    /* NON-COMPLIANT: Edge case values should be const */
    double zero_positive = 0.0;
    double zero_negative = -0.0;
    double infinity_positive = 1.0/0.0;
    double infinity_negative = -1.0/0.0;
    double not_a_number = 0.0/0.0;

    /* NON-COMPLIANT: String boundary cases should be const */
    char empty_string[] = "";
    char single_char[] = "a";
    char max_length_string[256];
    char special_chars[] = "!@#$%^&*()_+-=[]{}|;:,.<>?";

    printf("\\nBoundary Test Values:\\n");
    printf("  Integer range: %d to %d\\n", int_min_value, int_max_value);
    printf("  Unsigned max: %u\\n", uint_max_value);
    printf("  Short range: %d to %d\\n", short_min_value, short_max_value);

    printf("\\nFloating point edge cases:\\n");
    printf("  Zero positive: %f\\n", zero_positive);
    printf("  Zero negative: %f\\n", zero_negative);

    printf("\\nString edge cases:\\n");
    printf("  Empty: '%s' (length %lu)\\n", empty_string, strlen(empty_string));
    printf("  Single char: '%s' (length %lu)\\n", single_char, strlen(single_char));
    printf("  Special chars: '%s'\\n", special_chars);

    /* Boundary values used for testing but never modified */
    memset(max_length_string, 'A', 255);
    max_length_string[255] = '\\0';
    printf("  Max length string created: %lu chars\\n", strlen(max_length_string));
}

void performance_test_data(void) {
    /* NON-COMPLIANT: Performance test parameters should be const */
    int small_dataset_size = 100;
    int medium_dataset_size = 10000;
    int large_dataset_size = 1000000;
    int iterations_per_test = 1000;

    /* NON-COMPLIANT: Timing thresholds should be const */
    double fast_threshold_ms = 1.0;
    double acceptable_threshold_ms = 10.0;
    double slow_threshold_ms = 100.0;
    double timeout_threshold_ms = 5000.0;

    /* NON-COMPLIANT: Memory usage limits should be const */
    int small_memory_limit = 1024;      /* bytes */
    int medium_memory_limit = 1048576;  /* 1MB */
    int large_memory_limit = 104857600; /* 100MB */

    printf("\\nPerformance Test Configuration:\\n");
    printf("  Dataset sizes: Small=%d, Medium=%d, Large=%d\\n",
           small_dataset_size, medium_dataset_size, large_dataset_size);
    printf("  Iterations per test: %d\\n", iterations_per_test);

    printf("\\nTiming Thresholds (ms):\\n");
    printf("  Fast: %.1f, Acceptable: %.1f, Slow: %.1f, Timeout: %.1f\\n",
           fast_threshold_ms, acceptable_threshold_ms, slow_threshold_ms, timeout_threshold_ms);

    printf("\\nMemory Limits:\\n");
    printf("  Small: %d bytes, Medium: %d bytes, Large: %d bytes\\n",
           small_memory_limit, medium_memory_limit, large_memory_limit);

    /* Performance parameters used for test execution but never modified */
    int current_dataset = medium_dataset_size;
    double max_allowed_time = acceptable_threshold_ms;
    printf("\\nCurrent test: %d items, max time: %.1f ms\\n", current_dataset, max_allowed_time);
}

void mock_test_data(void) {
    /* NON-COMPLIANT: Mock response data should be const */
    char mock_json_response[] = "{\\"status\\":\\"success\\",\\"code\\":200,\\"data\\":\\"test\\"}";
    char mock_xml_response[] = "<?xml version=\\"1.0\\"?><response><status>success</status></response>";
    char mock_error_response[] = "{\\"status\\":\\"error\\",\\"code\\":404,\\"message\\":\\"Not found\\"}";

    /* NON-COMPLIANT: Mock user data should be const */
    char mock_username[] = "testuser";
    char mock_email[] = "test@example.com";
    char mock_password[] = "testpass123";
    int mock_user_id = 12345;
    char mock_session_token[] = "abc123def456ghi789";

    /* NON-COMPLIANT: Test database records should be const */
    struct {
        int id;
        char name[50];
        char department[30];
        double salary;
    } mock_employees[] = {
        {1, "John Doe", "Engineering", 75000.0},
        {2, "Jane Smith", "Marketing", 65000.0},
        {3, "Bob Johnson", "Sales", 55000.0}
    };

    printf("\\nMock Test Data:\\n");
    printf("  JSON response: %s\\n", mock_json_response);
    printf("  XML response: %s\\n", mock_xml_response);
    printf("  Error response: %s\\n", mock_error_response);

    printf("\\nMock User Data:\\n");
    printf("  Username: %s\\n", mock_username);
    printf("  Email: %s\\n", mock_email);
    printf("  User ID: %d\\n", mock_user_id);
    printf("  Session: %s\\n", mock_session_token);

    printf("\\nMock Employee Records:\\n");
    for (int i = 0; i < 3; i++) {
        printf("  ID: %d, Name: %s, Dept: %s, Salary: $%.2f\\n",
               mock_employees[i].id, mock_employees[i].name,
               mock_employees[i].department, mock_employees[i].salary);
    }

    /* Mock data used for testing but never modified */
    char test_response[256];
    strcpy(test_response, mock_json_response);
    printf("\\nUsing mock response: %s\\n", test_response);
}

int main(void) {
    /* NON-COMPLIANT: Test configuration should be const */
    char test_suite_name[] = "Comprehensive Test Suite";
    char test_version[] = "1.0";
    int max_test_failures = 5;
    int timeout_seconds = 300;
    char log_file[] = "test_results.log";

    /* NON-COMPLIANT: Test result codes should be const */
    int TEST_PASS = 0;
    int TEST_FAIL = 1;
    int TEST_SKIP = 2;
    int TEST_ERROR = 3;
    int TEST_TIMEOUT = 4;

    printf("Test Configuration:\\n");
    printf("  Suite: %s v%s\\n", test_suite_name, test_version);
    printf("  Max failures: %d\\n", max_test_failures);
    printf("  Timeout: %d seconds\\n", timeout_seconds);
    printf("  Log file: %s\\n", log_file);

    printf("\\nTest Result Codes:\\n");
    printf("  Pass=%d, Fail=%d, Skip=%d, Error=%d, Timeout=%d\\n",
           TEST_PASS, TEST_FAIL, TEST_SKIP, TEST_ERROR, TEST_TIMEOUT);

    unit_test_data();
    boundary_test_values();
    performance_test_data();
    mock_test_data();

    return 0;
}