.global print_str

print_str:	// x0 is the string buffer, x1 is the length of the string
mov x2, x1
mov x1, x0
mov x0, #1
mov x8, #64
svc #0

ret
