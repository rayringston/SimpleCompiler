.global print_int

print_int:
mov x2, x0		// x2-3 will hold intermediate valuesi
mov x4, #10		// x4 holds divisor, 10

sub sp, sp, #16
add x1, sp, #15

mov w7, #'\n'		// add the newline first since it prints last 
strb w7, [x1]
mov x9, #1		// x9 will count digits

print_int_loop:		// first loop to push the digits of the number into the stack
udiv x3, x2, x4
mul x6, x3, x4
sub  x7, x2, x6

add w7, w7, #'0'	// converts the digit to ASCII

sub x1, x1, #1		// push to stack
strb w7, [x1]
add x9, x9, #1

mov x2, x3		// repeat with the number shifted down
cbnz x2, print_int_loop

mov x0, #1
mov x2, x9
mov x8, #64
svc #0

add sp, sp, #16

ret



