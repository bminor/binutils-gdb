// parameters.h -- general parameters for a link using gold  -*- C++ -*-

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

#ifndef GOLD_PARAMETERS_H
#define GOLD_PARAMETERS_H

namespace gold
{

class General_options;

// Here we define the Parameters class which simply holds simple
// general parameters which apply to the entire link.  We use a global
// variable for this.  This is in contrast to the General_options
// class, which holds the complete state of position independent
// command line options.  The hope is that Parameters will stay fairly
// simple, so that if this turns into a library it will be clear how
// these parameters should be set.

class Parameters
{
 public:
  Parameters(const General_options*);

  // Whether we are generating a regular executable.
  bool
  output_is_executable() const
  { return this->output_file_type_ == OUTPUT_EXECUTABLE; }

  // Whether we are generating a shared library.
  bool
  output_is_shared() const
  { return this->output_file_type_ == OUTPUT_SHARED; }

  // Whether we are generating an object file.
  bool
  output_is_object() const
  { return this->output_file_type_ == OUTPUT_OBJECT; }

  // The general linker optimization level.
  int
  optimization_level() const
  { return this->optimization_level_; }

 private:
  // The types of output files.
  enum Output_file_type
    {
      // Generating executable.
      OUTPUT_EXECUTABLE,
      // Generating shared library.
      OUTPUT_SHARED,
      // Generating object file.
      OUTPUT_OBJECT
    };

  // The type of the output file.
  Output_file_type output_file_type_;
  // The optimization level.
  int optimization_level_;
};

// This is a global variable.
extern const Parameters* parameters;

// Initialize the global variable.
extern void initialize_parameters(const General_options*);

} // End namespace gold.

#endif // !defined(GOLD_PARAMATERS_H)
