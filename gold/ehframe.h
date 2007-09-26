// ehframe.h -- handle exception frame sections for gold  -*- C++ -*-

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

#ifndef GOLD_EHFRAME_H
#define GOLD_EHFRAME_H

#include "output.h"

namespace gold
{

// This class manages the .eh_frame_hdr section, which holds the data
// for the PT_GNU_EH_FRAME segment.  gcc's unwind support code uses
// the PT_GNU_EH_FRAME segment to find the list of FDEs.  This saves
// the time required to register the exception handlers at startup
// time and when a shared object is loaded, and the time required to
// deregister the exception handlers when a shared object is unloaded.

// FIXME: gcc supports using storing a sorted lookup table for the
// FDEs in the PT_GNU_EH_FRAME segment, but we do not yet generate
// that.

class Eh_frame_hdr : public Output_section_data
{
 public:
  Eh_frame_hdr(Output_section* eh_frame_section);

  // Set the final data size.
  void
  do_set_address(uint64_t address, off_t offset);

  // Write the data to the file.
  void
  do_write(Output_file*);

 private:
  // The .eh_frame section.
  Output_section* eh_frame_section_;
};

} // End namespace gold.

#endif // !defined(GOLD_EHFRAME_H)
