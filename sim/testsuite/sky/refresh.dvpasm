.include "sce_macros.s"
.global gpu_refresh
.text
gpu_refresh:
DMAcnt *
direct *
GIFpacked REGS={A_D}, NLOOP=1, EOP	; WAS GPUIFpacked REG=...
iwzyx	0x00000000, 0x0000007f, 0x00000000, 0x00000000
.endgif					; New Line
.EndDirect
.EndDmaData
DMAend
