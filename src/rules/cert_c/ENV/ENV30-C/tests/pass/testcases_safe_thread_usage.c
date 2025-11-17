/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_thread_usage.c
 *
 * This case demonstrates compliant usage of ENV30-C functions
 * in multithreaded environments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/* Thread-safe data structure for passing information */
typedef struct {
    int thread_id;
    char env_copy[256];
    char time_copy[64];
    char error_copy[256];
} ThreadSafeData;

/* COMPLIANT: Thread-safe environment variable access */
void *safe_thread_env_access(void *arg) {
    ThreadSafeData *data = (ThreadSafeData *)arg;

    /* Get environment variable and copy immediately */
    const char *env_value = getenv("SHARED_VAR");
    if (env_value != NULL) {
        strncpy(data->env_copy, env_value, sizeof(data->env_copy) - 1);
        data->env_copy[sizeof(data->env_copy) - 1] = '\0';
    } else {
        strcpy(data->env_copy, "(not set)");
    }

    printf("Thread %d safely copied env: %s\n", data->thread_id, data->env_copy);
    return NULL;
}

/* COMPLIANT: Thread-safe time function access */
void *safe_thread_time_access(void *arg) {
    ThreadSafeData *data = (ThreadSafeData *)arg;

    time_t now = time(NULL);

    /* Use strftime for thread-safe time formatting */
    struct tm time_info;
    if (localtime_r(&now, &time_info) != NULL) {
        strftime(data->time_copy, sizeof(data->time_copy),
                "%Y-%m-%d %H:%M:%S", &time_info);
    } else {
        strcpy(data->time_copy, "(time error)");
    }

    printf("Thread %d safely formatted time: %s\n", data->thread_id, data->time_copy);
    return NULL;
}

/* COMPLIANT: Thread-safe error handling */
void *safe_thread_error_handling(void *arg) {
    ThreadSafeData *data = (ThreadSafeData *)arg;

    /* Generate an error */
    errno = EACCES;

    /* Use thread-safe strerror_r */
    char error_buffer[256];
#ifdef _GNU_SOURCE
    char *result = strerror_r(errno, error_buffer, sizeof(error_buffer));
    strncpy(data->error_copy, result, sizeof(data->error_copy) - 1);
#else
    int result = strerror_r(errno, error_buffer, sizeof(error_buffer));
    if (result == 0) {
        strncpy(data->error_copy, error_buffer, sizeof(data->error_copy) - 1);
    } else {
        strcpy(data->error_copy, "(error in strerror_r)");
    }
#endif
    data->error_copy[sizeof(data->error_copy) - 1] = '\0';

    printf("Thread %d safely handled error: %s\n", data->thread_id, data->error_copy);
    return NULL;
}

/* COMPLIANT: Thread-safe locale handling */
void safe_threaded_locale_demo(void) {
    printf("Thread-safe locale handling:\n");

    /* In a real application, locale should be set once at startup */
    /* Here we demonstrate safe reading in multiple threads */

    pthread_t threads[3];
    ThreadSafeData thread_data[3];

    for (int i = 0; i < 3; i++) {
        thread_data[i].thread_id = i + 1;

        if (i == 0) {
            pthread_create(&threads[i], NULL, safe_thread_env_access, &thread_data[i]);
        } else if (i == 1) {
            pthread_create(&threads[i], NULL, safe_thread_time_access, &thread_data[i]);
        } else {
            pthread_create(&threads[i], NULL, safe_thread_error_handling, &thread_data[i]);
        }
    }

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Display results */
    printf("Thread results:\n");
    for (int i = 0; i < 3; i++) {
        printf("  Thread %d:\n", thread_data[i].thread_id);
        if (strlen(thread_data[i].env_copy) > 0) {
            printf("    Env: %s\n", thread_data[i].env_copy);
        }
        if (strlen(thread_data[i].time_copy) > 0) {
            printf("    Time: %s\n", thread_data[i].time_copy);
        }
        if (strlen(thread_data[i].error_copy) > 0) {
            printf("    Error: %s\n", thread_data[i].error_copy);
        }
    }
}

int main(void) {
    printf("=== ENV30-C Safe Thread Usage Demo ===\n");

    /* Set up environment for testing */
    setenv("SHARED_VAR", "thread_safe_value", 1);

    safe_threaded_locale_demo();

    return 0;
}