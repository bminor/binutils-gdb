#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <stdio.h>

main ()
{
  struct user u;
  printf ("&u.u_ar0 - &u = %d, 0%o\n", (int) &u.u_ar0 - (int) &u,
	  (int) &u.u_ar0 - (int) &u);
  printf ("sizeof (struct pcb) = %d, 0%o\n",
	  sizeof (struct pcb), sizeof (struct pcb));
}
