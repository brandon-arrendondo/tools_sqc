/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: switch_case_values.c
 *
 * This case demonstrates violations where values used in switch
 * statements and case labels are not const-qualified.
 */

#include <stdio.h>

void process_command(int cmd) {
    /* NON-COMPLIANT: Command codes should be const */
    int CMD_START = 1;
    int CMD_STOP = 2;
    int CMD_PAUSE = 3;
    int CMD_RESUME = 4;
    int CMD_RESET = 5;

    /* These values are never modified but used for comparison */
    if (cmd == CMD_START) {
        printf("Starting...\n");
    } else if (cmd == CMD_STOP) {
        printf("Stopping...\n");
    } else if (cmd == CMD_PAUSE) {
        printf("Pausing...\n");
    } else if (cmd == CMD_RESUME) {
        printf("Resuming...\n");
    } else if (cmd == CMD_RESET) {
        printf("Resetting...\n");
    } else {
        printf("Unknown command: %d\n", cmd);
    }
}

void handle_status_code(int status) {
    /* NON-COMPLIANT: Status codes should be const */
    int STATUS_OK = 200;
    int STATUS_CREATED = 201;
    int STATUS_BAD_REQUEST = 400;
    int STATUS_UNAUTHORIZED = 401;
    int STATUS_NOT_FOUND = 404;
    int STATUS_SERVER_ERROR = 500;

    printf("Status code %d: ", status);

    /* Values used for comparison but never modified */
    if (status == STATUS_OK) {
        printf("OK\n");
    } else if (status == STATUS_CREATED) {
        printf("Created\n");
    } else if (status == STATUS_BAD_REQUEST) {
        printf("Bad Request\n");
    } else if (status == STATUS_UNAUTHORIZED) {
        printf("Unauthorized\n");
    } else if (status == STATUS_NOT_FOUND) {
        printf("Not Found\n");
    } else if (status == STATUS_SERVER_ERROR) {
        printf("Internal Server Error\n");
    } else {
        printf("Unknown\n");
    }
}

void menu_selection(void) {
    /* NON-COMPLIANT: Menu options should be const */
    char OPTION_FILE = 'F';
    char OPTION_EDIT = 'E';
    char OPTION_VIEW = 'V';
    char OPTION_HELP = 'H';
    char OPTION_QUIT = 'Q';

    char choices[] = {'F', 'E', 'V', 'H', 'Q'};

    printf("\nMenu Options:\n");
    printf("  %c - File operations\n", OPTION_FILE);
    printf("  %c - Edit mode\n", OPTION_EDIT);
    printf("  %c - View settings\n", OPTION_VIEW);
    printf("  %c - Help\n", OPTION_HELP);
    printf("  %c - Quit\n", OPTION_QUIT);

    /* Simulate processing each option */
    for (int i = 0; i < 5; i++) {
        printf("Processing option %c...\n", choices[i]);
    }
}

int main(void) {
    /* NON-COMPLIANT: Test values should be const */
    int test_commands[] = {1, 2, 3, 4, 5};
    int test_statuses[] = {200, 404, 500};

    printf("Command Processing:\n");
    for (int i = 0; i < 5; i++) {
        process_command(test_commands[i]);
    }

    printf("\nStatus Code Handling:\n");
    for (int i = 0; i < 3; i++) {
        handle_status_code(test_statuses[i]);
    }

    menu_selection();

    return 0;
}