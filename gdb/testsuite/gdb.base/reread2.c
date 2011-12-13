/* pr 13484 */

#include <stdio.h>

int x;

void foo()
{
  x++;
  printf("This is foo\n");
}

int main()
{
  foo();
  return 0;
}
