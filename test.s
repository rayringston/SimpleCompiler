.global _start
.text

_start:
mov x9, #10
mov x10, x9
mov x11, x10
mov x0, x11
bl printf
mov x8, #93
mov x0, #0
svc #0

	.data
