void gdb1253 (void);
void gdb1338 (void);
void jump_at_beginning (void);

int
main (void)
{
  gdb1253 ();
  gdb1338 ();
  jump_at_beginning ();
  return 0;
}

/* Relevant part of the prologue from symtab/1253.  */

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

/* Relevant part of the prologue from backtrace/1338.  */

asm(".text\n"
    "    .align 8\n"
    "gdb1338:\n"
    "    pushl %edi\n"
    "    pushl %esi\n"
    "    pushl %ebx\n"
    "    int   $0x03\n"
    "    popl  %ebx\n"
    "    popl  %esi\n"
    "    popl  %edi\n"
    "    ret\n");

/* The purpose of this function is to verify that, during prologue
   skip, GDB does not follow a jump at the beginnning of the "real"
   code.  */

asm(".text\n"
    "    .align 8\n"
    "jump_at_beginning:\n"
    "    pushl %ebp\n"
    "    movl  %esp,%ebp\n"
    "    jmp   .gdbjump\n"
    "    nop\n"
    ".gdbjump:\n"
    "    movl  %ebp,%esp\n"
    "    popl  %ebp\n"
    "    ret\n");
