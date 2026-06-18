.global _start
.text

_start:
mov x9, #10
mov x10, x9
mov x11, x10
adr x13, V0
str x11, [x13]
mov x9, #10
mov x10, x9
mov x11, x10
adr x13, V1
str x10, [x13]
adr x9, V0
ldr x9, [x9]
mov x10, x9
mov x11, x10
adr x9, V1
ldr x9, [x9]
mov x10, x9
add x11, x11, x10
mov x0, x11
bl print_int
mov x8, #93
mov x0, #0
svc #0

	.data
V0: .quad 0
V1: .quad 0
