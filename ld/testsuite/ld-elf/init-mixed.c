#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#ifdef HAVE_INITFINI_ARRAY
static int count;

static void
init1005 ()
{
  if (count != 0)
    abort ();
  count = 1005;
}
void (*const init_array1005[]) ()
  __attribute__ ((section (".init_array.01005"), aligned (sizeof (void *))))
  = { init1005 };
static void
fini1005 ()
{
  if (count != 1005)
    abort ();
}
void (*const fini_array1005[]) ()
  __attribute__ ((section (".fini_array.01005"), aligned (sizeof (void *))))
  = { fini1005 };

static void
ctor1007 ()
{
  if (count != 1005)
    abort ();
  count = 1007;
}
void (*const ctors1007[]) ()
  __attribute__ ((section (".ctors.64528"), aligned (sizeof (void *))))
  = { ctor1007 };
static void
dtor1007 ()
{
  if (count != 1007)
    abort ();
  count = 1005;
}
void (*const dtors1007[]) ()
  __attribute__ ((section (".dtors.64528"), aligned (sizeof (void *))))
  = { dtor1007 };

static void
init65530 ()
{
  if (count != 1007)
    abort ();
  count = 65530;
}
void (*const init_array65530[]) ()
  __attribute__ ((section (".init_array.65530"), aligned (sizeof (void *))))
  = { init65530 };
static void
fini65530 ()
{
  if (count != 65530)
    abort ();
  count = 1007;
}
void (*const fini_array65530[]) ()
  __attribute__ ((section (".fini_array.65530"), aligned (sizeof (void *))))
  = { fini65530 };

static void
ctor65535 ()
{
  if (count != 65530)
    abort ();
  count = 65535;
}
void (*const ctors65535[]) ()
  __attribute__ ((section (".ctors"), aligned (sizeof (void *))))
  = { ctor65535 };
static void
dtor65535 ()
{
  if (count != 65535)
    abort ();
  count = 65530;
}
void (*const dtors65535[]) ()
  __attribute__ ((section (".dtors"), aligned (sizeof (void *))))
  = { dtor65535 };
#endif

int
main ()
{
  printf ("OK\n");
  return 0;
}
