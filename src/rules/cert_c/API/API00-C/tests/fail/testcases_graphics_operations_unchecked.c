/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: graphics_operations_unchecked.c
 *
 * This case demonstrates violations where graphics/image processing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple RGB pixel structure */
typedef struct {
    unsigned char r, g, b;
} RGB_Pixel;

/* Simple image structure */
typedef struct {
    int width;
    int height;
    RGB_Pixel *pixels;
} Image;

/* NON-COMPLIANT: No validation of image structure or coordinates */
void set_pixel(Image *image, int x, int y, RGB_Pixel color) {
    /* No validation of image or coordinates */
    int index = y * image->width + x;  /* image could be NULL, coordinates unchecked */
    image->pixels[index] = color;  /* Could access out of bounds */
}

/* NON-COMPLIANT: No validation of image parameters */
RGB_Pixel get_pixel(Image *image, int x, int y) {
    /* No validation of image or coordinates */
    int index = y * image->width + x;  /* Could be out of bounds */
    return image->pixels[index];  /* image could be NULL */
}

/* NON-COMPLIANT: No validation of image dimensions */
Image *create_image(int width, int height) {
    /* No validation of dimensions */
    Image *image = malloc(sizeof(Image));
    image->width = width;  /* width could be negative or zero */
    image->height = height;  /* height could be negative or zero */
    image->pixels = malloc(width * height * sizeof(RGB_Pixel));  /* Could overflow */
    return image;
}

/* NON-COMPLIANT: No validation of source and destination images */
void copy_image_region(Image *src, Image *dest, int src_x, int src_y,
                      int dest_x, int dest_y, int width, int height) {
    /* No validation of any parameters */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB_Pixel pixel = get_pixel(src, src_x + x, src_y + y);  /* Could be out of bounds */
            set_pixel(dest, dest_x + x, dest_y + y, pixel);  /* Could be out of bounds */
        }
    }
}

/* NON-COMPLIANT: No validation of scaling parameters */
Image *scale_image(Image *source, float scale_x, float scale_y) {
    /* No validation of source or scale factors */
    int new_width = (int)(source->width * scale_x);  /* source could be NULL */
    int new_height = (int)(source->height * scale_y);  /* scale could be negative */

    Image *scaled = create_image(new_width, new_height);

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (int)(x / scale_x);
            int src_y = (int)(y / scale_y);
            RGB_Pixel pixel = get_pixel(source, src_x, src_y);
            set_pixel(scaled, x, y, pixel);
        }
    }

    return scaled;
}

/* NON-COMPLIANT: No validation of filter matrix */
void apply_convolution_filter(Image *image, float *filter, int filter_size) {
    /* No validation of image or filter */
    int offset = filter_size / 2;

    for (int y = offset; y < image->height - offset; y++) {  /* image could be NULL */
        for (int x = offset; x < image->width - offset; x++) {
            float r = 0, g = 0, b = 0;

            for (int fy = 0; fy < filter_size; fy++) {
                for (int fx = 0; fx < filter_size; fx++) {
                    RGB_Pixel pixel = get_pixel(image, x + fx - offset, y + fy - offset);
                    float weight = filter[fy * filter_size + fx];  /* filter could be NULL */
                    r += pixel.r * weight;
                    g += pixel.g * weight;
                    b += pixel.b * weight;
                }
            }

            RGB_Pixel new_pixel = {(unsigned char)r, (unsigned char)g, (unsigned char)b};
            set_pixel(image, x, y, new_pixel);
        }
    }
}

/* NON-COMPLIANT: No validation of rotation angle or center */
Image *rotate_image(Image *source, float angle, int center_x, int center_y) {
    /* No validation of source or parameters */
    Image *rotated = create_image(source->width, source->height);  /* source could be NULL */

    float cos_angle = cos(angle);
    float sin_angle = sin(angle);

    for (int y = 0; y < rotated->height; y++) {
        for (int x = 0; x < rotated->width; x++) {
            int translated_x = x - center_x;
            int translated_y = y - center_y;

            int original_x = (int)(translated_x * cos_angle - translated_y * sin_angle) + center_x;
            int original_y = (int)(translated_x * sin_angle + translated_y * cos_angle) + center_y;

            /* No bounds checking for original coordinates */
            RGB_Pixel pixel = get_pixel(source, original_x, original_y);
            set_pixel(rotated, x, y, pixel);
        }
    }

    return rotated;
}

int main(void) {
    Image *null_image = NULL;
    float *null_filter = NULL;

    /* Examples of dangerous graphics operations */
    // set_pixel(null_image, 10, 10, (RGB_Pixel){255, 0, 0});  /* NULL image */
    // get_pixel(null_image, 5, 5);  /* NULL image */
    // create_image(-100, -50);  /* Negative dimensions */
    // copy_image_region(null_image, null_image, 0, 0, 0, 0, 10, 10);  /* NULL images */
    // scale_image(null_image, -1.0f, 2.0f);  /* NULL image and negative scale */
    // apply_convolution_filter(null_image, null_filter, 3);  /* NULL parameters */
    // rotate_image(null_image, 45.0f, 50, 50);  /* NULL image */

    printf("Graphics functions compiled but lack parameter validation\n");
    return 0;
}