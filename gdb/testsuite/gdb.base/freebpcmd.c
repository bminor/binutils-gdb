int
main (int argc, char **argv)
{
  int i;

#ifdef usestubs
  set_debug_traps();
  breakpoint();
#endif

  for (i = 0; i < 100; i++)
    printf (">>> %d\n", i); /* euphonium */

  return 0;
}
