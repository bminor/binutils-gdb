SECTIONS 
{
  .data :
  {
    LONG (foo)
    LONG (bar)
    *(.data)
  }
  PROVIDE (foo = .);
  PROVIDE (bar = .);
}
