/*
 * Project 1-1: Computing a Displacement Map
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {
    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement)
            / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;
}

/* Helper function to return the square euclidean distance between two values. */
unsigned int square_euclidean_distance(unsigned char a, unsigned char b) {
   /*
    * This is an optional helper function which you may find useful. It
    * currently has an incomplete example CUnit test to help with debugging.
    * This test is not graded and is merely for your convenience. See the spec
    * for details on how to use CUnit.
    */
    return (a - b) * (a - b);
}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {
    /* YOUR CODE HERE */ 
	for(int i = 0; i < image_width; i++){
		for(int j = 0; j < image_height; j++){
			if(j - feature_height < 0 || j + feature_height >= image_height 
				|| i - feature_width < 0 || i + feature_width >= image_width){
				depth_map[j * image_width + i] = 0;
				continue;
			}
			if(maximum_displacement == 0){
				depth_map[j * image_width + i] = 0;
                                continue;
			}
			int right_ul_x, right_ul_y, right_lr_x, right_lr_y;
			right_ul_x = i - maximum_displacement;
			right_ul_y = j - maximum_displacement;
			right_lr_x = i + maximum_displacement;
			right_lr_y = j + maximum_displacement;
			if(right_ul_x - feature_width < 0){
				right_ul_x = feature_width;
			}
			if(right_ul_y - feature_height < 0){
                                right_ul_y = feature_height;
                        }
			if(right_lr_x + feature_width >= image_width){
                                right_lr_x = image_width - 1  - feature_width;
                        }
			if(right_lr_y + feature_height >= image_height){
                                right_lr_y = image_height - 1 - feature_height;
                        }				
			//the UpperLeft and LowerRight coordiates of the scope for all possible features' center

			int sed_len = (right_lr_x - right_ul_x + 1) * (right_lr_y - right_ul_y + 1);
			//short for Squared Euclidean Distance 	
			unsigned sed[sed_len];
			for(int t = 0; t < sed_len; t++){
				sed[t] = ~0;
			}
			unsigned int temp;
			int count = 0;

/*calculate SEDs of all matching features in right image*/
			for(int x = right_ul_x; x <= right_lr_x; x++){
				for(int y = right_ul_y; y <= right_lr_y; y++){
					temp = 0;
					for(int x1 = -feature_width; x1 <= feature_width; x1++){
						for(int y1 = -feature_height; y1 <= feature_height; y1++){
							temp += square_euclidean_distance(left[(j + y1) * image_width + i + x1], right[(y + y1) * image_width + x + x1]);
						}
					}
					sed[count] = temp;
					count++;
				}
			}

/*find the minimum sed*/
                        unsigned int min = sed[0];
			int dx = abs(right_ul_x - i);
			int dy = abs(right_ul_y - j);
			unsigned  depth = normalized_displacement(dx, dy, maximum_displacement);
			for (int z = 0; z < sed_len; z++){
    				if (sed[z] < min){
        				min = sed[z];
					dx = abs(right_ul_x +  z / (right_lr_y - right_ul_y + 1) - i);
					dy = abs(right_ul_y +  z % (right_lr_y - right_ul_y + 1) - j);
					depth = normalized_displacement(dx, dy, maximum_displacement);
    				}
			}
/*find the feature with minimum SED; 
and if two feature with the same SED,choose the nearest one */
			for(int z = 0; z < sed_len; z++){
				if(sed[z] == min){
					int temp_right_x = right_ul_x +  z / (right_lr_y - right_ul_y + 1);
					int temp_right_y = right_ul_y +  z % (right_lr_y - right_ul_y + 1);
					int temp_dx = abs(temp_right_x - i);
					int temp_dy = abs(temp_right_y - j);
					if(temp_dx == 0 && temp_dy == 0){
						depth = 0;
						break;
					}
					int temp_depth = normalized_displacement(temp_dx, temp_dy, maximum_displacement);
					if(temp_depth < depth){
						depth = temp_depth;
					}
				}
			}
			depth_map[j * image_width + i] = depth;
		}
	}
}
