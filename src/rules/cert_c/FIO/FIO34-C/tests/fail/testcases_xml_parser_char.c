/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: XML parser with char type cannot handle all XML characters
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void parse_tag(FILE *file) {
    char c; // VIOLATION: char type fails on extended character sets
    char tag_name[256];
    int pos = 0;

    // Skip '<'
    if ((c = fgetc(file)) != '<') {
        ungetc(c, file);
        return;
    }

    // Read tag name - will fail on XML with extended characters
    while ((c = fgetc(file)) != EOF && c != '>' && c != ' ') {
        if (pos < sizeof(tag_name) - 1) {
            tag_name[pos++] = c;
        }
    }

    tag_name[pos] = '\0';
    printf("Tag: <%s>\n", tag_name);

    // Skip to end of tag
    if (c != '>') {
        while ((c = fgetc(file)) != EOF && c != '>') {
            // Continue
        }
    }
}

void parse_text(FILE *file) {
    char c; // VIOLATION: char type cannot handle all text content
    char text[1024];
    int pos = 0;

    // Parse text content - will fail on multi-byte characters
    while ((c = fgetc(file)) != EOF && c != '<') {
        if (pos < sizeof(text) - 1 && !isspace(c)) {
            text[pos++] = c;
        }
    }

    if (pos > 0) {
        text[pos] = '\0';
        printf("Text: %s\n", text);
    }

    if (c == '<') {
        ungetc(c, file);
    }
}

int main() {
    FILE *file = fopen("document.xml", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open XML file\n");
        return 1;
    }

    char c;

    // Simple XML parsing - will fail on files with extended characters
    while ((c = fgetc(file)) != EOF) {
        if (c == '<') {
            ungetc(c, file);
            parse_tag(file);
        } else if (!isspace(c)) {
            ungetc(c, file);
            parse_text(file);
        }
    }

    fclose(file);
    return 0;
}