/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Simulated database structure
typedef struct {
    int id;
    char name[64];
    double balance;
    int is_locked;
} account_t;

account_t database[100];
volatile sig_atomic_t transaction_count = 0;

void database_handler(int sig) {
    transaction_count++;

    printf("Handler: Signal %d performing database transaction\n", sig);

    // Violation: Database operations without proper signal masking
    // can cause partial updates and data corruption
    int account_id = sig % 100;
    account_t* account = &database[account_id];

    if (account->is_locked) {
        printf("Handler: Account %d is locked, aborting\n", account_id);
        return;
    }

    // Lock account (vulnerable to interruption)
    account->is_locked = 1;

    printf("Handler: Processing transaction for account %d\n", account_id);

    // Simulate database read
    double old_balance = account->balance;
    char old_name[64];
    strcpy(old_name, account->name);

    // Vulnerability window
    sleep(1);

    // Simulate transaction processing
    account->balance += 100.0 * sig;
    snprintf(account->name, sizeof(account->name),
             "Account_%d_Sig_%d", account_id, sig);

    // Another vulnerability window
    usleep(500000);

    // Verify transaction
    printf("Handler: Account %d: %s, balance %.2f -> %.2f\n",
           account_id, old_name, old_balance, account->balance);

    // Unlock account
    account->is_locked = 0;

    printf("Handler: Transaction %d complete\n", transaction_count);
}

int main() {
    struct sigaction sa;

    // Initialize database
    for (int i = 0; i < 100; i++) {
        database[i].id = i;
        snprintf(database[i].name, sizeof(database[i].name), "Account_%d", i);
        database[i].balance = 1000.0;
        database[i].is_locked = 0;
    }

    // Install handler without masking
    sa.sa_handler = database_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Database transactions vulnerable to interruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Database initialized, send signals to corrupt transactions\n");

    while (1) {
        printf("Main: Transactions processed: %d\n", transaction_count);

        // Check for corruption
        int corrupted_accounts = 0;
        int locked_accounts = 0;

        for (int i = 0; i < 100; i++) {
            if (database[i].is_locked) {
                locked_accounts++;
            }
            if (database[i].balance < 0) {
                corrupted_accounts++;
            }
        }

        printf("Main: Locked accounts: %d, Corrupted: %d\n",
               locked_accounts, corrupted_accounts);

        if (locked_accounts > 0) {
            printf("Main: WARNING - Accounts remain locked!\n");
        }

        sleep(3);
    }

    return 0;
}