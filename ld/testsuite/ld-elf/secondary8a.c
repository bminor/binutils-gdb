#if 1
asm (".secondary __bar");
asm (".secondary bar");
#else
asm (".weak __bar");
asm (".weak bar");
#endif

void
__bar (void)
{
}
asm (".set bar, __bar");
