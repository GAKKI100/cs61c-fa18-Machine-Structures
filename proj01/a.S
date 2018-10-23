.data
.files:
.word 0 
matrix:
	.align 4
	.byte 	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
start_msg: .asciiz "Starting program...\n"
exit_msg:  .asciiz "Exiting program...\n"
# Header for matrix
pm_head:  .asciiz "  -----0----------1----------2----------3----------4----------5----------6----------7-----\n"
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
.text
la t0 .files
main:
	# Load in pointers into data section because Venus does not
	# support using labels as data values
	la t0 q00
	la t1 qx00
	sw t1 20(t0)
	la t1 qx40
	sw t1 24(t0)
	la t1 qx44
	sw t1 28(t0)
	la t1 qx04
	sw t1 32(t0)
	la t0 qx00
	la t1 qxx00
	sw t1 20(t0)
	la t1 qxx20
	sw t1 24(t0)
	la t1 qxx22
	sw t1 28(t0)
	la t1 qxx02
	sw t1 32(t0)
	# printf("%s", start_msg)
	la a1 start_msg
	jal print_str
	# quad2matrix(&q00, &matrix, 8)
	la a0 q00
	la a1 matrix
	li a2 8
	jal quad2matrix
	# print_matrix(&matrix, 8)
	la a0 matrix
	li a1 8
	li a2 8
	jal print_matrix
	# print a new line
	jal print_newline
	# printf("%s", exit_msg)
	la a1 exit_msg
	jal print_str
	# exit()
	li a0 10
	ecall
# Helper functions (may be useful for debugging)
# You can assume these don't touch any registers except
# ra, a0, and a1.
print_int:
	li a0 1
	ecall
	jr ra
print_intx:
	li a0 34
	ecall
	jr ra
print_str:
	li a0 4
	ecall
	jr ra
print_space:
	li a1 ' '
	li a0 11
	ecall
	jr ra
print_newline:
	li a1 '\n'
	li a0 11
	ecall
	jr ra
# Prints a $a1 x $a2 matrix
#
# Arguments:
#     matrix (unsigned char*)
#     matrix_width (int)
#     matrix_height (int)
print_matrix:
	addi sp sp -24
	sw ra 0(sp)
	sw s0 4(sp)
	sw s1 8(sp)
	sw s2 12(sp)
	sw s3 16(sp)
	sw s4 20(sp)
	# s0 = matrix
	mv s0 a0
	# s1 = matrix_width
	mv s1 a1
	# s2 = matrix_height
	mv s2 a2
	# printf("%s", pm_head)
	la a1 pm_head
	jal print_str
	# for (s3 = 0; s3 < height; s3++)
	mv s3 x0
pm_loop1:
	bge s3 s2 pm_loop1end
	#printf("%d ", s3)
	mv a1 s3
	jal print_int
	jal print_space
	# for (s4 = 0; s4 < width; s4++)
	mv s4 x0
pm_loop2:
	bge s4 s1 pm_loop2end
	# printf("%x ", matrix[s3 * width + s4])
	mul t5 s1 s3
	add t5 t5 s4
	add t5 s0 t5
	lbu a1 0(t5)
	jal print_intx
	jal print_space
	addi s4 s4 1
	j pm_loop2
pm_loop2end:
	# printf("\n")
	jal print_newline
	addi s3 s3 1
	j pm_loop1
pm_loop1end:
	lw ra 0(sp)
	lw s0 4(sp)
	lw s1 8(sp)
	lw s2 12(sp)
	lw s3 16(sp)
	lw s4 20(sp)
	addi sp sp 24
	jr ra
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
