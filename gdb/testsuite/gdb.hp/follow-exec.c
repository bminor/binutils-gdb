#include <stdio.h>

int  global_i = 100;

main ()
{
  int  local_j = global_i+1;
  int  local_k = local_j+1;

  printf ("follow-exec is about to execlp(execd-program)...\n");

  execlp ("gdb.hp/execd-program",
          "gdb.hp/execd-program",
          "execlp arg1 from follow-exec",
          (char *)0);

  printf ("follow-exec is about to execl(execd-program)...\n");

  execl ("gdb.hp/execd-program",
         "gdb.hp/execd-program",
         "execl arg1 from follow-exec",
         "execl arg2 from follow-exec",
         (char *)0);

  {
    static char * argv[] = {
      "gdb.hp/execd-program",
      "execv arg1 from follow-exec",
      0};

    printf ("follow-exec is about to execv(execd-program)...\n");

    execv ("gdb.hp/execd-program", argv);
  }
}
