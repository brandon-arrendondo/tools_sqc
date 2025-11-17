/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: configuration_parsing_errors.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for configuration file parsing, leading to encoding
 * issues and incorrect data interpretation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    /* VIOLATION: Configuration data with signed char */
    signed char config_content[] =
        "# Configuration file\n"
        "server_name=MyServer\n"
        "port=8080\n"
        "debug=true\n"
        "path=/home/user/data\n"
        "encoding=UTF-8\n"
        "# Special characters: àáâãäå\n"
        "special_value=Café Münü\n";

    printf("Configuration parsing with signed char:\n");
    printf("Config content:\n%s\n", config_content);  /* Warning */

    /* VIOLATION: Line-by-line parsing with strtok */
    signed char *config_copy = malloc(strlen((char*)config_content) + 1);
    if (config_copy == NULL) return 1;

    strcpy(config_copy, config_content);  /* Warning */

    signed char *line = strtok(config_copy, "\n");  /* Warning */
    while (line != NULL) {
        /* Skip comments and empty lines */
        if (line[0] != '#' && line[0] != '\0') {
            printf("Processing line: %s\n", line);  /* Warning */

            /* VIOLATION: Key-value separation */
            signed char *equals = strchr(line, '=');  /* Warning */
            if (equals != NULL) {
                *equals = '\0';
                signed char *key = line;
                signed char *value = equals + 1;

                printf("  Key: %s\n", key);    /* Warning */
                printf("  Value: %s\n", value);  /* Warning */

                /* VIOLATION: Value type detection */
                if (strcmp((char*)value, "true") == 0 || strcmp((char*)value, "false") == 0) {
                    printf("  Type: boolean\n");
                } else if (strspn((char*)value, "0123456789") == strlen((char*)value)) {
                    printf("  Type: integer\n");
                } else {
                    printf("  Type: string\n");
                }
            }
        }
        line = strtok(NULL, "\n");  /* Warning */
    }

    free(config_copy);

    /* VIOLATION: INI file parsing simulation */
    printf("\nINI file parsing:\n");

    unsigned char ini_content[] =
        "[database]\n"
        "host=localhost\n"
        "user=admin\n"
        "password=secret123\n"
        "\n"
        "[logging]\n"
        "level=debug\n"
        "file=/var/log/app.log\n";

    printf("INI content:\n%s\n", ini_content);  /* Warning */

    /* VIOLATION: Section and key parsing */
    unsigned char *ini_copy = malloc(strlen((char*)ini_content) + 1);
    if (ini_copy == NULL) return 1;

    strcpy(ini_copy, ini_content);  /* Warning */

    unsigned char current_section[100] = "";
    unsigned char *ini_line = strtok(ini_copy, "\n");  /* Warning */

    while (ini_line != NULL) {
        /* Trim whitespace (simplified) */
        while (*ini_line == ' ' || *ini_line == '\t') ini_line++;

        if (*ini_line == '[') {
            /* VIOLATION: Section header parsing */
            unsigned char *section_end = strchr(ini_line, ']');  /* Warning */
            if (section_end != NULL) {
                *section_end = '\0';
                strcpy(current_section, ini_line + 1);  /* Warning */
                printf("Section: %s\n", current_section);  /* Warning */
            }
        } else if (*ini_line != '\0' && *ini_line != '#') {
            /* VIOLATION: Key-value parsing */
            unsigned char *ini_equals = strchr(ini_line, '=');  /* Warning */
            if (ini_equals != NULL) {
                *ini_equals = '\0';
                printf("  %s.%s = %s\n", current_section, ini_line, ini_equals + 1);  /* Warning */
            }
        }

        ini_line = strtok(NULL, "\n");  /* Warning */
    }

    free(ini_copy);

    /* VIOLATION: XML-style configuration parsing */
    printf("\nXML-style parsing:\n");

    char xml_config[] =
        "<config>\n"
        "  <server port=\"8080\">localhost</server>\n"
        "  <database host=\"db.example.com\" />\n"
        "  <features>\n"
        "    <logging enabled=\"true\" />\n"
        "  </features>\n"
        "</config>\n";

    printf("XML config:\n%s\n", xml_config);

    /* VIOLATION: Simple tag extraction */
    signed char *xml_copy = malloc(strlen(xml_config) + 1);
    if (xml_copy == NULL) return 1;

    strcpy(xml_copy, xml_config);  /* Warning */

    /* Find opening tags */
    signed char *tag_start = strchr(xml_copy, '<');  /* Warning */
    while (tag_start != NULL) {
        signed char *tag_end = strchr(tag_start, '>');  /* Warning */
        if (tag_end != NULL) {
            *tag_end = '\0';
            printf("Tag: %s>\n", tag_start);  /* Warning */

            /* Look for attributes */
            signed char *space = strchr(tag_start, ' ');  /* Warning */
            if (space != NULL) {
                printf("  Has attributes\n");
            }

            tag_start = strchr(tag_end + 1, '<');  /* Warning */
        } else {
            break;
        }
    }

    free(xml_copy);

    /* VIOLATION: Environment variable style parsing */
    printf("\nEnvironment variable parsing:\n");

    signed char env_config[] =
        "DATABASE_URL=postgresql://user:pass@localhost/db\n"
        "API_KEY=abc123def456\n"
        "DEBUG_MODE=1\n"
        "LOG_LEVEL=INFO\n";

    printf("Environment config:\n%s\n", env_config);  /* Warning */

    /* Parse environment variables */
    signed char *env_copy = malloc(strlen((char*)env_config) + 1);
    if (env_copy == NULL) return 1;

    strcpy(env_copy, env_config);  /* Warning */

    signed char *env_line = strtok(env_copy, "\n");  /* Warning */
    while (env_line != NULL) {
        signed char *env_equals = strchr(env_line, '=');  /* Warning */
        if (env_equals != NULL) {
            *env_equals = '\0';
            signed char *var_name = env_line;
            signed char *var_value = env_equals + 1;

            printf("Environment variable: %s=%s\n", var_name, var_value);  /* Warning */

            /* VIOLATION: URL parsing within environment variable */
            if (strncmp((char*)var_name, "DATABASE_URL", 12) == 0) {
                printf("  Parsing database URL...\n");
                signed char *proto_end = strstr(var_value, "://");  /* Warning */
                if (proto_end != NULL) {
                    *proto_end = '\0';
                    printf("    Protocol: %s\n", var_value);  /* Warning */
                }
            }
        }

        env_line = strtok(NULL, "\n");  /* Warning */
    }

    free(env_copy);

    /* VIOLATION: YAML-style parsing simulation */
    printf("\nYAML-style parsing:\n");

    unsigned char yaml_config[] =
        "server:\n"
        "  host: localhost\n"
        "  port: 8080\n"
        "database:\n"
        "  connection_string: \"postgresql://localhost/mydb\"\n"
        "  pool_size: 10\n";

    printf("YAML config:\n%s\n", yaml_config);  /* Warning */

    /* Simple indentation-based parsing */
    unsigned char *yaml_copy = malloc(strlen((char*)yaml_config) + 1);
    if (yaml_copy == NULL) return 1;

    strcpy(yaml_copy, yaml_config);  /* Warning */

    unsigned char *yaml_line = strtok(yaml_copy, "\n");  /* Warning */
    while (yaml_line != NULL) {
        /* Count leading spaces for indentation */
        int indent = 0;
        unsigned char *content = yaml_line;
        while (*content == ' ') {
            indent++;
            content++;
        }

        if (*content != '\0') {
            printf("Indent %d: %s\n", indent, content);  /* Warning */

            /* Look for key-value separator */
            unsigned char *colon = strchr(content, ':');  /* Warning */
            if (colon != NULL) {
                *colon = '\0';
                printf("  Key: %s\n", content);  /* Warning */
                if (*(colon + 1) != '\0') {
                    printf("  Value: %s\n", colon + 2);  /* Warning (skip ": ") */
                }
            }
        }

        yaml_line = strtok(NULL, "\n");  /* Warning */
    }

    free(yaml_copy);

    return 0;
}