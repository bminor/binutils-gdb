// debug.h -- gold internal debugging support   -*- C++ -*-

// Copyright 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef GOLD_DEBUG_H
#define GOLD_DEBUG_H

#include "parameters.h"
#include "errors.h"

namespace gold
{

// The different types of debugging we support.  These are bitflags.

const int DEBUG_TASK = 1;
const int DEBUG_SCRIPT = 2;

const int DEBUG_ALL = DEBUG_TASK | DEBUG_SCRIPT;

// Print a debug message if TYPE is enabled.  This is a macro so that
// we only evaluate the arguments if necessary.

#define gold_debug(TYPE, FORMAT, ...)				\
  do								\
    {								\
      if (is_debugging_enabled(TYPE))				\
	parameters->errors()->debug(FORMAT, __VA_ARGS__);	\
    }								\
  while (0)

} // End namespace gold.

#endif // !defined(GOLD_DEBUG_H)
