/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: compression_functions_unchecked.c
 *
 * This case demonstrates violations where compression/decompression functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Mock compression structures */
typedef struct {
    void *stream_handle;
    int compression_level;
    size_t buffer_size;
} CompressionStream;

/* NON-COMPLIANT: No validation of compression parameters */
CompressionStream *init_compression(int compression_level, size_t buffer_size) {
    CompressionStream *stream = malloc(sizeof(CompressionStream));
    /* No validation of compression_level or buffer_size */
    stream->compression_level = compression_level;  /* Could be out of valid range */
    stream->buffer_size = buffer_size;  /* Could be zero or excessively large */
    stream->stream_handle = malloc(buffer_size);  /* Could cause allocation failure */
    return stream;
}

/* NON-COMPLIANT: No validation of input data */
size_t compress_data(CompressionStream *stream, const void *input_data, size_t input_size,
                    void *output_buffer, size_t output_buffer_size) {
    /* No validation of any parameters */
    printf("Compressing %zu bytes with level %d\n",
           input_size, stream->compression_level);  /* stream could be NULL */

    /* Mock compression - copying input to output without validation */
    size_t compressed_size = input_size / 2;  /* Simulated compression ratio */

    if (compressed_size > output_buffer_size) {  /* No check if output_buffer is NULL */
        compressed_size = output_buffer_size;
    }

    memcpy(output_buffer, input_data, compressed_size);  /* Both could be NULL */
    return compressed_size;
}

/* NON-COMPLIANT: No validation of compressed data */
size_t decompress_data(const void *compressed_data, size_t compressed_size,
                      void *output_buffer, size_t output_buffer_size) {
    /* No validation of any parameters */
    printf("Decompressing %zu bytes\n", compressed_size);

    /* Mock decompression without validation */
    size_t decompressed_size = compressed_size * 2;  /* Simulated expansion */

    if (decompressed_size > output_buffer_size) {  /* No check if output_buffer is NULL */
        decompressed_size = output_buffer_size;
    }

    memcpy(output_buffer, compressed_data, decompressed_size);  /* Both could be NULL */
    return decompressed_size;
}

/* NON-COMPLIANT: No validation of file paths */
int compress_file(const char *input_filename, const char *output_filename, int compression_level) {
    /* No validation of filenames */
    FILE *input_file = fopen(input_filename, "rb");  /* input_filename could be NULL */
    FILE *output_file = fopen(output_filename, "wb");  /* output_filename could be NULL */

    if (!input_file || !output_file) {
        return -1;  /* But we already tried to open NULL filenames */
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        /* Mock compression write */
        fwrite(buffer, 1, bytes_read / 2, output_file);  /* Simulated compression */
    }

    fclose(input_file);
    fclose(output_file);
    return 0;
}

/* NON-COMPLIANT: No validation of archive parameters */
int create_archive(const char *archive_name, char **file_list, int file_count) {
    /* No validation of archive_name or file_list */
    printf("Creating archive: %s\n", archive_name);  /* archive_name could be NULL */

    for (int i = 0; i < file_count; i++) {
        printf("Adding file: %s\n", file_list[i]);  /* file_list or elements could be NULL */
    }

    return 0;
}

/* NON-COMPLIANT: No validation of extraction parameters */
int extract_archive(const char *archive_name, const char *destination_path) {
    /* No validation of parameters */
    printf("Extracting archive %s to %s\n", archive_name, destination_path);  /* Both could be NULL */
    return 0;
}

/* NON-COMPLIANT: No validation of chunk parameters */
size_t compress_chunk(const void *chunk_data, size_t chunk_size, int chunk_index,
                     void *compressed_buffer, size_t buffer_size) {
    /* No validation of any parameters */
    printf("Compressing chunk %d of size %zu\n", chunk_index, chunk_size);

    /* Mock chunk compression */
    size_t compressed_size = chunk_size / 3;  /* Simulated compression */

    memcpy(compressed_buffer, chunk_data, compressed_size);  /* Both could be NULL */
    return compressed_size;
}

/* NON-COMPLIANT: No validation of dictionary parameters */
int set_compression_dictionary(CompressionStream *stream, const void *dictionary, size_t dict_size) {
    /* No validation of stream or dictionary */
    printf("Setting compression dictionary of size %zu\n", dict_size);

    /* Mock dictionary setting */
    stream->buffer_size += dict_size;  /* stream could be NULL */
    return 0;
}

int main(void) {
    CompressionStream *null_stream = NULL;
    void *null_data = NULL;
    char *null_filename = NULL;
    char **null_file_list = NULL;

    /* Examples of dangerous compression operations */
    // init_compression(-10, 0);  /* Invalid compression level and zero buffer size */
    // compress_data(null_stream, null_data, 100, null_data, 0);  /* NULL parameters */
    // decompress_data(null_data, 100, null_data, 0);  /* NULL parameters */
    // compress_file(null_filename, null_filename, 15);  /* NULL filenames */
    // create_archive(null_filename, null_file_list, -5);  /* NULL parameters */
    // extract_archive(null_filename, null_filename);  /* NULL parameters */
    // compress_chunk(null_data, 0, -1, null_data, 0);  /* NULL and invalid parameters */
    // set_compression_dictionary(null_stream, null_data, 0);  /* NULL parameters */

    printf("Compression functions compiled but lack parameter validation\n");
    return 0;
}