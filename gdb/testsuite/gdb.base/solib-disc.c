/* This testcase is part of GDB, the GNU debugger.

   Copyright 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include <stdio.h>
#include <stdlib.h>

#ifdef __WIN32__
#include <windows.h>
#define dlopen(name, mode) LoadLibrary (name)
#define dlsym(handle, func) GetProcAddress (handle, func)
#define dlclose(handle) FreeLibrary (handle)
#define dlerror() "an error occurred"
#else
#include <dlfcn.h>
#endif

int main()
{
  void *handle;
  void (*func) (void);

  handle = dlopen (SHLIB_NAME, RTLD_LAZY);
  if (!handle)
    {
      fprintf (stderr, dlerror ());
      exit (1);
    }

  func = (void (*)(void)) dlsym (handle, "shrfunc");
  if (!func)
    {
      fprintf (stderr, dlerror ());
      exit (1);
    }

  func ();

  dlclose (handle);

  return 0;
}
