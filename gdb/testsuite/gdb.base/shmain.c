/* A test */

#include "ss.h"
#include <stdio.h>

extern int shr1();
extern int shr2();
extern float sg;

int eglob;

struct {
 int a;  
 int b;
} s;

int g;

int local_structarg(x)
struct s x;
{
  return x.b;
}

main()
{
  struct s y;
  g = 1;
  g = shr1(g);
  g = shr2(g);
  g = mainshr1(g);
  sg = 1.1;
  printf("address of sg is 0x%x\n", &sg);
  y.a = 3;
  y.b = 4;
  g = local_structarg(y);
  g = structarg(y);
  g = pstructarg(&y);
}

int mainshr1(g)
int g;
{
  return 2*g;
}


