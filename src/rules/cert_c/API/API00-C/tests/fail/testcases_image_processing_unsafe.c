/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: image_processing_unsafe.c
 *
 * This case demonstrates violations where image processing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Image format enumeration */
typedef enum {
    FORMAT_RGB,
    FORMAT_RGBA,
    FORMAT_GRAYSCALE,
    FORMAT_YUV
} ImageFormat;

/* Image structure */
typedef struct {
    int width;
    int height;
    ImageFormat format;
    unsigned char *data;
    size_t data_size;
} Image;

/* Color structure */
typedef struct {
    unsigned char r, g, b, a;
} Color;

/* NON-COMPLIANT: No validation of image loading parameters */
Image *load_image(const char *filename, ImageFormat desired_format) {
    /* No validation of filename */
    FILE *file = fopen(filename, "rb");  /* filename could be NULL */

    if (!file) {
        return NULL;  /* But we already tried to open NULL filename */
    }

    Image *image = malloc(sizeof(Image));
    /* Mock image loading without validation */
    image->width = 640;
    image->height = 480;
    image->format = desired_format;  /* No validation of format */

    size_t bytes_per_pixel = (desired_format == FORMAT_RGBA) ? 4 : 3;
    image->data_size = image->width * image->height * bytes_per_pixel;
    image->data = malloc(image->data_size);

    fread(image->data, 1, image->data_size, file);
    fclose(file);

    return image;
}

/* NON-COMPLIANT: No validation of image saving parameters */
int save_image(const Image *image, const char *filename, ImageFormat output_format) {
    /* No validation of image or filename */
    FILE *file = fopen(filename, "wb");  /* filename could be NULL */

    if (!file) {
        return -1;
    }

    /* No validation of image structure */
    fwrite(&image->width, sizeof(int), 1, file);  /* image could be NULL */
    fwrite(&image->height, sizeof(int), 1, file);
    fwrite(&output_format, sizeof(ImageFormat), 1, file);
    fwrite(image->data, 1, image->data_size, file);

    fclose(file);
    return 0;
}

/* NON-COMPLIANT: No validation of pixel access parameters */
Color get_pixel(const Image *image, int x, int y) {
    /* No validation of image or coordinates */
    int index = (y * image->width + x) * 3;  /* image could be NULL, x/y unchecked */
    Color color = {
        image->data[index],      /* Could access out of bounds */
        image->data[index + 1],
        image->data[index + 2],
        255
    };
    return color;
}

/* NON-COMPLIANT: No validation of pixel setting parameters */
void set_pixel(Image *image, int x, int y, Color color) {
    /* No validation of image or coordinates */
    int index = (y * image->width + x) * 3;  /* image could be NULL, x/y unchecked */
    image->data[index] = color.r;      /* Could write out of bounds */
    image->data[index + 1] = color.g;
    image->data[index + 2] = color.b;
}

/* NON-COMPLIANT: No validation of resize parameters */
Image *resize_image(const Image *source, int new_width, int new_height) {
    /* No validation of source or dimensions */
    Image *resized = malloc(sizeof(Image));
    resized->width = new_width;   /* Could be negative or zero */
    resized->height = new_height; /* Could be negative or zero */
    resized->format = source->format;  /* source could be NULL */

    size_t bytes_per_pixel = 3;  /* Assuming RGB */
    resized->data_size = new_width * new_height * bytes_per_pixel;  /* Could overflow */
    resized->data = malloc(resized->data_size);

    /* Simple nearest-neighbor scaling without bounds checking */
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (x * source->width) / new_width;    /* Division by zero possible */
            int src_y = (y * source->height) / new_height;
            Color pixel = get_pixel(source, src_x, src_y);  /* No bounds checking */
            set_pixel(resized, x, y, pixel);
        }
    }

    return resized;
}

