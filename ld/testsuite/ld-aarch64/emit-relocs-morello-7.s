# Want to test the special "compatibility hack" where we treat a linker script
# defined symbol with a name of the form __.*_start or __start_.* which is at
# the very end of a section as being at the *start* of the next section.
#
# This is required now that symbols have permissions and bounds according to
# their section.
# Checking LD128 relocations against a linker script defined symbol which
# should get this compatibility hack (and some linker script defined symbols
# that should not get the compatibility hack).
.arch morello+c64
  .section .othersection,"aw",@progbits
  .asciz "Hello there ;-)"
  .data
  .word 10
  .text

obj:

  ldr     c0, [c0, :got_lo12:__text_start]
  ldr     c0, [c0, :got_lo12:__text_other]
  ldr     c0, [c0, :got_lo12:__data_start]
  ldr     c0, [c0, :got_lo12:__data_other]
