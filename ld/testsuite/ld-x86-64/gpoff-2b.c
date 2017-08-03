#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <syscall.h>
#include <asm/prctl.h>

extern int __gp;
extern int foo_gpoff;
extern int get_foo (void);
extern int get_foo_gpoff (void);

int
setup_gp (void *p)
{
  return syscall (SYS_arch_prctl, ARCH_SET_GS, p);
}

int
main ()
{
  if (setup_gp (&__gp) == 0
      && *(int *) ((char *) &__gp + foo_gpoff) == 0x12345678
      && *(int *) ((char *) &__gp + get_foo_gpoff ()) == 0x12345678
      && get_foo () == 0x12345678)
    printf ("PASS\n");
  return 0;
}