/* NON-COMPLIANT: No validation of rotation parameters */
Image *rotate_image(const Image *source, double angle_degrees) {
    /* No validation of source */
    Image *rotated = malloc(sizeof(Image));
    rotated->width = source->width;   /* source could be NULL */
    rotated->height = source->height;
    rotated->format = source->format;
    rotated->data_size = source->data_size;
    rotated->data = malloc(rotated->data_size);

    double angle_rad = angle_degrees * M_PI / 180.0;  /* No validation of angle */
    double cos_angle = cos(angle_rad);
    double sin_angle = sin(angle_rad);

    int center_x = source->width / 2;
    int center_y = source->height / 2;

    for (int y = 0; y < rotated->height; y++) {
        for (int x = 0; x < rotated->width; x++) {
            int tx = x - center_x;
            int ty = y - center_y;
            int src_x = (int)(tx * cos_angle - ty * sin_angle) + center_x;
            int src_y = (int)(tx * sin_angle + ty * cos_angle) + center_y;

            /* No bounds checking for source coordinates */
            Color pixel = get_pixel(source, src_x, src_y);
            set_pixel(rotated, x, y, pixel);
        }
    }

    return rotated;
}

/* NON-COMPLIANT: No validation of filter application */
void apply_blur_filter(Image *image, int radius) {
    /* No validation of image or radius */
    unsigned char *temp_data = malloc(image->data_size);  /* image could be NULL */

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int r_sum = 0, g_sum = 0, b_sum = 0, count = 0;

            /* No validation of filter bounds */
            for (int fy = -radius; fy <= radius; fy++) {  /* radius could be negative */
                for (int fx = -radius; fx <= radius; fx++) {
                    int sample_x = x + fx;
                    int sample_y = y + fy;

                    /* No bounds checking */
                    Color pixel = get_pixel(image, sample_x, sample_y);
                    r_sum += pixel.r;
                    g_sum += pixel.g;
                    b_sum += pixel.b;
                    count++;
                }
            }

            int index = (y * image->width + x) * 3;
            temp_data[index] = r_sum / count;      /* Division by zero possible if radius is 0 */
            temp_data[index + 1] = g_sum / count;
            temp_data[index + 2] = b_sum / count;
        }
    }

    memcpy(image->data, temp_data, image->data_size);
    free(temp_data);
}

/* NON-COMPLIANT: No validation of format conversion */
Image *convert_format(const Image *source, ImageFormat target_format) {
    /* No validation of source */
    Image *converted = malloc(sizeof(Image));
    converted->width = source->width;   /* source could be NULL */
    converted->height = source->height;
    converted->format = target_format;  /* No validation of target_format */

    size_t target_bytes_per_pixel = (target_format == FORMAT_RGBA) ? 4 : 3;
    converted->data_size = source->width * source->height * target_bytes_per_pixel;
    converted->data = malloc(converted->data_size);

    /* Mock format conversion without validation */
    for (int i = 0; i < source->width * source->height; i++) {
        /* Assuming source is RGB */
        int src_index = i * 3;
        int dst_index = i * target_bytes_per_pixel;

        converted->data[dst_index] = source->data[src_index];       /* No bounds checking */
        converted->data[dst_index + 1] = source->data[src_index + 1];
        converted->data[dst_index + 2] = source->data[src_index + 2];

        if (target_format == FORMAT_RGBA) {
            converted->data[dst_index + 3] = 255;  /* Alpha channel */
        }
    }

    return converted;
}

/* NON-COMPLIANT: No validation of histogram calculation */
void calculate_histogram(const Image *image, int histogram[256]) {
    /* No validation of image or histogram array */
    memset(histogram, 0, 256 * sizeof(int));  /* histogram could be NULL */

    for (size_t i = 0; i < image->data_size; i++) {  /* image could be NULL */
        unsigned char pixel_value = image->data[i];
        histogram[pixel_value]++;  /* No bounds checking for pixel_value */
    }
}

int main(void) {
    Image *null_image = NULL;
    char *null_filename = NULL;
    int *null_histogram = NULL;

    /* Examples of dangerous image processing operations */
    // load_image(null_filename, -1);  /* NULL filename and invalid format */
    // save_image(null_image, null_filename, FORMAT_RGB);  /* NULL parameters */
    // get_pixel(null_image, -10, 1000);  /* NULL image and out of bounds */
    // set_pixel(null_image, -5, -5, (Color){255, 0, 0, 255});  /* NULL image and negative coords */
    // resize_image(null_image, -100, 0);  /* NULL image and invalid dimensions */
    // rotate_image(null_image, 720.0);  /* NULL image */
    // apply_blur_filter(null_image, -5);  /* NULL image and negative radius */
    // convert_format(null_image, 999);  /* NULL image and invalid format */
    // calculate_histogram(null_image, null_histogram);  /* NULL parameters */

    printf("Image processing functions compiled but lack parameter validation\n");
    return 0;
}