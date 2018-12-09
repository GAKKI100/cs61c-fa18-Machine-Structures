/*
 * Project 2: Correctness Tester
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "calc_depth_naive.h"
#include "calc_depth_optimized.h"
#include "utils.h"

static bool test_calc_depth(int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {
    float* left = (float*) malloc(sizeof(float) * image_width * image_height);
    if (left == NULL) {
        allocation_failed();
    }

    float* right = (float*) malloc(sizeof(float) * image_width * image_height);
    if (right == NULL) {
        free(left);
        allocation_failed();
    }

    fill_random_float(left, image_width * image_height);
    fill_random_float(right, image_width * image_height);

    float* depth_naive = (float*) malloc(sizeof(float) * image_width * image_height);
    if (depth_naive == NULL) {
        free(left);
        free(right);
        allocation_failed();
    }

    float* depth_optimized = (float*) malloc(sizeof(float) * image_width * image_height);
    if (depth_optimized == NULL) {
        free(left);
        free(right);
        free(depth_naive);
        allocation_failed();
    }

    calc_depth_optimized(depth_optimized, left, right, image_width,
            image_height, feature_width, feature_height, maximum_displacement);
    calc_depth_naive(depth_naive, left, right, image_width,
            image_height, feature_width, feature_height, maximum_displacement);

    for (size_t i = 0; i < image_width * image_height; i++) {
        if (!floats_within_tolerance(depth_naive[i], depth_optimized[i])) {
            free(left);
            free(right);
            free(depth_naive);
            free(depth_optimized);
            return false;
        }
    }

    free(left);
    free(right);
    free(depth_naive);
    free(depth_optimized);
    return true;
}

static bool test_calc_depth_verbose(int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {
    printf("Testing %ix%i image, feature width %i, feature height %i, maximum diplacement %i... ",
            image_width, image_height, feature_width, feature_height, maximum_displacement);
    if (test_calc_depth(image_width, image_height, feature_width,
                feature_height, maximum_displacement)) {
        printf("Test success!\r\n");
        return true;
    } else {
        printf("Error! Results of naive and optimized code do not match.\r\n");
        return false;
    }
}

int main(int argc, char **argv) {
    int return_result = 0;
    if (!test_calc_depth_verbose(3, 3, 2, 2, 1)) {
        return_result = - 1;
    }
    if (!test_calc_depth_verbose(5, 5, 2, 2, 1)) {
        return_result = -1;
    }
    if (!test_calc_depth_verbose(10, 10, 2, 2, 1)) {
        return_result = -1;
    }
    if (!test_calc_depth_verbose(10, 10, 5, 5, 2)) {
        return_result = -1;
    }
    if (!test_calc_depth_verbose(100, 100, 3, 3, 14)) {
        return_result = -1;
    }

    if (!test_calc_depth_verbose(1, 20, 3, 3, 14)) {
        return_result = -1;
    }
    if (!test_calc_depth_verbose(20, 1, 3, 3, 14)) {
        return_result = -1;
    }

    if (!test_calc_depth_verbose(10, 10, 2, 5, 1)) {
        return_result = -1;
    }
    if (!test_calc_depth_verbose(10, 10, 5, 2, 2)) {
        return_result = -1;
    }

    if (!test_calc_depth_verbose(21, 5, 9, 5, 15)) {
        return_result = -1;
    }

    return return_result;
}
