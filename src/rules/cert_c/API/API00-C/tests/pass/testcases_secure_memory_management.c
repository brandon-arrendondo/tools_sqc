/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: secure_memory_management.c
 *
 * This case demonstrates compliant memory management functions
 * with comprehensive parameter validation and safe operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

/* Memory allocation tracking structure */
typedef struct MemoryBlock {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    struct MemoryBlock *next;
} MemoryBlock;

/* Global tracking (for demonstration only - in practice use thread-local storage) */
static MemoryBlock *allocated_blocks = NULL;
static size_t total_allocated = 0;
static size_t max_allocation_limit = 100 * 1024 * 1024;  /* 100 MB limit */

/* COMPLIANT: Safe memory allocation with validation */
void *safe_malloc(size_t size, const char *file, int line) {
    /* Validate parameters */
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Check for unreasonable allocation sizes */
    const size_t MAX_SINGLE_ALLOC = 50 * 1024 * 1024;  /* 50 MB */
    if (size > MAX_SINGLE_ALLOC) {
        errno = ERANGE;
        return NULL;
    }

    /* Check total memory limit */
    if (total_allocated + size > max_allocation_limit) {
        errno = ENOMEM;
        return NULL;
    }

    /* Check for size overflow when adding tracking overhead */
    if (size > SIZE_MAX - sizeof(MemoryBlock)) {
        errno = ERANGE;
        return NULL;
    }

    /* Attempt allocation */
    void *ptr = malloc(size);
    if (!ptr) {
        errno = ENOMEM;
        return NULL;
    }

    /* Create tracking block */
    MemoryBlock *block = malloc(sizeof(MemoryBlock));
    if (!block) {
        free(ptr);
        errno = ENOMEM;
        return NULL;
    }

    /* Initialize tracking */
    block->ptr = ptr;
    block->size = size;
    block->file = file;
    block->line = line;
    block->next = allocated_blocks;
    allocated_blocks = block;
    total_allocated += size;

    /* Clear allocated memory for security */
    memset(ptr, 0, size);

    return ptr;
}

/* COMPLIANT: Safe memory reallocation with validation */
void *safe_realloc(void *ptr, size_t new_size, const char *file, int line) {
    /* Handle realloc(NULL, size) case */
    if (!ptr) {
        return safe_malloc(new_size, file, line);
    }

    /* Handle realloc(ptr, 0) case */
    if (new_size == 0) {
        safe_free(ptr, file, line);
        return NULL;
    }

    /* Find existing block */
    MemoryBlock *block = allocated_blocks;
    while (block && block->ptr != ptr) {
        block = block->next;
    }

    if (!block) {
        errno = EINVAL;  /* Pointer not found in our tracking */
        return NULL;
    }

    /* Validate new size */
    const size_t MAX_SINGLE_ALLOC = 50 * 1024 * 1024;  /* 50 MB */
    if (new_size > MAX_SINGLE_ALLOC) {
        errno = ERANGE;
        return NULL;
    }

    /* Check if new allocation would exceed limits */
    size_t old_size = block->size;
    if (new_size > old_size) {
        size_t size_increase = new_size - old_size;
        if (total_allocated + size_increase > max_allocation_limit) {
            errno = ENOMEM;
            return NULL;
        }
    }

    /* Attempt reallocation */
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr) {
        errno = ENOMEM;
        return NULL;  /* Original pointer still valid */
    }

    /* Update tracking */
    block->ptr = new_ptr;
    total_allocated = total_allocated - old_size + new_size;
    block->size = new_size;
    block->file = file;
    block->line = line;

    /* Clear any newly allocated space */
    if (new_size > old_size) {
        memset((char *)new_ptr + old_size, 0, new_size - old_size);
    }

    return new_ptr;
}

/* COMPLIANT: Safe memory deallocation with validation */
void safe_free(void *ptr, const char *file, int line) {
    /* Handle free(NULL) - this is safe and allowed */
    if (!ptr) {
        return;
    }

    /* Find block in tracking list */
    MemoryBlock *current = allocated_blocks;
    MemoryBlock *previous = NULL;

    while (current && current->ptr != ptr) {
        previous = current;
        current = current->next;
    }

    if (!current) {
        /* Pointer not found - this is an error but we can't do much */
        fprintf(stderr, "Warning: Attempt to free untracked pointer at %s:%d\n", file, line);
        return;
    }

    /* Clear memory before freeing for security */
    memset(ptr, 0, current->size);

    /* Remove from tracking list */
    if (previous) {
        previous->next = current->next;
    } else {
        allocated_blocks = current->next;
    }

    /* Update totals */
    total_allocated -= current->size;

    /* Free actual memory */
    free(ptr);
    free(current);
}

