.global str_concat

str_concat:                 // x0 = str1, x1 = str2

stp fp, lr, [sp, #-16]!
mov fp, sp

stp x19, x20, [sp, #-16]!
stp x21, x22, [sp, #-16]!
str x23, [sp, #-16]!

mov x19, x0                // str1
mov x20, x1                // str2

// len1
mov x0, x19
bl str_len
mov x21, x0

// len2
mov x0, x20
bl str_len
mov x22, x0

// alloc(len1 + len2 + 1)
add x0, x21, x22
add x0, x0, #1
bl alloc

mov x23, x0                // save destination pointer

// copy first string
mov x1, #0                 // total offset
mov x6, x21                // remaining chars

str_concat_first:
cbz x6, str_concat_setup_second

ldrb w7, [x19, x1]
strb w7, [x23, x1]

sub x6, x6, #1
add x1, x1, #1
b str_concat_first

// copy second string
str_concat_setup_second:
mov x6, x22

str_concat_second:
cbz x6, str_concat_done

sub x2, x1, x21
ldrb w7, [x20, x2]
strb w7, [x23, x1]

sub x6, x6, #1
add x1, x1, #1
b str_concat_second

// write null terminator
str_concat_done:
strb wzr, [x23, x1]

mov x0, x23               // return destination pointer

ldr x23, [sp], #16
ldp x21, x22, [sp], #16
ldp x19, x20, [sp], #16

ldp fp, lr, [sp], #16
ret
