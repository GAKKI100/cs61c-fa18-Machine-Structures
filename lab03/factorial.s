.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial
    
    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result
    
    addi a0, x0, 10
    ecall # Exit

factorial:
    # YOUR CODE HERE
    add t1, a0, zero
    add t2, t1, zero
    addi t3, zero, 1
loop:
    beq t1, t3, exit
    addi t1, t1, -1
    mul t2, t2, t1
    jal x0, loop
exit:
	add a0, t2, zero
    jr ra
