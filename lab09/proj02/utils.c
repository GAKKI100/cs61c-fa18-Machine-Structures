/*
 * Project 2: Utilities
 *
 * ANY MODIFICATIONS TO THIS FILE WILL BE OVERWRITTEN DURING GRADING
 */

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

/* Reads a little-endian int from a buffer (works on both little and big endian machines). */
static unsigned int read_int(const unsigned char* buffer) {
    return (buffer[0] & 0xff)
        | ((buffer[1] & 0xff) << 8)
        | ((buffer[2] & 0xff) << 16)
        | ((buffer[3] & 0xff) << 24);
}

/* Writes a int to a buffer (works on both little and big endian machines). */
static void write_int(unsigned char* buffer, unsigned int value) {
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) & 0xff;
    buffer[2] = (value >> 16) & 0xff;
    buffer[3] = (value >> 24) & 0xff;
}

/* Loads a BMP file */
Image load_image(char *filename) {
    unsigned char info[54];
    unsigned char* data;
    unsigned char* color_table;
    int width;
    int height;
    int size;
    int color_table_size;
    int check;
    int row;
    int row_padding_per_row;

    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Error opening file: %s\r\n", filename);
        exit(-1);
    }

    // Read the 54-byte info header
    check = fread(info, sizeof(unsigned char), 54, f);
    if (check != 54) {
        printf("Bad file format\r\n");
        exit(-1);
    }

    color_table_size = read_int(info + 0x0a) - 54;

    // Expecting a 54-byte header and a bit-depth of 8 bits (grayscale)
    if (read_int(info + 0x0e) != 40 || read_int(info + 0x1c) != 8) {
        printf("Unsupported image format. Please specify an 8-bit (grayscale) BMP image.\r\n");
        exit(-1);
    }

    color_table = (unsigned char *) malloc(sizeof(unsigned char)* color_table_size);
    if (fread(color_table, sizeof(unsigned char), color_table_size, f) != color_table_size) {
        printf("Error reading image file.\r\n");
        exit(-1);
    }

    width = read_int(info + 0x12);
    height = read_int(info + 0x16);
    width = (width > 0) ? width : -1 * width;
    height = (height > 0) ? height : -1 * height;
    size = width * height;

    data = (unsigned char*) malloc(sizeof(unsigned char) * size);
    if (data == NULL) {
        allocation_failed();
    }

    // Assumes 1 byte per pixel
    row_padding_per_row = (4 - ((width * 1) % 4)) % 4;

    // Load data from bottom to top
    for (row = height - 1; row >= 0; row--) {
        if (fread(data + row * width, sizeof(unsigned char), width, f) != width) {
            printf("Error reading image file.\r\n");
            exit(-1);
        }
        if (row_padding_per_row > 0) {
            fseek(f, row_padding_per_row, SEEK_CUR);
        }
    }

    fclose(f);

    Image img = { .data = data, .width = width, .height = height };
    free(color_table);
    return img;
}

/* Prints the image. May be helpful in debugging */
void print_image(const unsigned char *data, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%02x ", data[y * width + x]);
        }
        printf("\r\n");
    }
}

/* Prints the image, represented as 2D float array. */
void print_float_image(const float *data, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%04.4f ", data[y * width + x]);
        }
        printf("\r\n");
    }
}

/* Converts a float array to a normalized char array. */
void float_to_char(unsigned char* dst, const float* const src, size_t n,
        int maximum_displacement) {
    for (size_t i = 0; i < n; i++) {
        dst[i] = (unsigned char) round(
                (float) UCHAR_MAX
                * src[i]
                / sqrt(2 * maximum_displacement * maximum_displacement));
    }
}

/* Converts a char array into a float array. */
void char_to_float(float* dst, const unsigned char* const src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dst[i] = (float)src[i];
    }
}

/* Fills dst with n random floats between 0 and 1. */
void fill_random_float(float* dst, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dst[i] = (int) (
                (float) UCHAR_MAX * ((float) rand() / (float) RAND_MAX)
                ) % UCHAR_MAX;
    }
}

/* Checks for floating point equality. */
bool floats_within_tolerance(float a, float b) {
    if (a == INFINITY || b == INFINITY || a == -INFINITY || b == -INFINITY) {
        return false;
    } else if (fabsf(a - b) <= FLOATING_POINT_TOLERANCE) {
        return true;
    }
    return false;
}

