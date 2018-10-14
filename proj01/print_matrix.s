.data

# Header for matrix
pm_head:  .asciiz "  -----0----------1----------2----------3----------4----------5----------6----------7-----\n"

.text

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
