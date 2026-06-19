.global print_cstr

print_cstr:     // x0 is the address of a null-terminated string buffer

sub sp, sp, #16  // must perserve the sp and lr, since print_cstr calls other functions
stp fp, lr, [sp]

mov x1, x0
bl str_len

mov x2, x0
mov x0, x1
mov x1, x2
bl print_str

ldp fp, lr, [sp]
add sp, sp, #16

ret
