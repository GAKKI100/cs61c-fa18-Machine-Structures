/*
 * Project 2: Benchmark Runner
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include "calc_depth_naive.h"
#include "calc_depth_optimized.h"
#include "utils.h"

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#if defined(_MSC_VER)
#pragma intrinsic(__rdtsc)
#else
static __inline__ uint64_t __rdtsc() {
    uint32_t hi, lo;
    __asm__ volatile
        (
        "rdtsc"
        : "=a"(lo), "=d"(hi)
        );
    return (((uint64_t) hi) << 32) | ((uint64_t) lo);
}
#endif

static bool benchmark_matrix(int image_width, int image_height,
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

    double speedup;
    uint64_t start_tSC, end_tSC;

    // Get total clock cycles for optimized
    start_tSC = __rdtsc();
    calc_depth_optimized(depth_optimized, left, right, image_width,
            image_height, feature_width, feature_height, maximum_displacement);
    end_tSC = __rdtsc();
    double optimized_tSC = end_tSC - start_tSC;

    // Get total clock cycles for naive
    start_tSC = __rdtsc();
    calc_depth_naive(depth_naive, left, right, image_width,
            image_height, feature_width, feature_height, maximum_displacement);
    end_tSC = __rdtsc();
    double naive_tSC = end_tSC - start_tSC;

    // Speedup is just naive / optimised
    speedup = naive_tSC / optimized_tSC;
    printf("%.4f Speedup Ratio\r\n", speedup);

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

int main(int argc, char** argv) {
    int return_result = 0;

    // Fix random seed, please don't precompute things
    srand(0xDEAD + 0x61C + 0xEEC5);

    printf("Testing non-odd cases: \r\n");
    int tests[][5] = {
        { 250, 250, 4, 4, 6 },
        { 250, 250, 6, 6, 6 },
        { 500, 500, 4, 4, 6 },
        { 500, 500, 6, 6, 6 },
        { 750, 750, 8, 8, 6 }
    };
    for (int i = 0; i < ARRAY_SIZE(tests); i++) {
        printf("Testing %ix%i image, feature width %i, feature height %i, maximum diplacement %i... ",
                tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4]);
        if (!benchmark_matrix(tests[i][0], tests[i][1], tests[i][2], tests[i][3], tests[i][4])) {
            printf("Error: optimized does not match naive.\r\n");
            return_result = -1;
        }
    }

    printf("\r\nTesting odd cases: \r\n");
    int odd_tests[][5] = {
        { 400, 600, 3, 3, 7 },
        { 600, 400, 4, 4, 7 },
        { 501, 501, 4, 4, 7 },
        { 751, 751, 4, 4, 4 }
    };
    for (int i = 0; i < ARRAY_SIZE(odd_tests); i++) {
        printf("Testing %ix%i image, feature width %i, feature height %i, maximum diplacement %i... ",
                odd_tests[i][0], odd_tests[i][1], odd_tests[i][2], odd_tests[i][3], odd_tests[i][4]);
        if (!benchmark_matrix(odd_tests[i][0], odd_tests[i][1], odd_tests[i][2], odd_tests[i][3], odd_tests[i][4])) {
            printf("Error: optimized does not match naive.\r\n");
            return_result = -1;
        }
    }

    return return_result;
}
