.global usr_input

.equ usr_input_buffer_size, 256

usr_input:                 // no input params, returns addr to string

stp fp, lr, [sp, #-16]!
mov fp, sp

stp x19, x20, [sp, #-16]!
stp x21, x22, [sp, #-16]!
stp x23, x24, [sp, #-16]!

mov x0, #0
adr x1, usr_input_buffer
mov x2, usr_input_buffer_size
mov x8, #63
svc #0

adr x19, usr_input_buffer       // x19 : hold input buffer addr
mov x20, #0                     // x20 : idx / length

usr_input_get_len:
ldrb w21, [x19, x20]
cmp w21, #0
b.eq usr_input_len_done
cmp w21, #10
b.eq usr_input_len_done
add x20, x20, #1
b usr_input_get_len

usr_input_len_done:

mov w21, #0
strb w21, [x19, x20]

mov x0, x20
add x0, x0, #1
bl alloc
mov x23, x0                     // x23 : allocated string address

mov x22, #0     
                // x22 : copying idx
usr_input_alloc:
cmp x22, x20
b.ge usr_input_alloc_done

ldrb w21, [x19, x22]
strb w21, [x23, x22]

add x22, x22, #1
b usr_input_alloc

usr_input_alloc_done:

strb wzr, [x23, x20]

mov x0, x23

ldp x23, x24, [sp], #16
ldp x21, x22, [sp], #16
ldp x19, x20, [sp], #16

ldp fp, lr, [sp], #16
ret

.data

usr_input_buffer:
.space usr_input_buffer_size
