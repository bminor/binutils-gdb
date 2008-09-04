#include <stdio.h>
/* Test reverse "return" command.  */

void func1 ()
{
  printf("in func1\n");	/* break in func1 */
}

int
func2 ()
{
  return -5;		/* break in func2 */
}

double
func3 ()
{
  return -5.0;		/* break in func3 */
}

int tmp2;
double tmp3;

int main ()
{
  func1 ();		/* break in main */
  printf("in main after func1\n");
  tmp2 = func2 ();
  tmp3 = func3 ();
  printf("exiting\n");
  return 0;
}
