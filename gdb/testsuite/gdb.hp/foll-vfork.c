#include <stdio.h>

main ()
{
  int  pid;

  pid = vfork ();
  if (pid == 0) {
    printf ("I'm the child!\n");
    execlp ("gdb.hp/vforked-program", "gdb.hp/vforked-program", (char *)0);
  }
  else {
    printf ("I'm the proud parent of child #%d!\n", pid);
  }
}
