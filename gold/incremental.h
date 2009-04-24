// inremental.h -- incremental linking support for gold   -*- C++ -*-

// Copyright 2009 Free Software Foundation, Inc.
// Written by Mikolaj Zalewski <mikolajz@google.com>.

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

#ifndef GOLD_INCREMENTAL_H
#define GOLD_INCREMENTAL_H

#include <vector>

#include "stringpool.h"
#include "workqueue.h"

namespace gold
{

class Archive;
class Input_argument;
class Incremental_inputs_checker;
class Object;
class Output_section_data;

// This class contains the information needed during an incremental
// build about the inputs necessary to build the .gnu_incremental_inputs.
class Incremental_inputs
{
 public:
  Incremental_inputs()
    : command_line_key_(0), strtab_(new Stringpool())
  { }
  ~Incremental_inputs() { delete this->strtab_; }

  // Record the command line.
  void
  report_command_line(int argc, const char* const* argv);
  
  // Prepare for layout.  Called from Layout::finalize.
  void
  finalize();

  // Create the content of the .gnu_incremental_inputs section.
  Output_section_data*
  create_incremental_inputs_section_data();
  
  // Return the .gnu_incremental_strtab stringpool. 
  Stringpool*
  get_stringpool()
  { return this->strtab_; }

 private:
  // Code for each of the four possible variants of create_inputs_section_data.
  template<int size, bool big_endian>
  Output_section_data*
  sized_create_inputs_section_data();  

  // The key of the command line string in the string pool.
  Stringpool::Key command_line_key_;
  // The .gnu_incremental_strtab string pool associated with the
  // .gnu_incremental_inputs.
  Stringpool* strtab_;
};

} // End namespace gold.

#endif // !defined(GOLD_INCREMENTAL_H)
