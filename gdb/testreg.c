main (argc, argv)
     int argc;
     char **argv;
{
  register int d1 = 34;
  register int d2 = 35;
  register int d3 = 36;
  register int d4 = 37;
  register char *a1 = "1";
  register char *a2 = "2";
  register char *a3 = "3";
  register char *a4 = "4";
  register char *a5 = "5";
  int x[4];
  foo ();
  foo (0x222, abort (), 0x444);
}

foo ()
{
  return 22;
}
