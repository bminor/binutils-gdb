/* Test various kinds of stepping.
*/
int glob = 0;

int callee() {
    glob++;
    return (0);
}

int main () {
   int w,x,y,z;
   int a[10], b[10];

   /* Test "next" and "step" */
   w = 0;
   x = 1;
   y = 2;
   z = 3;
   w = w + 2;
   x = x + 3;
   y = y + 4;
   z = z + 5;

   /* Test that "next" goes over a call */
   callee(); /* OVER */

   /* Test that "step" doesn't */
   callee(); /* INTO */

   /* Test "stepi" */
   a[5] = a[3] - a[4];
   callee(); /* STEPI */
   
   /* Test "nexti" */
   callee(); /* NEXTI */

   y = w + z;

   return (0);
}
