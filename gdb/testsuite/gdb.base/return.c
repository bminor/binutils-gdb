/*  Test "return" command.  */

func1 ()
{
  printf("in func1\n");
}

int
func2 ()
{
  return -5;
}

double
func3 ()
{
  return -5.0;
}

main ()
{
  func1 ();
  printf("in main after func1\n");
  printf("func2 = %d\n", func2 ());
  printf("func3 = %f\n", func3 ());
}
