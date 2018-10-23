.text

# Decodes a quadtree to the original matrix
#
# Arguments:
#     quadtree (qNode*)
#     matrix (void*)
#     matrix_width (int)
#
# Recall that quadtree representation uses the following format:
#     struct qNode {
#         int leaf;
#         int size;
#         int x;
#         int y;
#         int gray_value;
#         qNode *child_NW, *child_NE, *child_SE, *child_SW;
#     }

quad2matrix:
	# Your code here
depth:
	addi t0, x0, 1
	lw	t1, 0(a0)
	lw	t2, 8(a0)
	lw	t3, 12(a0)
	bne 	t1, t0, nw
	lw	t1, 4(a0)
	add 	t4, t1, t2
	add	t5, t1, t3
	lw	t1, 16(a0)
loop1:
	beq	t2, t4, end1
loop2:
	beq	t3, t5, end2
	mul	t6, t3, a2
	add 	t6, t6, t2
	add	t6, t6, a1
	sb	t1,0(t6)
	addi 	t3, t3, 1
	jal	x0, loop2
end2:
	lw	t3, 12(a0)	
	addi 	t2, t2, 1
	jal	x0, loop1
end1:
	jr 	ra
nw:
	addi 	sp, sp, -8
	sw	a0, 0(sp)
	sw	ra, 4(sp)
	beq	a0, x0, ne
	lw	t1, 20(a0)
	add	a0, x0, t1
	jal	ra, depth
	lw	a0, 0(sp)
	lw	ra, 4(sp)
	addi 	sp, sp, 8			
ne:
	addi 	sp, sp, -8
	sw	a0, 0(sp)
	sw	ra, 4(sp)
	beq	a0, x0, se
	lw	t1, 24(a0)
	add	a0, x0, t1
	jal	ra, depth
	lw	a0, 0(sp)
	lw	ra, 4(sp)
	addi 	sp, sp, 8	
se:
	addi 	sp, sp, -8
	sw	a0, 0(sp)
	sw	ra, 4(sp)
	beq	a0, x0, sw
	lw	t1, 28(a0)
	add	a0, x0, t1
	jal	ra, depth
	lw	a0, 0(sp)
	lw	ra, 4(sp)
	addi 	sp, sp, 8	
sw:
	addi 	sp, sp, -8
	sw	a0, 0(sp)
	sw	ra, 4(sp)
	beq	a0, x0, end
	lw	t1, 32(a0)
	add	a0, x0, t1
	jal	ra, depth
	lw	a0, 0(sp)
	lw	ra, 4(sp)
	addi 	sp, sp, 8	
end:
	jr 	ra
