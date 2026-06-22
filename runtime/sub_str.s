.global sub_str

sub_str:                 // x0 = TEXT addr, x1 = INT, x2 = INT

stp fp, lr, [sp, #-16]!
mov fp, sp

stp x19, x20, [sp, #-16]!
stp x21, x22, [sp, #-16]!
stp x23, x24, [sp, #-16]!

mov x19, x0             // x19 : source string addr
mov x20, x1             // x20 : start idx
mov x21, x2             // x21 : end idx

cmp x21, x20            // make sure end >= start
b.lt sub_str_oob

cmp x21, #0             // make sure end >= 0
b.lt sub_str_oob

cmp x20, #0             // make sure start >= 0
b.lt sub_str_oob

bl str_len              // x0 will hold length of input string
cmp x21, x0
b.gt sub_str_oob        // if end index > len, oob exit

sub x22, x21, x20
add x0, x22, #1         // move the size of space to x0,

bl alloc
mov x22, x0             // x22 : addr of the output string

mov x0, x22         
add x1, x19, x20        // source addr = src + start offset
sub x2, x21, x20
bl mem_cpy

sub x23, x21, x20
add x23, x23, #1        // get offest for the next byte, to copy over \0
strb wzr, [x22, x23]
mov x0, x22

ldp x23, x24, [sp], #16
ldp x21, x22, [sp], #16
ldp x19, x20, [sp], #16

ldp fp, lr, [sp], #16
ret

sub_str_oob:
adr x1, sub_str_oob_message
mov x2, sub_str_oob_message_len
mov x0, #1
mov x8, #64
svc #0

mov x0, #1
mov x8, #93
svc #0
ret                 // shouldnt reach here

.data
sub_str_oob_message:
.asciz "Error: Index is out of bounds of string.\n"
sub_str_oob_message_len = . - sub_str_oob_message
