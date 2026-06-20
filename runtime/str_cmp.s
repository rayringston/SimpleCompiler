.global str_cmp

str_cmp:    // x0 is the addr of first cstring, x1 is the addr of second cstring
            // return 0 if the strings are equal, otherwise the difference between the first non-matching characters
mov x9, #0

str_cmp_loop:
ldrb w10, [x0, x9]
ldrb w11, [x1, x9]

subs w12, w10, w11
cbnz w12, str_cmp_done

cbz w10, str_cmp_done
cbz w11, str_cmp_done

add x9, x9, #1
b str_cmp_loop

str_cmp_done:
mov x0, x12
ret
