extern void abort (void);

asm (".secondary bar");

void
bar (void)
{
  abort ();
}
