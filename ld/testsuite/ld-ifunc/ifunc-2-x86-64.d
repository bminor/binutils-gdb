#ld: -shared
#objdump: -dw
#target: x86_64-*-*

#...
[ \t0-9a-f]+:[ \t0-9a-f]+call[ \t0-9a-fq]+<\*ABS\*@plt>
[ \t0-9a-f]+:[ \t0-9a-f]+lea[ \t]+.*\(%rip\),%rax.*[ \t0-9a-fq]+<\*ABS\*@plt>
#pass
