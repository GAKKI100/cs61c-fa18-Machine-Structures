.data

.include "quad2matrix_data_2.s"

matrix:
	.align 4
	.byte 	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0



start_msg: .asciiz "Starting program...\n"
exit_msg:  .asciiz "Exiting program...\n"

.text

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

.include "quad2matrix.s"
.include "print_matrix.s"
.include "print_helpers.s"
