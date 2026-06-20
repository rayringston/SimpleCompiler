.global _start
.text

_start:
adr x13, V0
adr x11, S0
str x11, [x13]
adr x9, V0
ldr x9, [x9]
mov x10, x9
mov x11, x10
mov x0, x11
bl str_len
mov x9, x0
mov x10, x9
mov x11, x10
mov x9, #1
mov x10, x9
sub x11, x11, x10
adr x13, V1
str x11, [x13]
SWHILE0:
adr x9, V1
ldr x9, [x9]
mov x10, x9
mov x11, x10
mov x12, x11
mov x9, #0
mov x10, x9
mov x11, x10
cmp x12, x11
blt XWHILE0
adr x9, V0
ldr x9, [x9]
mov x10, x9
mov x11, x10
mov x0, x11
adr x9, V1
ldr x9, [x9]
mov x10, x9
mov x11, x10
mov x1, x11
bl str_char_at
mov x9, x0
mov x10, x9
mov x11, x10
mov x0, x11
bl print_char
adr x9, V1
ldr x9, [x9]
mov x10, x9
mov x11, x10
mov x9, #1
mov x10, x9
sub x11, x11, x10
adr x13, V1
str x11, [x13]
B SWHILE0
XWHILE0:
bl exit

	.data
V0: .quad 0
V1: .quad 0
S0: .asciz "hello world.\n"
S0_len = . - S0
