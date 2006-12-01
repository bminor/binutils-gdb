#objdump: -dw
#name: x86-64 rex64 in/out

.*: +file format .*

Disassembly of section .text:

0+000 <_in>:
   0:	48 ed                	rex64 in     \(%dx\),%eax
   2:	66                   	data16
   3:	48 ed                	rex64 in     \(%dx\),%eax

0+005 <_out>:
   5:	48 ef                	rex64 out    %eax,\(%dx\)
   7:	66                   	data16
   8:	48 ef                	rex64 out    %eax,\(%dx\)

0+00a <_ins>:
   a:	48 6d                	rex64 insl   \(%dx\),%es:\(%rdi\)
   c:	66                   	data16
   d:	48 6d                	rex64 insl   \(%dx\),%es:\(%rdi\)

0+00f <_outs>:
   f:	48 6f                	rex64 outsl  %ds:\(%rsi\),\(%dx\)
  11:	66                   	data16
  12:	48 6f                	rex64 outsl  %ds:\(%rsi\),\(%dx\)
#pass