.thumb
.syntax unified

T:
blx r4
blx r9
bx  r4
bx  r9
tt    r0, r1
tt    r8, r9
ttt   r0, r1
ttt   r8, r9
movw  r0, #0xF123
movw  r8, #0xF123
movt  r0, #0xF123
movt  r8, #0xF123
cbz   r4, .L1
cbnz  r4, .L1
b.w   .L1
sdiv  r0, r1, r2
sdiv  r8, r9, r10
udiv  r0, r1, r2
udiv  r8, r9, r10
.L1:
 add   r0, r1
clrex
ldrex  r0, [r1, #0x4]
ldrexb r0, [r1]
ldrexh r0, [r1]
strex  r0, r1, [r2, #0x4]
strexb r0, r1, [r2]
strexh r0, r1, [r2]
lda    r0, [r1]
ldab   r0, [r1]
ldah   r0, [r1]
stl    r0, [r1]
stlb   r0, [r1]
stlh   r0, [r1]
ldaex  r0, [r1]
ldaexb r0, [r1]
ldaexh r0, [r1]
stlex  r0, r1, [r2]
stlexb r0, r1, [r2]
stlexh r0, r1, [r2]


