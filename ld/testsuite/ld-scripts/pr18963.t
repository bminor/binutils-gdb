SECTIONS
{
  .text 0x300 :
  {
    *(.text)
  }
  A = .;
  .data :
  {
    *(.data)
  }
  B = .;
  .bss :
  {
    *(.bss)
  }
  C = .;
  D = A - C + B;
  E = A + B - C;
  /DISCARD/ : {*(*)}
}

ASSERT(D == E, "Addition is not commutative");
