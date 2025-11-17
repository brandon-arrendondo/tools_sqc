/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <unistd.h>

void wchar_handler(int sig) {
    wchar_t wide_buffer[100];
    char narrow_buffer[200];

    // VIOLATION: Wide character I/O functions are not async-safe
    wprintf(L"Signal %d received\n", sig);
    fwprintf(stderr, L"Wide character signal handler\n");

    // VIOLATION: swprintf() is not async-safe
    swprintf(wide_buffer, 100, L"Signal number: %d", sig);

    // VIOLATION: Wide character string functions
    wcscpy(wide_buffer, L"Signal ");
    wcscat(wide_buffer, L"received");
    size_t len = wcslen(wide_buffer);

    // VIOLATION: Wide character conversion functions
    mbstowcs(wide_buffer, "Hello", 100);
    wcstombs(narrow_buffer, wide_buffer, 200);

    // VIOLATION: Wide character classification functions
    for (size_t i = 0; i < len; i++) {
        if (iswalpha(wide_buffer[i])) {
            wide_buffer[i] = towupper(wide_buffer[i]);
        }
    }

    // VIOLATION: Wide character comparison functions
    int cmp = wcscmp(wide_buffer, L"SIGNAL RECEIVED");
    cmp = wcsncmp(wide_buffer, L"SIGNAL", 6);

    // VIOLATION: Wide character search functions
    wchar_t *found = wcschr(wide_buffer, L'S');
    found = wcsstr(wide_buffer, L"SIGNAL");

    // VIOLATION: Wide character locale-dependent functions
    wcscoll(wide_buffer, L"test");
    wcsxfrm(wide_buffer, L"transform", 100);
}

int main() {
    printf("Demonstrating unsafe wide character functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, wchar_handler);

    printf("Send SIGUSR1 to trigger unsafe wide character operations\n");

    while (1) {
        pause();
    }

    return 0;
}