.section        .eh_frame,"a"
.type   __EH_FRAME_BEGIN__, %object
__EH_FRAME_BEGIN__:

.text
.type   get_eh_frame_begin, %function
get_eh_frame_begin:
  .cfi_startproc purecap
  stp c29, c30, [csp, -64]!
  .cfi_def_cfa_offset 64
  .cfi_offset 227, -64
  .cfi_offset 228, -48
  adrp    c0, __EH_FRAME_BEGIN__
  adrp    c0, .eh_frame
  add     c0, c0, :lo12:__EH_FRAME_BEGIN__
  add	c0, c0, :lo12:.eh_frame
  ldp c29, c30, [csp], 64
  .cfi_restore 228
  .cfi_restore 227
  .cfi_def_cfa_offset 0
  ret
  .cfi_endproc

.global _start
.type _start, %function
_start:
  mov x0, #0
  ret

# .zero 0xff0 - 0x38 - 0x78
.zero 0xff0 - 0x38
