/* This program is used to test the "jump" command.  There's nothing
   particularly deep about the functionality nor names in here.
   */

static int square (x)
  int  x;
{
  return x*x;
}


main ()
{
  int i = 99;

  i++;
  i = square (i);
  i--;
}
