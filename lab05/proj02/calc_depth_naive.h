/*
 * Project 2: Performance Optimization
 */

#ifndef CALC_DEPTH_NAIVE_H
#define CALC_DEPTH_NAIVE_H

float displacement_naive(int dx, int dy);

float square_euclidean_distance(float a, float b);

void calc_depth_naive(float *depth, float *left, float *right, int image_width,
        int image_height, int feature_width, int feature_height,
        int maximum_displacement);

#endif
