main ()
{
  foo (4);
}

foo (x)
     int x;
{
  if (x > 0)
    foo (x - 1);
}

