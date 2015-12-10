/* Misc. GOOGLE LOCAL code.

   Copyright (C) 2015 Free Software Foundation, Inc.

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

#include "defs.h"
#include "google.h"

/* Bug 1155829.
   Warn about troublesome environment variables.
   While it might be nice to move this to python/lib/google/google3/__init__.py
   we'd have to export the quiet flag, which could be useful in general, but
   later.  */

void
warn_troublesome_environment (void)
{
  const char *ld_library_path;

  if (getenv ("LD_ASSUME_KERNEL") != NULL)
    {
      printf_filtered ("\n");
      printf_filtered (_("WARNING: GDB has detected that you have set the\n"
                         "LD_ASSUME_KERNEL environment variable.\n"));
      printf_filtered (_("You may have intended to do this, but you should know\n"
                         "that this can often cause problems while debugging.\n"));
      printf_filtered (_("For more information see http://wiki/Main/GdbFaq#LD_ASSUME_KERNEL_usage_warning\n"));
      printf_filtered ("\n");
    }

  ld_library_path = getenv ("LD_LIBRARY_PATH");
  if (ld_library_path != NULL
      && strstr (ld_library_path, "/usr/lib/debug") != NULL)
    {
      printf_filtered ("\n");
      printf_filtered (_("WARNING: GDB has detected that your LD_LIBRARY_PATH environment variable\n"
                         "includes /usr/lib/debug. You may have intended this, but if your program\n"
                         "is using a different libc than /usr/lib/libc.so, it is the wrong thing to do.\n"
                         "For example Crosstool V11 and higher use the libc from GRTE.\n"));
      printf_filtered (_("For more information see http://wiki/Main/GdbFaq#LD_LIBRARY_PATH_usage_warning\n"));
      printf_filtered ("\n");
    }
}

extern initialize_file_ftype _initialize_google;

void
_initialize_google (void)
{
}
