.global int_to_text

int_to_text:

stp fp, lr, [sp, #-16]!
mov fp, sp

stp x19, x20, [sp, #-16]!
stp x21, x22, [sp, #-16]!
stp x23, x24, [sp, #-16]!

mov x19, x0		// x2-3 will hold intermediate valuesi

			// at the very start, determine if the number is negative
cmp x19, #0
cset x20, LT
csneg x19, x19, x19, GE

mov x21, #10		// x4 holds divisor, 10

sub sp, sp, #32		// allocate 32 bytes on stack to hold the max of 20 digits (+1 for negative)
add x22, sp, #31

mov w23, #'\0'		// add the null terminator first since it prints last 
strb w23, [x22]

mov x24, #0		// FIX: true digit count starts at 0

int_to_text_loop:		// first loop to push the digits of the number into the stack
udiv x0, x19, x21
msub x1, x0, x21, x19

add w1, w1, #'0'	// converts the digit to ASCII

sub x22, x22, #1		// push to stack
strb w1, [x22]
add x24, x24, #1

mov x19, x0
cbnz x19, int_to_text_loop

cbz x20, int_to_text_positive

mov w1, #'-'
sub x22, x22, #1
strb w1, [x22]
add x24, x24, #1

int_to_text_positive:

mov x0, x24
add x0, x0, #1
bl alloc
mov x23, x0         // x23 : addr of string

mov x20, #0         // index = 0

mov x21, x22        // FIX: preserve true start pointer of string

int_to_text_copy:
ldrb w1, [x21, x20]
strb w1, [x23, x20]

add x20, x20, #1
cmp x20, x24
b.lt int_to_text_copy

mov w1, #0
strb w1, [x23, x20]

mov x0, x23

add sp, sp, #32

ldp x23, x24, [sp], #16
ldp x21, x22, [sp], #16
ldp x19, x20, [sp], #16

ldp fp, lr, [sp], #16
ret