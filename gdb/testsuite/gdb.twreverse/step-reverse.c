#include <stdlib.h>
#include <string.h>

/* Test various kinds of stepping.
*/
int myglob = 0;

int callee() {
  myglob++; return 0;	/* ARRIVED IN CALLEE */
}			/* RETURN FROM CALLEE */

/* A structure which, we hope, will need to be passed using memcpy.  */
struct rhomboidal {
  int rather_large[100];
};

void
large_struct_by_value (struct rhomboidal r)
{
  myglob += r.rather_large[42]; /* step-test.exp: arrive here 1 */
}

int main () {
   int w,x,y,z;
   int a[10], b[10];

   /* Test "next" and "step" */
   w = 0;	/* BREAK AT MAIN */
   x = 1;	/* NEXT TEST 1 */
   y = 2;	/* STEP TEST 1 */
   z = 3;	/* REVERSE NEXT TEST 1 */
   w = w + 2;	/* NEXT TEST 2 */
   x = x + 3;	/* REVERSE STEP TEST 1 */
   y = y + 4;
   z = z + 5;	/* STEP TEST 2 */

   /* Test that "next" goes over a call */
   callee(); /* NEXT OVER THIS CALL */

   /* Test that "step" doesn't */
   callee(); /* STEP INTO THIS CALL */

   /* Test "stepi" */
   a[5] = a[3] - a[4]; /* FINISH TEST */
   callee(); /* STEPI */
   
   /* Test "nexti" */
   callee(); /* NEXTI */

   y = w + z;

   {
     struct rhomboidal r;
     memset (r.rather_large, 0, sizeof (r.rather_large));
     r.rather_large[42] = 10;
     large_struct_by_value (r);  /* step-test.exp: large struct by value */
   }

   exit (0);
}

