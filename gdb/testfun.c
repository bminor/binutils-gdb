main ()
{
  register double a = 1.5;
  foo (a);
}

foo (x)
{
  printf ("%f\n", x);
}

do_add (x, y)
{
  return x + y;
}

double
do_float_add (x, y, z)
     register double x, y, z;
{
  return x + y + z;
}
