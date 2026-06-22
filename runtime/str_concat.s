.global str_concat

str_concat:                 // x0 = str1, x1 = str2

stp fp, lr, [sp, #-16]!
mov fp, sp

stp x19, x20, [sp, #-16]!
stp x21, x22, [sp, #-16]!
str x23, [sp, #-16]!

mov x19, x0                 // x19 : str1
mov x20, x1                 // x20 : str2

// len1
mov x0, x19
bl str_len
mov x21, x0                 // x21 : len 1

// len2
mov x0, x20
bl str_len
mov x22, x0                 // x22 : len 2

// alloc(len1 + len2 + 1)
add x0, x21, x22
add x0, x0, #1
mov x24, x0                 // hold this final length to add the null-pointer at the end

bl alloc                    // x0 holds destination pointer
mov x23, x0                 // x3 : dest pointer

mov x1, x19
mov x2, x21
bl mem_cpy                  // copy str1 to dest

add x0, x0, x21             // offset the dest string by the length of str1
mov x1, x20
add x2, x22, #1                 // copy str2 to dest + offset
bl mem_cpy                  // x0 will hold the offsetted addr

//strb wzr, [x23, x24]

mov x0, x23               // return destination pointer

ldr x23, [sp], #16
ldp x21, x22, [sp], #16
ldp x19, x20, [sp], #16

ldp fp, lr, [sp], #16
ret
