/* Simple little program that just generates a core dump from inside some
   nested function calls. */


void
func2 ()
{
  abort ();
}

void
func1 ()
{
  func2 ();
}

main ()
{
  func1 ();
}

