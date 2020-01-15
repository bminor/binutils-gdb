/* config.h for the remote server for GDB.
   Copyright (C) 2020 Free Software Foundation, Inc.

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

/* This file exists so that "#include <config.h>" in header files
   (e.g. gnulib headers), includes this file, which then includes
   gnulib's config.h, along with our real config.h (which is called
   PROJECT-config.h) and other similar config.h files we may depend
   on.  */

#include <build-gnulib-gdbserver/config.h>

#undef PACKAGE_NAME
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME

#include "gdbserver-config.h"
