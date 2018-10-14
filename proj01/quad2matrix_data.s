.data
# Quadtree Node representation
# Recall that quadtree representation uses the following format:
#	struct qNode {
#		int leaf;
#		int size;
#		int x;
#		int y;
#		int gray_value;
#		qNode *child_NW, *child_NE, *child_SE, *child_SW;
#	}

q00:   .word  0, 8, 0, 0,  -1, 0, 0, 0, 0

qx00:  .word  1, 4, 0, 0,   1, 0, 0, 0, 0
qx40:  .word  0, 4, 4, 0,  -1, 0, 0, 0, 0
qx44:  .word  0, 4, 4, 4,  -1, 0, 0, 0, 0
qx04:  .word  1, 4, 0, 4,   2, 0, 0, 0, 0

qxx40: .word  1, 2, 4, 0,   3, 0, 0, 0, 0
qxx60: .word  1, 2, 6, 0,   4, 0, 0, 0, 0
qxx62: .word  0, 2, 6, 2,  -1, 0, 0, 0, 0
qxx42: .word  1, 2, 4, 2,   5, 0, 0, 0, 0
qxx44: .word  1, 2, 4, 4,   6, 0, 0, 0, 0
qxx64: .word  1, 2, 6, 4,   7, 0, 0, 0, 0
qxx66: .word  0, 2, 6, 6,  -1, 0, 0, 0, 0
qxx46: .word  1, 2, 4, 6,   8, 0, 0, 0, 0

qxxx62: .word 1, 1, 6, 2,   9, 0, 0, 0, 0
qxxx72: .word 1, 1, 7, 2,  10, 0, 0, 0, 0
qxxx73: .word 1, 1, 7, 3,  11, 0, 0, 0, 0
qxxx63: .word 1, 1, 6, 3,  12, 0, 0, 0, 0
qxxx66: .word 1, 1, 6, 6,  13, 0, 0, 0, 0
qxxx76: .word 1, 1, 7, 6,  14, 0, 0, 0, 0
qxxx77: .word 1, 1, 7, 7,  15, 0, 0, 0, 0
qxxx67: .word 1, 1, 6, 7,  16, 0, 0, 0, 0
