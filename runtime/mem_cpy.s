.global mem_cpy

mem_cpy:                // x0 and x1, hold the dest and source addresses,  x2 holds the number of bytes
mov x3, #0              // x0 will just need to keed the dest    // x3 : offset

mem_cpy_loop:
cbz x2, mem_cpy_exit
ldrb w4, [x1, x3]
strb w4, [x0, x3]

sub x2, x2, #1
add x3, x3, #1
b mem_cpy_loop

mem_cpy_exit:    
ret