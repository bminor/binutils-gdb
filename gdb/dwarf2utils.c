/* DWARF 2 debugging format utils for GDB.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Check for GCC >= 4.x according to the symtab->producer string.  Return minor
   version (x) of 4.x in such case.  If it is not GCC or it is GCC older than
   4.x return -1.  If it is GCC 5.x or higher return INT_MAX.  */

#include "config.h"
#include "dwarf2utils.h"
#include "stdio.h"
#include "string.h"
#include "defs.h"
#include "dyn-string.h"
#include <ctype.h>

#include "common/common-types.h"
#include "common/common-exceptions.h"
#include "common/common-utils.h"

#if defined GDB_SELF_TEST
#include "selftest.h"
#endif

#include "utils.h"

int
producer_is_gcc_ge_4 (const char *producer)
{
  int major, minor;

  if (! producer_is_gcc (producer, &major, &minor))
    return -1;
  if (major < 4)
    return -1;
  if (major > 4)
    return INT_MAX;
  return minor;
}

/* Returns nonzero if the given PRODUCER string is GCC and sets the MAJOR
   and MINOR versions when not NULL.  Returns zero if the given PRODUCER
   is NULL or it isn't GCC.  */

int
producer_is_gcc (const char *producer, int *major, int *minor)
{
  const char *cs;

  if (producer != NULL && startswith (producer, "GNU "))
    {
      int maj, min;

      if (major == NULL)
	major = &maj;
      if (minor == NULL)
	minor = &min;

      /* Skip any identifier after "GNU " - such as "C11" "C++" or "Java".
	 A full producer string might look like:
	 "GNU C 4.7.2"
	 "GNU Fortran 4.8.2 20140120 (Red Hat 4.8.2-16) -mtune=generic ..."
	 "GNU C++14 5.0.0 20150123 (experimental)"
      */
      cs = &producer[strlen ("GNU ")];
      while (*cs && !isspace (*cs))
        cs++;
      if (*cs && isspace (*cs))
        cs++;
      if (sscanf (cs, "%d.%d", major, minor) == 2)
	return 1;
    }

  /* Not recognized as GCC.  */
  return 0;
}

/* Returns nonzero if the given PRODUCER string is Intel or zero
   otherwise.  Sets the MAJOR and MINOR versions when not NULL.

   Internal and external versions have to be taken into account.
   Before a public release string for the PRODUCER is slightly
   different than the public one.  Internal releases have mainly
   a major release number and 0 as minor release.  External
   releases have 4 fields, 3 of them are not 0 and only two
   are of interest, major and update.

   Examples are:

     Public release:
     "Intel(R) Fortran Intel(R) 64 Compiler XE for applications
     running on Intel(R) 64, Version 14.0.1.074 Build 20130716";
     "Intel(R) C++ Intel(R) 64 Compiler XE for applications
     running on Intel(R) 64, Version 14.0.1.074 Build 20130716";

     Internal releases:
     "Intel(R) C++ Intel(R) 64 Compiler for applications
     running on Intel(R) 64, Version 18.0 Beta ....".  */

bool
producer_is_icc (const char *producer, int *major, int *minor)
{
  if (producer == NULL || !startswith (producer, "Intel(R)"))
    return 0;

/* Preparing the used fields.  */
  int maj, min;
  if (major == NULL)
    major = &maj;
  if (minor == NULL)
    minor = &min;

  *minor = 0;
  *major = 0;

  /* Consumes the string till a "Version" is found.  */
  const char *cs = strstr(producer, "Version");
  cs = skip_to_space (cs);

  int intermediate = 0;
  int nof = sscanf (cs, "%d.%d.%d.%*d", major, &intermediate, minor);

  /* Internal versions are represented only as MAJOR.MINOR, whereas
     minor is usually 0.
     Public versions have 4 fields as described with the command above.  */
  if (nof == 3)
    return TRUE;

  if (nof == 2)
    {
      *minor = intermediate;
      return TRUE;
    }

  static bool warning_printed = FALSE;
  /* Not recognized as Intel, let user know.  */
  if (warning_printed == FALSE)
    {
      warning (_("Could not recognize version of Intel Compiler in:%s"), producer);
      warning_printed = TRUE;
    }
  return FALSE;
}

#if defined GDB_SELF_TEST
namespace selftests {
namespace dwarf2utils {

static void
dwarf_producer_test ()
{
  int major = 0, minor = 0;

  const char *extern_f_14_1 = "Intel(R) Fortran Intel(R) 64 Compiler \
XE for applications running on Intel(R) 64, Version \
14.0.1.074 Build 20130716";

  bool retval = producer_is_icc (extern_f_14_1, &major, &minor);
  SELF_CHECK (retval == 1 && major == 14 && minor == 1);
  retval = producer_is_gcc (extern_f_14_1, &major, &minor);
  SELF_CHECK (retval == 0);

  const char *intern_f_14 = "Intel(R) Fortran Intel(R) 64 Compiler \
XE for applications running on Intel(R) 64, Version \
14.0";

  major = 0;
  minor = 0;
  retval = producer_is_icc (intern_f_14, &major, &minor);
  SELF_CHECK (retval == 1 && major == 14 && minor == 0);
  retval = producer_is_gcc (intern_f_14, &major, &minor);
  SELF_CHECK (retval == 0);

  const char *intern_c_14 = "Intel(R) C++ Intel(R) 64 Compiler \
XE for applications running on Intel(R) 64, Version \
14.0";
  major = 0;
  minor = 0;
  retval = producer_is_icc (intern_c_14, &major, &minor);
  SELF_CHECK (retval == 1 && major == 14 && minor == 0);
  retval = producer_is_gcc (intern_c_14, &major, &minor);
  SELF_CHECK (retval == 0);

  const char *intern_c_18 = "Intel(R) C++ Intel(R) 64 Compiler \
for applications running on Intel(R) 64, Version 18.0 Beta";
  major = 0;
  minor = 0;
  retval = producer_is_icc (intern_c_18, &major, &minor);
  SELF_CHECK (retval == 1 && major == 18 && minor == 0);

  const char *gnu = "GNU C 4.7.2";
  major = 0;
  minor = 0;
  retval = producer_is_icc (gnu, &major, &minor);
  SELF_CHECK (retval == 0);
  retval = producer_is_gcc (gnu, &major, &minor);
  SELF_CHECK (retval == 1 && major == 4 && minor == 7);

  const char *gnu_exp ="GNU C++14 5.0.0 20150123 (experimental)";
  major = 0;
  minor = 0;
  retval = producer_is_icc (gnu_exp, &major, &minor);
  SELF_CHECK (retval == 0);
  retval = producer_is_gcc (gnu_exp, &major, &minor);
  SELF_CHECK (retval == 1 && major == 5 && minor == 0);
}
}
}
#endif



/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_dwarf2utils;

void
_initialize_dwarf2utils (void)
{
#if defined GDB_SELF_TEST
  selftests::register_test  (selftests::dwarf2utils::dwarf_producer_test);
#endif
}
