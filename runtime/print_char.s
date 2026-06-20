.global print_char

print_char:     // x0 is the char to print

str x0, [sp, #-16]! // setup single char buffer on stack
mov x1, sp
mov x2, #1

mov x0, #1
mov x8, #64
svc #0

add sp, sp, #16 // clean up stack

ret
