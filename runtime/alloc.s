.global alloc

.equ ALLOC_POOL_SIZE, 8192

alloc:              // size in x0, return pointer in x0
adr x1, alloc_pool_ptr      // x1 will hold the curr addr in the pool
ldr x1, [x1]

mov x3, x1                  // x3 will save this addr

add x0, x1, x0              // x0 will add the new size to it, and hold the new addr
adr x2, alloc_pool
add x2, x2, ALLOC_POOL_SIZE
cmp x0, x2
b.hi alloc_overflow

adr x1, alloc_pool_ptr
str x0, [x1]
mov x0, x3

ret

alloc_overflow:
adr x1, alloc_overflow_msg
mov x2, alloc_overflow_msg_len
mov x0, #1
mov x8, #64
svc #0

mov x0, #1
mov x8, #93
svc #0
ret                 // shouldnt reach here

.data

alloc_pool:
.space ALLOC_POOL_SIZE

alloc_pool_ptr:
.quad alloc_pool

alloc_overflow_msg:
.asciz "Error: Out of memory in bump allocator.\n"
alloc_overflow_msg_len = . - alloc_overflow_msg