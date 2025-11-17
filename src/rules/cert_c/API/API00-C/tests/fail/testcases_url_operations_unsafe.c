/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: url_operations_unsafe.c
 *
 * This case demonstrates violations where URL handling functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* URL components structure */
typedef struct {
    char *scheme;
    char *host;
    int port;
    char *path;
    char *query;
    char *fragment;
} URL;

/* NON-COMPLIANT: No validation of URL string */
URL *parse_url(const char *url_string) {
    /* No validation of url_string */
    URL *url = malloc(sizeof(URL));

    /* Mock parsing without validation */
    const char *scheme_end = strstr(url_string, "://");  /* url_string could be NULL */
    if (scheme_end) {
        size_t scheme_len = scheme_end - url_string;
        url->scheme = malloc(scheme_len + 1);
        strncpy(url->scheme, url_string, scheme_len);
        url->scheme[scheme_len] = '\0';
    } else {
        url->scheme = NULL;
    }

    url->host = NULL;
    url->port = 80;
    url->path = NULL;
    url->query = NULL;
    url->fragment = NULL;

    return url;
}

/* NON-COMPLIANT: No validation of URL encoding parameters */
char *url_encode(const char *input_string) {
    /* No validation of input_string */
    size_t input_len = strlen(input_string);  /* input_string could be NULL */
    char *encoded = malloc(input_len * 3 + 1);  /* Fixed multiplier without proper calculation */

    size_t encoded_pos = 0;
    for (size_t i = 0; i < input_len; i++) {
        char c = input_string[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded[encoded_pos++] = c;
        } else {
            sprintf(&encoded[encoded_pos], "%%%02X", (unsigned char)c);
            encoded_pos += 3;
        }
    }
    encoded[encoded_pos] = '\0';

    return encoded;
}

/* NON-COMPLIANT: No validation of URL decoding parameters */
char *url_decode(const char *encoded_string) {
    /* No validation of encoded_string */
    size_t encoded_len = strlen(encoded_string);  /* encoded_string could be NULL */
    char *decoded = malloc(encoded_len + 1);

    size_t decoded_pos = 0;
    for (size_t i = 0; i < encoded_len; i++) {
        if (encoded_string[i] == '%' && i + 2 < encoded_len) {
            /* No validation of hex digits */
            char hex_str[3] = {encoded_string[i + 1], encoded_string[i + 2], '\0'};
            decoded[decoded_pos++] = (char)strtol(hex_str, NULL, 16);
            i += 2;
        } else {
            decoded[decoded_pos++] = encoded_string[i];
        }
    }
    decoded[decoded_pos] = '\0';

    return decoded;
}

/* NON-COMPLIANT: No validation of URL building parameters */
char *build_url(const char *scheme, const char *host, int port, const char *path, const char *query) {
    /* No validation of any parameters */
    char *url = malloc(2048);  /* Fixed size without checking combined length */

    sprintf(url, "%s://%s", scheme, host);  /* scheme and host could be NULL */

    if (port != 80 && port != 443) {  /* No validation of port range */
        sprintf(url + strlen(url), ":%d", port);
    }

    if (path) {
        if (path[0] != '/') {
            strcat(url, "/");
        }
        strcat(url, path);
    }

    if (query) {
        strcat(url, "?");
        strcat(url, query);
    }

    return url;
}

/* NON-COMPLIANT: No validation of query parameter parsing */
char *get_query_parameter(const char *query_string, const char *param_name) {
    /* No validation of query_string or param_name */
    char *param_start = strstr(query_string, param_name);  /* query_string could be NULL */

    if (!param_start) {
        return NULL;
    }

    param_start += strlen(param_name);  /* param_name could be NULL */
    if (*param_start != '=') {
        return NULL;
    }
    param_start++;

    char *param_end = strchr(param_start, '&');
    size_t param_len = param_end ? (size_t)(param_end - param_start) : strlen(param_start);

    char *param_value = malloc(param_len + 1);
    strncpy(param_value, param_start, param_len);
    param_value[param_len] = '\0';

    return param_value;
}

/* NON-COMPLIANT: No validation of URL validation */
int is_valid_url(const char *url_string) {
    /* No validation of url_string */
    return strstr(url_string, "://") != NULL;  /* url_string could be NULL */
}

/* NON-COMPLIANT: No validation of URL normalization */
char *normalize_url(const char *url_string) {
    /* No validation of url_string */
    char *normalized = malloc(strlen(url_string) + 1);  /* url_string could be NULL */
    strcpy(normalized, url_string);

    /* Mock normalization - convert to lowercase */
    for (char *p = normalized; *p; p++) {
        *p = tolower(*p);
    }

    return normalized;
}

/* NON-COMPLIANT: No validation of path resolution */
char *resolve_relative_path(const char *base_path, const char *relative_path) {
    /* No validation of base_path or relative_path */
    if (relative_path[0] == '/') {  /* relative_path could be NULL */
        char *resolved = malloc(strlen(relative_path) + 1);
        strcpy(resolved, relative_path);
        return resolved;
    }

    size_t base_len = strlen(base_path);  /* base_path could be NULL */
    size_t relative_len = strlen(relative_path);
    char *resolved = malloc(base_len + relative_len + 2);

    strcpy(resolved, base_path);
    strcat(resolved, "/");
    strcat(resolved, relative_path);

    return resolved;
}

/* NON-COMPLIANT: No validation of domain extraction */
char *extract_domain(const char *url_string) {
    /* No validation of url_string */
    const char *domain_start = strstr(url_string, "://");  /* url_string could be NULL */

    if (!domain_start) {
        return NULL;
    }

    domain_start += 3;
    const char *domain_end = strchr(domain_start, '/');
    if (!domain_end) {
        domain_end = strchr(domain_start, ':');
    }
    if (!domain_end) {
        domain_end = domain_start + strlen(domain_start);
    }

    size_t domain_len = domain_end - domain_start;
    char *domain = malloc(domain_len + 1);
    strncpy(domain, domain_start, domain_len);
    domain[domain_len] = '\0';

    return domain;
}

int main(void) {
    char *null_string = NULL;
    URL *null_url = NULL;

    /* Examples of dangerous URL operations */
    // parse_url(null_string);  /* NULL URL string */
    // url_encode(null_string);  /* NULL input */
    // url_decode(null_string);  /* NULL encoded string */
    // build_url(null_string, null_string, -1, null_string, null_string);  /* NULL parameters */
    // get_query_parameter(null_string, null_string);  /* NULL parameters */
    // is_valid_url(null_string);  /* NULL URL */
    // normalize_url(null_string);  /* NULL URL */
    // resolve_relative_path(null_string, null_string);  /* NULL paths */
    // extract_domain(null_string);  /* NULL URL */

    printf("URL functions compiled but lack parameter validation\n");
    return 0;
}