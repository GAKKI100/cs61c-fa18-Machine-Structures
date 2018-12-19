/*
 * Project 2: Performance Optimization
 */

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "calc_depth_naive.h"
#include "calc_depth_optimized.h"
#include "utils.h"

void calc_depth_optimized(float *depth, float *left, float *right, int image_width, int image_height, int feature_width, int feature_height, int maximum_displacement) {
#pragma omp parallel	
	{
#pragma omp for collapse(2)
		for (int y = 0; y < image_height; y++) {
			for (int x = 0; x < image_width; x++) {
			    	if (y < feature_height || y >= image_height - feature_height || x < feature_width || x >= image_width - feature_width) {
					depth[y * image_width + x] = 0;
					continue;
				}	
				float min_diff = -1;
			    	int min_dy = 0;
			    	int min_dx = 0;
			    	for (int dy = -maximum_displacement; dy <= maximum_displacement; dy++) {
					for (int dx = -maximum_displacement; dx <= maximum_displacement; dx++) {
				    		if (y + dy - feature_height < 0 || y + dy + feature_height >= image_height || x + dx - feature_width < 0 || x + dx + feature_width >= image_width) {
				        		continue;
				    		}
				    		float squared_diff = 0;
						__m128 _result = _mm_setzero_ps();

				    		for (int box_y = -feature_height; box_y <= feature_height; box_y++) {					
							int left_y = y + box_y;
							int right_y = y + dy + box_y;
							_result = _mm_setzero_ps();				
							for (int box_x = -feature_width; box_x < 2 * feature_width / 4 * 4 - feature_width; box_x += 4){
								int left_x = x + box_x;
						                int right_x = x + dx + box_x;
								__m128 _left = _mm_loadu_ps(left + left_y * image_width + left_x);
								__m128 _right = _mm_loadu_ps(right + right_y * image_width + right_x);
								__m128 _sub = _mm_sub_ps(_left, _right);
								__m128 _SED = _mm_mul_ps(_sub, _sub);
								_result = _mm_add_ps(_SED, _result);
							}
							float result[4];
							_mm_store_ps(result, _result);
							squared_diff = squared_diff + result[0] + result[1] + result[2] + result[3];
							for (int box_x =  2 * feature_width / 4 * 4 - feature_width; box_x <= feature_width; box_x++) {
								int left_x = x + box_x;
								int left_y = y + box_y;
								int right_x = x + dx + box_x;
								int right_y = y + dy + box_y;
								squared_diff += square_euclidean_distance(left[left_y * image_width + left_x], right[right_y * image_width + right_x]);
							}


					    	}
					    	if (min_diff == -1 || min_diff > squared_diff || (min_diff == squared_diff && displacement_naive(dx, dy) < displacement_naive(min_dx, min_dy))) {
							min_diff = squared_diff;
							min_dx = dx;
							min_dy = dy;
					    	}
					}
				}
				if (min_diff != -1) {
					if (maximum_displacement == 0) {
						depth[y * image_width + x] = 0;
					} else {
					    	depth[y * image_width + x] = displacement_naive(min_dx, min_dy);
					}
				} else {
					depth[y * image_width + x] = 0;
				}
			}
		}
	}
}
