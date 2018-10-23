.data
# Quadtree Node representation
# Recall that quadtree representation uses the following format:
#       struct qNode {
#               int leaf;
#               int size;
#               int x;
#               int y;
#               int gray_value;
#               qNode *child_NW, *child_NE, *child_SE, *child_SW;
#       }

q00:	.word	0, 8, 0, 0, 	-1, 0, 0, 0, 0

qx00:	.word	0, 4, 0, 0,	-1, 0, 0, 0, 0 		
qx40:	.word	1, 4, 4, 0, 	13, 0, 0, 0, 0
qx44:	.word	1, 4, 4, 4,	20, 0, 0, 0, 0
qx04:	.word	1, 4, 0, 4, 	15, 0, 0, 0, 0 

qxx00:	.word	1, 2, 0, 0,	3, 0, 0, 0, 0
qxx20:	.word	1, 2, 2, 0,	5, 0, 0, 0, 0
qxx22:	.word	1, 2, 2, 2,	7, 0, 0, 0, 0
qxx02:	.word	1, 2, 0, 2, 	6, 0, 0, 0, 0
