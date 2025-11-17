/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: config_parser_unchecked.c
 *
 * This case demonstrates violations where configuration parsing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Configuration entry structure */
typedef struct ConfigEntry {
    char *key;
    char *value;
    struct ConfigEntry *next;
} ConfigEntry;

/* Configuration section structure */
typedef struct ConfigSection {
    char *section_name;
    ConfigEntry *entries;
    struct ConfigSection *next;
} ConfigSection;

/* NON-COMPLIANT: No validation of config file path */
ConfigSection *load_config_file(const char *config_file_path) {
    /* No validation of config_file_path */
    FILE *file = fopen(config_file_path, "r");  /* config_file_path could be NULL */

    if (!file) {
        return NULL;  /* But we already tried to open NULL path */
    }

    ConfigSection *root = malloc(sizeof(ConfigSection));
    root->section_name = malloc(strlen("default") + 1);
    strcpy(root->section_name, "default");
    root->entries = NULL;
    root->next = NULL;

    fclose(file);
    return root;
}

/* NON-COMPLIANT: No validation of section or key parameters */
const char *get_config_value(ConfigSection *sections, const char *section_name, const char *key) {
    /* No validation of any parameters */
    ConfigSection *current_section = sections;

    while (current_section) {  /* sections could be NULL */
        if (strcmp(current_section->section_name, section_name) == 0) {  /* section_name could be NULL */
            ConfigEntry *entry = current_section->entries;
            while (entry) {
                if (strcmp(entry->key, key) == 0) {  /* key could be NULL */
                    return entry->value;
                }
                entry = entry->next;
            }
        }
        current_section = current_section->next;
    }

    return NULL;
}

/* NON-COMPLIANT: No validation of setting parameters */
void set_config_value(ConfigSection *sections, const char *section_name, const char *key, const char *value) {
    /* No validation of any parameters */
    ConfigSection *section = sections;

    /* Find or create section */
    while (section && strcmp(section->section_name, section_name) != 0) {  /* section_name could be NULL */
        section = section->next;
    }

    if (!section) {
        section = malloc(sizeof(ConfigSection));
        section->section_name = malloc(strlen(section_name) + 1);  /* section_name could be NULL */
        strcpy(section->section_name, section_name);
        section->entries = NULL;
        section->next = sections->next;  /* sections could be NULL */
        sections->next = section;
    }

    /* Add or update entry */
    ConfigEntry *entry = malloc(sizeof(ConfigEntry));
    entry->key = malloc(strlen(key) + 1);  /* key could be NULL */
    strcpy(entry->key, key);
    entry->value = malloc(strlen(value) + 1);  /* value could be NULL */
    strcpy(entry->value, value);
    entry->next = section->entries;
    section->entries = entry;
}

/* NON-COMPLIANT: No validation of integer conversion */
int get_config_int(ConfigSection *sections, const char *section_name, const char *key, int default_value) {
    /* No validation of parameters */
    const char *str_value = get_config_value(sections, section_name, key);

    if (!str_value) {
        return default_value;
    }

    /* No validation of str_value format */
    return atoi(str_value);  /* str_value could contain non-numeric data */
}

/* NON-COMPLIANT: No validation of boolean conversion */
int get_config_bool(ConfigSection *sections, const char *section_name, const char *key, int default_value) {
    /* No validation of parameters */
    const char *str_value = get_config_value(sections, section_name, key);

    if (!str_value) {
        return default_value;
    }

    /* No validation of str_value format */
    return (strcmp(str_value, "true") == 0 || strcmp(str_value, "1") == 0);  /* str_value could be NULL */
}

/* NON-COMPLIANT: No validation of save parameters */
int save_config_file(ConfigSection *sections, const char *config_file_path) {
    /* No validation of sections or config_file_path */
    FILE *file = fopen(config_file_path, "w");  /* config_file_path could be NULL */

    if (!file) {
        return -1;
    }

    ConfigSection *section = sections;
    while (section) {  /* sections could be NULL */
        fprintf(file, "[%s]\n", section->section_name);

        ConfigEntry *entry = section->entries;
        while (entry) {
            fprintf(file, "%s=%s\n", entry->key, entry->value);
            entry = entry->next;
        }

        fprintf(file, "\n");
        section = section->next;
    }

    fclose(file);
    return 0;
}

/* NON-COMPLIANT: No validation of array parsing */
char **get_config_array(ConfigSection *sections, const char *section_name, const char *key, const char *delimiter) {
    /* No validation of any parameters */
    const char *str_value = get_config_value(sections, section_name, key);

    if (!str_value) {
        return NULL;
    }

    /* No validation of delimiter */
    char *value_copy = malloc(strlen(str_value) + 1);  /* str_value could be NULL */
    strcpy(value_copy, str_value);

    char **array = malloc(100 * sizeof(char *));  /* Fixed size without checking */
    int count = 0;

    char *token = strtok(value_copy, delimiter);  /* delimiter could be NULL */
    while (token && count < 100) {
        array[count] = malloc(strlen(token) + 1);
        strcpy(array[count], token);
        count++;
        token = strtok(NULL, delimiter);
    }

    array[count] = NULL;
    free(value_copy);
    return array;
}

/* NON-COMPLIANT: No validation of merge parameters */
void merge_config_sections(ConfigSection *target, ConfigSection *source) {
    /* No validation of target or source */
    ConfigSection *src_section = source;

    while (src_section) {  /* source could be NULL */
        ConfigEntry *entry = src_section->entries;
        while (entry) {
            set_config_value(target, src_section->section_name, entry->key, entry->value);
            entry = entry->next;
        }
        src_section = src_section->next;
    }
}

int main(void) {
    ConfigSection *null_sections = NULL;
    char *null_string = NULL;

    /* Examples of dangerous config operations */
    // load_config_file(null_string);  /* NULL file path */
    // get_config_value(null_sections, null_string, null_string);  /* NULL parameters */
    // set_config_value(null_sections, null_string, null_string, null_string);  /* NULL parameters */
    // get_config_int(null_sections, null_string, null_string, 0);  /* NULL parameters */
    // get_config_bool(null_sections, null_string, null_string, 0);  /* NULL parameters */
    // save_config_file(null_sections, null_string);  /* NULL parameters */
    // get_config_array(null_sections, null_string, null_string, null_string);  /* NULL parameters */
    // merge_config_sections(null_sections, null_sections);  /* NULL parameters */

    printf("Config functions compiled but lack parameter validation\n");
    return 0;
}