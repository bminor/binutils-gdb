// script.h -- handle linker scripts for gold   -*- C++ -*-

// Copyright 2006, 2007 Free Software Foundation, Inc.
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

// We implement a subset of the original GNU ld linker script language
// for compatibility.  The goal is not to implement the entire
// language.  It is merely to implement enough to handle common uses.
// In particular we need to handle /usr/lib/libc.so on a typical
// GNU/Linux system, and we want to handle linker scripts used by the
// Linux kernel build.

#ifndef GOLD_SCRIPT_H
#define GOLD_SCRIPT_H

namespace gold
{

class General_options;
class Command_line;
class Symbol_table;
class Layout;
class Input_argument;
class Input_objects;
class Input_group;
class Input_file;
class Task_token;
class Workqueue;

// FILE was found as an argument on the command line, but was not
// recognized as an ELF file.  Try to read it as a script.  We've
// already read BYTES of data into P.  Return true if the file was
// handled.  This has to handle /usr/lib/libc.so on a GNU/Linux
// system.

bool
read_input_script(Workqueue*, const General_options&, Symbol_table*, Layout*,
		  Dirsearch*, Input_objects*, Input_group*,
		  const Input_argument*, Input_file*, const unsigned char* p,
		  off_t bytes, Task_token* this_blocker,
		  Task_token* next_blocker);

// FILE was found as an argument to --script (-T).
// Read it as a script, and execute its contents immediately.

bool
read_commandline_script(const char* filename, Command_line*);

} // End namespace gold.

#endif // !defined(GOLD_SCRIPT_H)
