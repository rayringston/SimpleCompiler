.global _start
.text

_start:
adr x0, S0
ldr x1, =S0_len
bl print_str
mov x8, #93
mov x0, #0
svc #0

	.data
S0: .asciz "abc\n"
S0_len = . - S0
