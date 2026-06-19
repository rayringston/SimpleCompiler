.global str_len

str_len:
                        // x0 is the address of a null-terminated string buffer
mov x9, #0              // x9 will count the length

str_len_loop:
ldrb w10, [x0, x9]
cbz w10, str_len_done   // if the byte is 0 (\0), we are at the end of the string

add x9, x9, #1
b str_len_loop

str_len_done:
mov x0, x9

ret
