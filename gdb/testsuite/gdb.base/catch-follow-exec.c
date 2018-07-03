#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main (void)
{
  char *exec_args[] = { "/bin/ls", "ppp", NULL };
  execve (exec_args[0], exec_args, NULL);
}
