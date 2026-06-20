.global str_char_at

str_char_at:            // cstr addr in x0, index in x1, return char in x0
mov x9, #0              // count index with x9

srt_char_at_loop:
ldrb w10, [x0, x9]
cbz w10, str_char_at_done

cmp x9, x1              // finish if at the index
b.eq str_char_at_done   

add x9, x9, #1          // increment and loop if not
b srt_char_at_loop

str_char_at_done:       // move result to x0, and return
mov x0, x10
ret