/* COMPLIANT: Safe memory copy with overlap detection */
int safe_memory_copy(void *dest, size_t dest_size, const void *src, size_t src_size, size_t copy_size) {
    /* Validate parameters */
    if (!dest || !src) {
        errno = EINVAL;
        return -1;
    }

    if (dest_size == 0 || src_size == 0 || copy_size == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Validate copy size doesn't exceed buffer sizes */
    if (copy_size > dest_size || copy_size > src_size) {
        errno = ERANGE;
        return -1;
    }

    /* Check for buffer overlap */
    const char *src_bytes = (const char *)src;
    char *dest_bytes = (char *)dest;

    /* Detect overlap */
    if ((src_bytes < dest_bytes && src_bytes + copy_size > dest_bytes) ||
        (dest_bytes < src_bytes && dest_bytes + copy_size > src_bytes)) {
        /* Use memmove for overlapping regions */
        memmove(dest, src, copy_size);
    } else {
        /* Safe to use memcpy for non-overlapping regions */
        memcpy(dest, src, copy_size);
    }

    return 0;
}

/* COMPLIANT: Safe memory comparison with validation */
int safe_memory_compare(const void *ptr1, const void *ptr2, size_t size, int *result) {
    /* Validate parameters */
    if (!ptr1 || !ptr2 || !result) {
        errno = EINVAL;
        return -1;
    }

    if (size == 0) {
        *result = 0;  /* Zero-length regions are equal */
        return 0;
    }

    /* Check for reasonable comparison size */
    const size_t MAX_COMPARE_SIZE = 10 * 1024 * 1024;  /* 10 MB */
    if (size > MAX_COMPARE_SIZE) {
        errno = ERANGE;
        return -1;
    }

    /* Perform comparison */
    *result = memcmp(ptr1, ptr2, size);
    return 0;
}

/* COMPLIANT: Safe memory zeroing with validation */
int safe_memory_zero(void *ptr, size_t size) {
    /* Validate parameters */
    if (!ptr) {
        errno = EINVAL;
        return -1;
    }

    if (size == 0) {
        return 0;  /* Nothing to zero */
    }

    /* Check for reasonable size */
    const size_t MAX_ZERO_SIZE = 100 * 1024 * 1024;  /* 100 MB */
    if (size > MAX_ZERO_SIZE) {
        errno = ERANGE;
        return -1;
    }

    /* Clear memory */
    memset(ptr, 0, size);
    return 0;
}

/* COMPLIANT: Safe aligned memory allocation */
void *safe_aligned_alloc(size_t alignment, size_t size, const char *file, int line) {
    /* Validate parameters */
    if (alignment == 0 || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Check that alignment is a power of 2 */
    if ((alignment & (alignment - 1)) != 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Check for reasonable values */
    const size_t MAX_ALIGNMENT = 4096;  /* 4KB page alignment max */
    const size_t MAX_ALIGNED_SIZE = 50 * 1024 * 1024;  /* 50 MB */

    if (alignment > MAX_ALIGNMENT || size > MAX_ALIGNED_SIZE) {
        errno = ERANGE;
        return NULL;
    }

    /* Check total memory limit */
    if (total_allocated + size > max_allocation_limit) {
        errno = ENOMEM;
        return NULL;
    }

    /* Attempt aligned allocation */
    void *ptr;
    int result = posix_memalign(&ptr, alignment, size);
    if (result != 0) {
        errno = result;
        return NULL;
    }

    /* Add to tracking */
    MemoryBlock *block = malloc(sizeof(MemoryBlock));
    if (!block) {
        free(ptr);
        errno = ENOMEM;
        return NULL;
    }

    block->ptr = ptr;
    block->size = size;
    block->file = file;
    block->line = line;
    block->next = allocated_blocks;
    allocated_blocks = block;
    total_allocated += size;

    /* Clear allocated memory */
    memset(ptr, 0, size);

    return ptr;
}

/* COMPLIANT: Memory usage reporting */
void safe_memory_report(void) {
    printf("Memory Usage Report:\n");
    printf("  Total allocated: %zu bytes\n", total_allocated);
    printf("  Allocation limit: %zu bytes\n", max_allocation_limit);
    printf("  Available: %zu bytes\n", max_allocation_limit - total_allocated);

    size_t block_count = 0;
    MemoryBlock *block = allocated_blocks;
    while (block) {
        block_count++;
        block = block->next;
    }
    printf("  Active allocations: %zu\n", block_count);

    if (block_count > 0) {
        printf("  Active blocks:\n");
        block = allocated_blocks;
        while (block) {
            printf("    %p: %zu bytes (%s:%d)\n",
                   block->ptr, block->size, block->file, block->line);
            block = block->next;
        }
    }
}

/* COMPLIANT: Cleanup all tracked memory */
void safe_memory_cleanup(void) {
    MemoryBlock *current = allocated_blocks;
    while (current) {
        MemoryBlock *next = current->next;

        /* Clear memory before freeing */
        memset(current->ptr, 0, current->size);
        free(current->ptr);
        free(current);

        current = next;
    }

    allocated_blocks = NULL;
    total_allocated = 0;
}

/* Convenient macros for tracking allocation location */
#define SAFE_MALLOC(size) safe_malloc((size), __FILE__, __LINE__)
#define SAFE_REALLOC(ptr, size) safe_realloc((ptr), (size), __FILE__, __LINE__)
#define SAFE_FREE(ptr) safe_free((ptr), __FILE__, __LINE__)
#define SAFE_ALIGNED_ALLOC(alignment, size) safe_aligned_alloc((alignment), (size), __FILE__, __LINE__)

int main(void) {
    printf("=== Secure Memory Management Demo ===\n\n");

    /* Test basic allocation */
    printf("1. Basic memory allocation:\n");
    void *ptr1 = SAFE_MALLOC(1024);
    if (ptr1) {
        printf("   Allocated 1024 bytes at %p\n", ptr1);
    } else {
        printf("   Allocation failed: %s\n", strerror(errno));
    }

    /* Test reallocation */
    printf("\n2. Memory reallocation:\n");
    void *ptr2 = SAFE_REALLOC(ptr1, 2048);
    if (ptr2) {
        printf("   Reallocated to 2048 bytes at %p\n", ptr2);
        ptr1 = ptr2;  /* Update pointer */
    } else {
        printf("   Reallocation failed: %s\n", strerror(errno));
    }

    /* Test aligned allocation */
    printf("\n3. Aligned memory allocation:\n");
    void *aligned_ptr = SAFE_ALIGNED_ALLOC(64, 512);
    if (aligned_ptr) {
        printf("   Allocated 512 bytes aligned to 64 bytes at %p\n", aligned_ptr);
        printf("   Alignment check: %s\n",
               ((uintptr_t)aligned_ptr % 64 == 0) ? "PASS" : "FAIL");
    } else {
        printf("   Aligned allocation failed: %s\n", strerror(errno));
    }

    /* Test memory operations */
    printf("\n4. Memory operations:\n");
    if (ptr1 && aligned_ptr) {
        /* Test safe copy */
        if (safe_memory_copy(ptr1, 2048, "Hello, World!", 14, 13) == 0) {
            printf("   Memory copy successful\n");
        }

        /* Test memory comparison */
        int cmp_result;
        if (safe_memory_compare(ptr1, "Hello, World!", 13, &cmp_result) == 0) {
            printf("   Memory comparison: %s\n", (cmp_result == 0) ? "EQUAL" : "DIFFERENT");
        }
    }

    /* Test parameter validation */
    printf("\n5. Parameter validation:\n");
    void *null_ptr = SAFE_MALLOC(0);  /* Should fail */
    if (!null_ptr) {
        printf("   Correctly rejected zero-size allocation: %s\n", strerror(errno));
    }

    void *huge_ptr = SAFE_MALLOC(SIZE_MAX);  /* Should fail */
    if (!huge_ptr) {
        printf("   Correctly rejected oversized allocation: %s\n", strerror(errno));
    }

    /* Show memory usage */
    printf("\n6. Memory usage report:\n");
    safe_memory_report();

    /* Test double-free protection */
    printf("\n7. Testing deallocation:\n");
    if (ptr1) {
        SAFE_FREE(ptr1);
        printf("   Freed main allocation\n");
    }

    if (aligned_ptr) {
        SAFE_FREE(aligned_ptr);
        printf("   Freed aligned allocation\n");
    }

    /* Final memory report */
    printf("\n8. Final memory status:\n");
    safe_memory_report();

    /* Cleanup any remaining allocations */
    safe_memory_cleanup();

    printf("\n=== Memory management demo completed ===\n");
    return 0;
}