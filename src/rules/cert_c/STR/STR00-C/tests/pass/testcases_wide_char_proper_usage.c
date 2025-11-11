/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: wide_char_proper_usage.c
 *
 * This case demonstrates compliant code that properly uses wide character
 * types (wchar_t) for international text and Unicode data, following
 * STR00-C guidelines for natural-language character data.
 */

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <wctype.h>

int main(void) {
    /* COMPLIANT: Set locale to support wide characters */
    setlocale(LC_ALL, "");

    printf("Proper wide character usage:\n\n");

    /* COMPLIANT: Using wchar_t for international text */
    wchar_t greeting[] = L"Hello, 世界! Bonjour le monde!";
    wchar_t name[] = L"José María González";
    wchar_t buffer[200];

    wprintf(L"Greeting: %ls\n", greeting);
    wprintf(L"Name: %ls\n", name);

    /* COMPLIANT: Wide string operations */
    wcscpy(buffer, L"Welcome, ");
    wcscat(buffer, name);
    wcscat(buffer, L"!");

    wprintf(L"Welcome message: %ls\n", buffer);

    /* COMPLIANT: Wide string length and comparison */
    size_t greeting_len = wcslen(greeting);
    size_t name_len = wcslen(name);

    printf("Greeting length: %zu characters\n", greeting_len);
    printf("Name length: %zu characters\n", name_len);

    if (wcscmp(name, L"José María González") == 0) {
        wprintf(L"Name matches expected value\n");
    }

    /* COMPLIANT: Wide character manipulation */
    wchar_t text[] = L"Café, naïve, résumé, Zürich";
    wprintf(L"Original: %ls\n", text);

    /* Convert to uppercase using wide character functions */
    for (size_t i = 0; text[i] != L'\0'; i++) {
        text[i] = towupper(text[i]);
    }

    wprintf(L"Uppercase: %ls\n", text);

    /* COMPLIANT: Wide character classification */
    wchar_t test_chars[] = L"Hello123αβγñáéíóú中文日本語🌍";

    wprintf(L"\nWide character classification:\n");
    for (size_t i = 0; test_chars[i] != L'\0'; i++) {
        wchar_t wc = test_chars[i];

        wprintf(L"Character '%lc': ", wc);

        if (iswalpha(wc)) {
            wprintf(L"alphabetic ");
        }
        if (iswdigit(wc)) {
            wprintf(L"digit ");
        }
        if (iswpunct(wc)) {
            wprintf(L"punctuation ");
        }
        if (iswprint(wc)) {
            wprintf(L"printable ");
        }

        wprintf(L"\n");
    }

    /* COMPLIANT: Wide character searching */
    wchar_t *found = wcschr(greeting, L'世');
    if (found != NULL) {
        wprintf(L"Found Chinese character at position: %ld\n", found - greeting);
    }

    /* COMPLIANT: Wide string tokenization */
    wchar_t data[] = L"apple,banana,café,naïve,résumé";
    wchar_t *context;
    wchar_t *token = wcstok(data, L",", &context);

    wprintf(L"Tokens: ");
    while (token != NULL) {
        wprintf(L"'%ls' ", token);
        token = wcstok(NULL, L",", &context);
    }
    wprintf(L"\n");

    /* COMPLIANT: Wide character constants */
    wchar_t euro_sign = L'€';
    wchar_t yen_sign = L'¥';
    wchar_t pound_sign = L'£';

    wprintf(L"Currency symbols: %lc %lc %lc\n", euro_sign, yen_sign, pound_sign);

    /* COMPLIANT: Formatted wide string operations */
    wchar_t formatted[100];
    swprintf(formatted, sizeof(formatted)/sizeof(wchar_t),
             L"Price: %lc%.2f", euro_sign, 29.99);

    wprintf(L"Formatted string: %ls\n", formatted);

    /* COMPLIANT: Wide character file I/O */
    const char *filename = "wide_test.txt";
    FILE *file = fopen(filename, "w");

    if (file != NULL) {
        /* Write wide characters to file */
        fwprintf(file, L"International text: %ls\n", greeting);
        fwprintf(file, L"Name: %ls\n", name);
        fwprintf(file, L"Symbols: %lc %lc %lc\n", euro_sign, yen_sign, pound_sign);

        fclose(file);

        /* Read wide characters from file */
        file = fopen(filename, "r");
        if (file != NULL) {
            wchar_t line[200];

            wprintf(L"\nReading from file:\n");
            while (fgetws(line, sizeof(line)/sizeof(wchar_t), file) != NULL) {
                wprintf(L"%ls", line);
            }

            fclose(file);
        }

        /* Clean up */
        remove(filename);
    }

    /* COMPLIANT: Wide character comparison and sorting */
    wchar_t *words[] = {
        L"apple", L"café", L"naïve", L"résumé", L"zebra"
    };
    int word_count = 5;

    wprintf(L"\nWord comparison:\n");
    for (int i = 0; i < word_count - 1; i++) {
        for (int j = i + 1; j < word_count; j++) {
            int cmp = wcscmp(words[i], words[j]);
            wprintf(L"'%ls' vs '%ls': %s\n",
                   words[i], words[j],
                   (cmp < 0) ? L"less" : (cmp > 0) ? L"greater" : L"equal");
        }
    }

    /* COMPLIANT: Wide character numeric conversion */
    wchar_t number_str[] = L"12345";
    long number = wcstol(number_str, NULL, 10);

    wprintf(L"Number string '%ls' converted to: %ld\n", number_str, number);

    /* COMPLIANT: Character set validation */
    wchar_t multilingual[] = L"English, Español, Français, Deutsch, 中文, 日本語, العربية";

    wprintf(L"\nMultilingual text: %ls\n", multilingual);
    wprintf(L"Length: %zu characters\n", wcslen(multilingual));

    /* Count different character types */
    int latin_count = 0;
    int space_count = 0;
    int punct_count = 0;
    int other_count = 0;

    for (size_t i = 0; multilingual[i] != L'\0'; i++) {
        wchar_t wc = multilingual[i];

        if (iswspace(wc)) {
            space_count++;
        } else if (iswpunct(wc)) {
            punct_count++;
        } else if (wc < 0x80) {  /* Basic Latin */
            latin_count++;
        } else {
            other_count++;
        }
    }

    printf("Character type counts:\n");
    printf("  Basic Latin: %d\n", latin_count);
    printf("  Spaces: %d\n", space_count);
    printf("  Punctuation: %d\n", punct_count);
    printf("  Other Unicode: %d\n", other_count);

    /* COMPLIANT: Wide character case conversion */
    wchar_t mixed_case[] = L"MiXeD CaSe InTeRnAtIoNaL: Café Münü";
    wprintf(L"Original case: %ls\n", mixed_case);

    for (size_t i = 0; mixed_case[i] != L'\0'; i++) {
        if (iswlower(mixed_case[i])) {
            mixed_case[i] = towupper(mixed_case[i]);
        } else if (iswupper(mixed_case[i])) {
            mixed_case[i] = towlower(mixed_case[i]);
        }
    }

    wprintf(L"Case swapped: %ls\n", mixed_case);

    return 0;
}