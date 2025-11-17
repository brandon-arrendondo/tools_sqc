/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;
    char *metadata;
    FILE *logfile;
} resource_bundle_t;

resource_bundle_t* create_resource_bundle(size_t data_size, const char *log_filename) {
    resource_bundle_t *bundle = malloc(sizeof(resource_bundle_t));
    if (!bundle) {
        printf("Failed to allocate bundle\n");
        return NULL;
    }

    // Initialize to safe state
    bundle->data = NULL;
    bundle->metadata = NULL;
    bundle->logfile = NULL;

    // Allocate data array
    bundle->data = malloc(data_size * sizeof(int));
    if (!bundle->data) {
        printf("Failed to allocate data array\n");
        goto cleanup_bundle;
    }

    // Allocate metadata
    bundle->metadata = malloc(256);
    if (!bundle->metadata) {
        printf("Failed to allocate metadata\n");
        goto cleanup_data;
    }

    // Open log file
    if (log_filename) {
        bundle->logfile = fopen(log_filename, "w");
        if (!bundle->logfile) {
            printf("Failed to open log file\n");
            goto cleanup_metadata;
        }
    }

    printf("Resource bundle created successfully\n");
    return bundle;

    // Error cleanup paths - each resource freed exactly once
cleanup_metadata:
    free(bundle->metadata);
    bundle->metadata = NULL;

cleanup_data:
    free(bundle->data);
    bundle->data = NULL;

cleanup_bundle:
    free(bundle);
    return NULL;
}

void destroy_resource_bundle(resource_bundle_t **bundle) {
    if (!bundle || !*bundle) {
        return;
    }

    // Close file if open
    if ((*bundle)->logfile) {
        fclose((*bundle)->logfile);
        (*bundle)->logfile = NULL;
    }

    // Free allocated memory exactly once
    if ((*bundle)->metadata) {
        free((*bundle)->metadata);
        (*bundle)->metadata = NULL;
    }

    if ((*bundle)->data) {
        free((*bundle)->data);
        (*bundle)->data = NULL;
    }

    free(*bundle);
    *bundle = NULL;

    printf("Resource bundle destroyed\n");
}

int main() {
    // Test successful creation and cleanup
    resource_bundle_t *bundle1 = create_resource_bundle(100, "/tmp/test.log");
    if (bundle1) {
        destroy_resource_bundle(&bundle1);
    }

    // Test creation failure scenarios
    resource_bundle_t *bundle2 = create_resource_bundle(SIZE_MAX, NULL);  // Should fail
    if (bundle2) {
        destroy_resource_bundle(&bundle2);
    }

    return 0;
}