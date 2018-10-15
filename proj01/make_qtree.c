/*
 * Project 1-1: Quadtree Compression
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {
	int gray_value = depth_map[y * map_width + x];
	for(int i = 0; i < section_width; i++){
		for(int j = 0; j < section_width; j++){
			if(depth_map[(j + y) * map_width + i + x] != gray_value){
				return -1;
			}	
		}
	}
    	return gray_value;
}

void insert_node(qNode* q, unsigned char *depth_map, int map_width);

void free_node(qNode *q);

qNode *depth_to_quad(unsigned char *depth_map, int map_width) {
    /* YOUR CODE HERE */
	qNode* root = calloc(1, sizeof(qNode));
	if(root == NULL){
		allocation_failed();	
	}
	root->x = 0;
	root->y = 0;
	root->size = map_width;
	root->gray_value = homogenous(depth_map, map_width, root->x, root->y, root->size);
	root->leaf = (root->gray_value != -1)? true : false;
	insert_node(root, depth_map, map_width);
	return root;
}

void insert_node(qNode* q, unsigned char *depth_map, int map_width){
	if(q->leaf == true){
		q->child_NW = NULL;
                q->child_NE = NULL;
                q->child_SE = NULL;
                q->child_SW = NULL;
		return;		
	}
	q->child_NW = calloc(1, sizeof(qNode));
	if(q->child_NW == NULL){
                allocation_failed();
        }
        q->child_NE = calloc(1, sizeof(qNode));
        if(q->child_NE == NULL){
                allocation_failed();
        }
        q->child_SE = calloc(1, sizeof(qNode));
        if(q->child_SE == NULL){
                allocation_failed();
        }
	q->child_SW = calloc(1, sizeof(qNode));
	if(q->child_SW == NULL){
                allocation_failed();
        }
	q->child_NW->x = q->x;
	q->child_NW->y = q->y;
	q->child_NW->size = q->size / 2;
	q->child_NW->gray_value = homogenous(depth_map, map_width, q->child_NW->x, q->child_NW->y, q->child_NW->size);
	q->child_NW->leaf = (q->child_NW->gray_value != -1)? true : false;
	insert_node(q->child_NW, depth_map, map_width);
	
	q->child_NE->x = q->x + q->size / 2;
        q->child_NE->y = q->y;
        q->child_NE->size = q->size / 2;
        q->child_NE->gray_value = homogenous(depth_map, map_width, q->child_NE->x, q->child_NE->y, q->child_NE->size);
        q->child_NE->leaf = (q->child_NE->gray_value != -1)? true : false;
        insert_node(q->child_NE, depth_map, map_width);

	q->child_SE->x = q->x + q->size / 2;
        q->child_SE->y = q->y + q->size / 2;
        q->child_SE->size = q->size / 2;
        q->child_SE->gray_value = homogenous(depth_map, map_width, q->child_SE->x, q->child_SE->y, q->child_SE->size);
        q->child_SE->leaf = (q->child_SE->gray_value != -1)? true : false;
        insert_node(q->child_SE, depth_map, map_width);
	
	q->child_SW->x = q->x;
        q->child_SW->y = q->y + q->size / 2;
        q->child_SW->size = q->size / 2;
        q->child_SW->gray_value = homogenous(depth_map, map_width, q->child_SW->x, q->child_SW->y, q->child_SW->size);
        q->child_SW->leaf = (q->child_SW->gray_value != -1)? true : false;
        insert_node(q->child_SW, depth_map, map_width);
}

void free_qtree(qNode *qtree_node) {
    /* YOUR CODE HERE */
	free_node(qtree_node);
}

void free_node(qNode *q){
	if(q->child_NW != NULL){
		free_node(q->child_NW);
	}
	if(q->child_NE != NULL){
                free_node(q->child_NE);
        }
	if(q->child_SE != NULL){
                free_node(q->child_SE);
        }
	if(q->child_SW != NULL){
                free_node(q->child_SW);
        }
        free(q);
}

