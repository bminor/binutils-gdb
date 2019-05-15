.macro cond mnem
.irp cond, eq, ne, gt, ge, lt, le
it \cond
\mnem\().u32 q0, [r0]
.endr
.endm

.syntax unified
.thumb
vldrb.8 q0, [r0, #128]
vldrb.8 q0, [r0, #-128]
vldrb.u16 q0, [r0, #128]
vldrb.u16 q0, [r0, #-128]
vldrb.u32 q0, [r0, #128]
vldrb.u32 q0, [r0, #-128]
vldrb.8 q0, [r0, #128]!
vldrb.8 q0, [r0, #-128]!
vldrb.u16 q0, [r0, #128]!
vldrb.u16 q0, [r0, #-128]!
vldrb.u32 q0, [r0, #128]!
vldrb.u32 q0, [r0, #-128]!
vldrb.8 q0, [r0], #128
vldrb.8 q0, [r0], #-128
vldrb.u16 q0, [r0], #128
vldrb.u16 q0, [r0], #-128
vldrb.u32 q0, [r0], #128
vldrb.u32 q0, [r0], #-128
vldrb.u16 q0, [r10, #2]
vldrb.u16 q0, [r10, #2]!
vldrb.u16 q0, [r10], #2
vldrb.8 q0, [sp, #2]!
vldrb.8 q0, [sp], #2
vldrb.8 q0, [pc, #2]
cond vldrb
vldrb.16 q0, [r0]
vldrb.f16 q0, [r0]
vldrb.p16 q0, [r0]
vldrb.32 q0, [r0]
vldrb.f32 q0, [r0]
vldrh.16 q0, [r0, #1]
vldrh.16 q0, [r0, #17]
vldrh.16 q0, [r0, #-17]
vldrh.16 q0, [r0, #256]
vldrh.16 q0, [r0, #-256]
vldrh.u32 q0, [r0, #1]
vldrh.u32 q0, [r0, #17]
vldrh.u32 q0, [r0, #-17]
vldrh.u32 q0, [r0, #256]
vldrh.u32 q0, [r0, #-256]
vldrh.16 q0, [r0, #1]!
vldrh.16 q0, [r0, #17]!
vldrh.16 q0, [r0, #-17]!
vldrh.16 q0, [r0, #256]!
vldrh.16 q0, [r0, #-256]!
vldrh.s32 q0, [r0, #1]!
vldrh.s32 q0, [r0, #17]!
vldrh.s32 q0, [r0, #-17]!
vldrh.s32 q0, [r0, #256]!
vldrh.s32 q0, [r0, #-256]!
vldrh.16 q0, [r0], #1
vldrh.16 q0, [r0], #17
vldrh.16 q0, [r0], #-17
vldrh.16 q0, [r0], #256
vldrh.16 q0, [r0], #-256
vldrh.u32 q0, [r0], #1
vldrh.u32 q0, [r0], #17
vldrh.u32 q0, [r0], #-17
vldrh.u32 q0, [r0], #256
vldrh.u32 q0, [r0], #-256
vldrh.u32 q0, [r10, #4]
vldrh.16 q0, [sp, #2]!
vldrh.16 q0, [sp], #2
vldrh.16 q0, [pc, #2]
cond vldrh
vldrh.8 q0, [r0]
vldrh.u8 q0, [r0]
vldrh.s8 q0, [r0]
vldrh.p8 q0, [r0]
vldrh.32 q0, [r0]
vldrh.f32 q0, [r0]
vldrw.32 q0, [r0, #3]
vldrw.32 q0, [r0, #-3]
vldrw.32 q0, [r0, #514]
vldrw.32 q0, [r0, #-258]
vldrw.32 q0, [r0, #258]
vldrw.32 q0, [r0, #516]
vldrw.32 q0, [r0, #-516]
vldrw.32 q0, [r0, #3]!
vldrw.32 q0, [r0, #-3]!
vldrw.32 q0, [r0, #514]!
vldrw.32 q0, [r0, #-258]!
vldrw.32 q0, [r0, #258]!
vldrw.32 q0, [r0, #516]!
vldrw.32 q0, [r0, #-516]!
vldrw.32 q0, [r0], #3
vldrw.32 q0, [r0], #-3
vldrw.32 q0, [r0], #514
vldrw.32 q0, [r0], #-258
vldrw.32 q0, [r0], #258
vldrw.32 q0, [r0], #516
vldrw.32 q0, [r0], #-516
vldrw.32 q0, [sp, #4]!
vldrw.32 q0, [pc, #4]
cond vldrw
vldrw.8 q0, [r0]
vldrw.u8 q0, [r0]
vldrw.s8 q0, [r0]
vldrw.p8 q0, [r0]
vldrw.16 q0, [r0]
vldrw.u16 q0, [r0]
vldrw.s16 q0, [r0]
vldrw.f16 q0, [r0]
vldrw.p16 q0, [r0]
it eq
vldrbeq.8 q0, [r0]
vldrbeq.8 q0, [r0]
vpst
vldrbeq.8 q0, [r0]
vldrbt.8 q0, [r0]
vpst
vldrb.8 q0, [r0]
it eq
vldrheq.16 q0, [r0]
vldrheq.16 q0, [r0]
vpst
vldrheq.16 q0, [r0]
vldrht.16 q0, [r0]
vpst
vldrh.16 q0, [r0]
it eq
vldrweq.32 q0, [r0]
vldrweq.32 q0, [r0]
vpst
vldrweq.32 q0, [r0]
vldrwt.32 q0, [r0]
vpst
vldrw.32 q0, [r0]

