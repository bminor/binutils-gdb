
main ()
{
  foo(2);
  foo(3);
}

foo (i)
{
  printf ("i is %d, ", i);
  printf ("i*i is %d\n", i * i);
}
