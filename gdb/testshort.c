main ()
{
  register int ntabs;
  register short x;
  short stops[30];

  foo ();
}

foo ()
{
  register int ntabs;
  register short x;

  printf (x, ntabs);
}