/* Common code for writing bitmap files. 24-bit images should have pixels stored {blue, green, red}. */
static void save_image_internal(char *filename, const unsigned char *data,
        int width, int height, int bit_depth) {
    if (bit_depth != 8 && bit_depth != 24) {
        printf("Unsupported bit depth: %i\r\n", bit_depth);
        exit(-1);
    }

    FILE *f = fopen(filename, "wb");
    int row;
    int color;
    int bytes_per_pixel = bit_depth / 8;

    if (f == NULL) {
        printf("Error opening file for writing: %s\r\n", filename);
        exit(-1);
    }

    int info_size = 54;
    int color_table_size = (bit_depth == 24) ? 0 : 256 * 4;
    int row_padding_per_row = (4 - ((width * bytes_per_pixel) % 4)) % 4;
    int data_size = (width * bytes_per_pixel + row_padding_per_row) * height;
    int footer_size = (4 - ((info_size + color_table_size + data_size) % 4)) % 4;
    int total_size = info_size + color_table_size + data_size + footer_size;

    unsigned char info[] = {
        // 0x00 Magic byte
        0x42, 0x4d,
        // 0x02 File size
        0x00, 0x00, 0x00, 0x00,
        // 0x06 Unused
        0x00, 0x00, 0x00, 0x00,
        // 0x0A Location of data (from start of file)
        0x00, 0x00, 0x00, 0x00,
        // 0x0E Size of the remainder of the header (including this field)
        0x00, 0x00, 0x00, 0x00,
        // 0x12 Width
        0x00, 0x00, 0x00, 0x00,
        // 0x16 Height
        0x00, 0x00, 0x00, 0x00,
        // 0x1A Number of planes
        0x01, 0x00,
        // 0x1C Bit depth (8 for grayscale, 24 for rgb color)
        0x00, 0x00,
        // 0x1E BI_RBG
        0x00, 0x00, 0x00, 0x00,
        // 0x22 Size of the data (including padding)
        0x00, 0x00, 0x00, 0x00,
        // 0x26 Horizontal PPI
        0x12, 0x0b, 0x00, 0x00,
        // 0x2A Vertical PPI
        0x12, 0x0b, 0x00, 0x00,
        // Number of palette colors (defaults to 2^{bit depth})
        0x00, 0x00, 0x00, 0x00,
        // Number of "important colors"
        0x00, 0x00, 0x00, 0x00
    };

    // A positive height means bottom-to-top data order
    width = (width > 0) ? width : -1 * width;
    height = (height > 0) ? height : -1 * height;

    write_int(info + 0x2, total_size);
    write_int(info + 0xa, info_size + color_table_size);
    write_int(info + 0xe, info_size - 0xe);
    write_int(info + 0x12, width);
    write_int(info + 0x16, height);
    write_int(info + 0x22, data_size + footer_size);

    info[0x1c] = (unsigned char)bit_depth;

    fwrite(info, 1, 54, f);

    unsigned char color_table[4];
    if (bit_depth == 8) {
        for (color = 0x00; color <= 0xff; color++) {
            color_table[0] = (unsigned char)color;
            color_table[1] = (unsigned char)color;
            color_table[2] = (unsigned char)color;
            color_table[3] = 0x00;
            fwrite(color_table, 1, 4, f);
        }
    }

    unsigned char row_padding[4] = { 0x00, 0x00, 0x00, 0x00 };

    // Save data from bottom to top
    for (row = height - 1; row >= 0; row--) {
        fwrite(data + row * width * bytes_per_pixel, bytes_per_pixel, width, f);
        if (row_padding_per_row > 0) {
            fwrite(row_padding, 1, row_padding_per_row, f);
        }
    }

    // Pad the bmp to a round number (multiple of 4)
    unsigned char footer[4] = { 0x00, 0x00, 0x00, 0x00 };
    fwrite(footer, 1, footer_size, f);

    fclose(f);
}

/* Writes a image into a grayscale BMP file */
void save_image(char *filename, const unsigned char *data, int width,
        int height) {
    save_image_internal(filename, data, width, height, 8);
}

/* Writes a image into a color BMP file, highlighting boxes */
void save_image_with_depth(char *filename, const unsigned char *data,
        const unsigned char *depth, int width, int height, int feature_width,
        int feature_height) {
    // Blue (far away)
    unsigned int far_color[] = { 0xff, 0x060, 0x60 };
    // Red (close up)
    unsigned int close_color[] = { 0x60, 0x60, 0xff };
    unsigned int depth_color[3];

    unsigned char *data24bit = (unsigned char*) malloc(
            sizeof(unsigned char) * 24 / 8 * width * height);
    if (data24bit == NULL) {
        allocation_failed();
    }

    int x;
    int y;
    int channel;
    unsigned char data_pixel;
    unsigned char depth_pixel;
    unsigned char* destination_pixel;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            data_pixel = data[y * width + x];
            depth_pixel = depth[y * width + x];
            destination_pixel = &data24bit[3 * (y * width + x)];
            for (channel = 0; channel < 3; channel++) {
                if (x < feature_width
                        || x >= width - feature_width
                        || y < feature_height
                        || y >= height - feature_height) {
                    // If the target is at the edge, just use grayscale
                    destination_pixel[channel] = data_pixel;
                } else {
                    depth_color[channel] = (
                            far_color[channel]
                            * (256 - depth_pixel)
                            + close_color[channel]
                            * depth_pixel
                            ) / 256;
                    destination_pixel[channel] = (unsigned char)
                        ((depth_color[channel] + data_pixel) / 2);
                }
            }
        }
    }

    save_image_internal(filename, data24bit, width, height, 24);
}

/* Should be called if memory allocation fails. */
void allocation_failed() {
    printf("Memory allocation failed.\r\n");
    exit(-1);
}
