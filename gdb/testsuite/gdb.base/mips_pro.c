/* Tests regarding examination of prologues.  */

int
inner (z)
     int z;
{
  return 2 * z;
}

/* The native compiler seems to lack asm().  */

#if defined (__GNUC__) && defined (__mips__)
/* This is the verbatim output corresponding to the function middle.
   Label names have been changed to avoid conflicts.  It is here as
   asm() so we don't have to worry that (a) the compiler might change
   how it does things, and (b) we'd have to insure it gets compiled
   with gcc -O2.  */

asm ("\n\
	.text \n\
	.align 2 \n\
	.globl	middle \n\
	.loc	1 22 \n\
\n\
	.loc	1 21 \n\
	.ent	middle \n\
middle: \n\
	.frame	$sp,24,$31     	# vars= 0, regs= 1/0, args = 16, extra= 0 \n\
	.mask	0x80000000,-8 \n\
	.fmask	0x00000000,0 \n\
	subu	$sp,$sp,24 \n\
\n\
	.loc	1 23 \n\
	.set	noreorder \n\
	.set	nomacro \n\
	beq	$4,$0,middle_lab0 \n\
	sw	$31,16($sp) \n\
	.set	macro \n\
	.set	reorder \n\
\n\
\n\
	.loc	1 26 \n\
	.set	noreorder \n\
	.set	nomacro \n\
	jal	inner \n\
	li	$4,0x00000006		# 6 \n\
	.set	macro \n\
	.set	reorder \n\
\n\
	j	middle_lab1 \n\
middle_lab0: \n\
\n\
	.loc	1 24 \n\
	.set	noreorder \n\
	.set	nomacro \n\
	jal	inner \n\
	li	$4,0x00000005		# 5 \n\
	.set	macro \n\
	.set	reorder \n\
\n\
middle_lab1: \n\
	.set	noreorder \n\
	lw	$31,16($sp) \n\
	nop \n\
	j	$31 \n\
	addu	$sp,$sp,24 \n\
	.set	reorder \n\
	.end	middle \n\
");
#if 0
/* Not necessary, and loses if gcc is not using stabs.  */
asm (".stabs \"middle:F1\",36,0,13,middle");
asm (".stabs \"x:p1\",160,0,13,0");
#endif
#else
int
middle (x)
     int x;
{
  if (x == 0)
    return inner (5);
  else
    return inner (6);
}
#endif

int
top (y)
     int y;
{
  return middle (y + 1);
}

int
main (argc, argv)
{
  return top (-1) + top (1);
}
