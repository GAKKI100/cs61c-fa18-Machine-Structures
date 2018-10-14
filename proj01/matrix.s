.data

.include "quad2matrix_data.s"

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


	la t0 qx40
	la t1 qxx40
	sw t1 20(t0)
	la t1 qxx60
	sw t1 24(t0)
	la t1 qxx62
	sw t1 28(t0)
	la t1 qxx42
	sw t1 32(t0)

	la t0 qx44
	la t1 qxx44
	sw t1 20(t0)
	la t1 qxx64
	sw t1 24(t0)
	la t1 qxx66
	sw t1 28(t0)
	la t1 qxx46
	sw t1 32(t0)

	la t0 qxx62
	la t1 qxxx62
	sw t1 20(t0)
	la t1 qxxx72
	sw t1 24(t0)
	la t1 qxxx73
	sw t1 28(t0)
	la t1 qxxx63
	sw t1 32(t0)

	la t0 qxx66
	la t1 qxxx66
	sw t1 20(t0)
	la t1 qxxx76
	sw t1 24(t0)
	la t1 qxxx77
	sw t1 28(t0)
	la t1 qxxx67
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
