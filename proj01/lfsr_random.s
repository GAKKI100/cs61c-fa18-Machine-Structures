.data

lfsr:
	.align 4
	.word 0x1

.text

# Implements a 16-bit lfsr
#
# Arguments: None
lfsr_random:
	la t0 lfsr
	lhu a0 0(t0)

	# Your Code Here
	addi t1, x0, 0
        addi t2, x0, 16
        add t3, x0, a0
       	li t6, 0xffff
loop:
	beq t1, t2, end
        addi t4, x0, 0
        addi t5, x0, 0
        srli t4, t3, 2
        xor t5, t3, t4
        srli t4, t3, 3
        xor t5, t5, t4
        srli t4, t3, 5
        xor t5, t5, t4
        srli t4, t3, 1
        slli t5,t5, 15
        and t5,t5, t6
        or t3, t5, t4
        and t3, t3, t6
        addi t1, t1, 1
        jal x0, loop
end:
	addi a0, t3, 0
	la t0 lfsr
	sh a0 0(t0)
	jr ra
