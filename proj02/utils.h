/*
 * Project 2: Utilities
 *
 * ANY MODIFICATIONS TO THIS FILE WILL BE OVERWRITTEN DURING GRADING
 */

#include <stdbool.h>
#include <stdlib.h>

#ifndef UTILS_H
#define UTILS_H

typedef struct {
    unsigned char *data;
    int width;
    int height;
} Image;

typedef unsigned char byte;

#define FLOATING_POINT_TOLERANCE 0.0001

/* Loads a BMP file */
Image load_image(char *filename);

/* Prints the image. May be helpful in debugging */
void print_image(const unsigned char *data, int width, int height);

/* Prints the image, represented as 2D float array. */
void print_float_image(const float *data, int width, int height);

/* Converts a float array to a normalized char array. */
void float_to_char(unsigned char* dst, const float* const src, size_t n,
        int maximum_displacement);

/* Converts a char array into a float array. */
void char_to_float(float* dst, const unsigned char* const src, size_t n);

/* Fills dst with n random floats between 0 and 1. */
void fill_random_float(float* dst, size_t n);

/* Checks for floating point equality. */
bool floats_within_tolerance(float a, float b);

/* Writes a image into a grayscale BMP file */
void save_image(char *filename, const unsigned char *data, int width,
        int height);

/* Writes a image into a color BMP file, highlighting boxes */
void save_image_with_depth(char *filename, const unsigned char *data,
        const unsigned char *depth, int width, int height, int feature_width,
        int feature_height);

/* Should be called if memory allocation fails. */
void allocation_failed();

#endif
