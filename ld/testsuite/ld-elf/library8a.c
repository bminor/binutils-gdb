#if 1
asm (".secondary bar");
#else
asm (".weak bar");
#endif

void
bar (void)
{
}
