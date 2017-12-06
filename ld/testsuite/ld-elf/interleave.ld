MEMORY
{
  x0(xrw): ORIGIN = 0x0, LENGTH = 8
  x1(xrw): ORIGIN = 0xC, LENGTH = 8
}
SECTIONS
{
  .a0 : { *(.text.a0) } > x0
  .a1 : { *(.text.a1) } > x1
}
