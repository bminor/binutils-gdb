 .text
 .globl _binary_builtin_ovl_mgr_start
 .globl _binary_builtin_ovl_mgr_end

_binary_builtin_ovl_mgr_start:
 .incbin "spu_ovl.o"
_binary_builtin_ovl_mgr_end:
