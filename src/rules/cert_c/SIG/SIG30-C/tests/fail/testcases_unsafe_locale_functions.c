/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

void locale_handler(int sig) {
    // VIOLATION: setlocale() is not async-safe
    char *old_locale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    // VIOLATION: localeconv() is not async-safe
    struct lconv *locale_info = localeconv();

    // VIOLATION: nl_langinfo() is not async-safe (if available)
#ifdef __USE_XOPEN2K
    char *codeset = nl_langinfo(CODESET);
#endif

    // VIOLATION: Using locale-dependent functions
    double value = 123.45;
    char buffer[100];

    // sprintf with locale formatting - not async-safe
    sprintf(buffer, "%.2f", value);

    // VIOLATION: Wide character functions that depend on locale
    wchar_t wide_buffer[50];
    mbstowcs(wide_buffer, "Hello", 50);
    wcstombs(buffer, wide_buffer, 100);
}

int main() {
    printf("Demonstrating unsafe locale functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, locale_handler);

    printf("Send SIGUSR1 to trigger unsafe locale operations\n");

    while (1) {
        pause();
    }

    return 0;
}