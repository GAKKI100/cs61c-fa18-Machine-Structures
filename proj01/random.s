.text

main:
	li s0 20

main_random_loop:
	beq s0 x0 main_end

	# printf("%d", lfsr_random());
	jal lfsr_random
	mv a1 a0
	jal print_int

	# printf("\n");
	jal print_newline

	addi s0 s0 -1
	jal x0 main_random_loop

main_end:
	# exit()
	li a0 10
	ecall

	.include "lfsr_random.s"
	.include "print_helpers.s"
