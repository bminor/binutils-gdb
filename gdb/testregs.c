typedef struct aa { int b;} *hack;

static int
foo (argc)
{
  register int a = 0x1234;
  register int b = 0x56788765;
  register char *x = (char *) 0xabababab;
  register char *y = (char *) 0xcdcdcdcd;
  register double d = 1.582;
  int loser;
  printf ("Address of loser is 0x%x.\n", &loser);
  printf ("Address of argc is 0x%x.\n", &argc);
  abort ();
}

main (argc)
{
  foo (argc);
}
