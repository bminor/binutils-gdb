SECTIONS
{
  .text 0x1000 : {
    tmpdir/weak1.o(*)
  }
  .data 0x2000 : {
    tmpdir/weak2.o(*)
  }
}
