/* Run this program straight.  Then set a breakpoint in `dump',
   run it again, and continue the program.  Diff the outputs
   and you will see what the supplied debugger does for a breakpoint.  */

int dump ();
int after_dump ();

main ()
{
  printf ("main = 0x%x\ndump = 0x%x\nend = 0x%x\n", main, dump, after_dump);

  dump (main, after_dump);
}

int dump (p, q)
     int *p;
     int *q;
{
  int cnt = 0;
  printf ("dump: 0x%x-0x%x\n", p, q);

  while (p < q)
    {
      if ((cnt++ & 3) == 0)
	printf ("\n0x%08x: ", p);
      printf ("0x%08x ", *p++);
    }
  printf ("\n");
}

after_dump ()
{
}
