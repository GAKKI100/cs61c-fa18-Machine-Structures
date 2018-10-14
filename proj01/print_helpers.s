.text

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
