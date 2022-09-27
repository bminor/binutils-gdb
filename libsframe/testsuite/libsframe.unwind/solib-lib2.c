#include <execinfo.h>
#include <stdlib.h>
#include <string.h>
#include "sframe-backtrace-api.h"
#include "solib-lib2.h"

#define BT_BUF_SIZE 100

/* funclist for running "ttest.x 3".  */
static const char *const bt_list[] =
{
  "adder2",
  "bar",
  "adder",
  "main"
};

unsigned int
adder2 (unsigned int a, unsigned int b, int (*call)(int))
{
  void *buffer[BT_BUF_SIZE];
  int i, nptrs, err;
  char **strings;

  nptrs = sframe_backtrace (buffer, BT_BUF_SIZE, &err);
  if (err)
    {
      printf ("SFrame error: %s\n", sframe_bt_errmsg (err));
      return (-1);
    }
  if (nptrs != 4)
    {
      printf ("sframe_backtrace failed: %d %d\n", nptrs, err);
      return (-1);
    }

  strings = backtrace_symbols (buffer, nptrs);
  if (strings == NULL)
    {
       printf ("Error in backtrace_symbols");
       return (-1);
    }

  /* Verify the results.  */
  for (i = 0; i < nptrs; i++)
    if (!strstr (strings[i], bt_list[i]))
      break;

  free (strings);

  printf ("%s: unwind solib test\n", i == nptrs ? "PASS" : "FAIL");

  (void)(*call) (a+b);
  return (a+b);
}
