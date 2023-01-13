SECTIONS
{
  . = 0x80000;
  A = .;
  .text :
  {
    _start = .;
    *(.text)
    . = 0x10000;
  }
  B = .;
  .data :
  {
    *(.data)
    . = 0x10000;
  }
  C = .;
  .bss :
  {
    *(.bss)
    . = 0x10000;
  }
  D = A - C + B;
  E = A + B - C;
  /DISCARD/ : {*(*)}
}

ASSERT(D == E, "Addition is not commutative");
