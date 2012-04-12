#objdump: -dr --prefix-addresses --show-raw-insn
#name: STM and LDM

# Test the `STM*' and `LDM*' instructions

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <.*> e92d0001 	push	{r0}
0+004 <.*> e92d000e 	push	{r1, r2, r3}
0+008 <.*> e92d0200 	push	{r9}
0+00c <.*> e8bd0200 	pop	{r9}
0+010 <.*> e8bd000e 	pop	{r1, r2, r3}
0+014 <.*> e8bd0001 	pop	{r0}
