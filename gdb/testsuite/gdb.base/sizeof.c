int
main ()
{
  printf ("sizeof (char) == %d\n", sizeof (char));
  printf ("sizeof (short) == %d\n", sizeof (short));
  printf ("sizeof (int) == %d\n", sizeof (int));
  printf ("sizeof (long) == %d\n", sizeof (long));
  printf ("sizeof (long long) == %d\n", sizeof (long long));

  printf ("sizeof (void *) == %d\n", sizeof (void*));
  printf ("sizeof (void (*)(void)) == %d\n", sizeof (void (*)(void)));

  printf ("sizeof (float) == %d\n", sizeof (float));
  printf ("sizeof (double) == %d\n", sizeof (double));
  printf ("sizeof (long double) == %d\n", sizeof (long double));
  return 0;
}
