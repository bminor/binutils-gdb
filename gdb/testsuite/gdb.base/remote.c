#include <stdio.h>
#include <stdlib.h>

/************************************************************************** 
 * TESTS :
 *   -- downloading of a rather large executable
 ***************************************************************************/ 


/* A large array in .data.  If RLE compression becomes available during
   downloads, this would have to become a bunch of real random data.  
   Here's a quick way of generating such a bunch:

yes | awk '{printf ("%4d,", rand()*1000);}' | fold -w80 -s | head -4096

*/

unsigned long random_data[65536] = { 1 };

int
main() 
{
  printf ("%lu\n", random_data [rand() % 
			       (sizeof (random_data) /
			        sizeof (random_data [0]))]);
}
