#include "defs.h"
#include "param.h"

static char *args[] = {"kdb", "kdb-symbols", 0};

static char *environment[] = {0};

char **environ;

start ()
{
#ifdef NAMES_HAVE_UNDERSCORE
  INIT_STACK (_kdb_stack_beg, _kdb_stack_end);
#else /* not NAMES_HAVE_UNDERSCORE */
  INIT_STACK (kdb_stack_beg, kdb_stack_end);
#endif /* not NAMES_HAVE_UNDERSCORE */

  environ = environment;

  main (2, args, environment);
}
