void gdb1253 (void);

int
main (void)
{
  gdb1253 ();
  return 0;
}

/* Relevant part of the prologue from gdb/1253.  */

asm(".text\n"
    "    .align 8\n"
    "gdb1253:\n"
    "    pushl %ebp\n"
    "    xorl  %ecx, %ecx\n"
    "    movl  %esp, %ebp\n"
    "    pushl %edi\n"
    "    int   $0x03\n"
    "    leave\n"
    "    ret\n");
