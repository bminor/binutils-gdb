// parameters.cc -- general parameters for a link using gold

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

#include "gold.h"

#include "options.h"
#include "target.h"
#include "parameters.h"

namespace gold
{

// Initialize the parameters from the options.

Parameters::Parameters(Errors* errors)
  : errors_(errors), threads_(false), output_file_name_(NULL),
    output_file_type_(OUTPUT_INVALID), sysroot_(),
    strip_(STRIP_INVALID), allow_shlib_undefined_(false),
    symbolic_(false), demangle_(false), detect_odr_violations_(false),
    optimization_level_(0), export_dynamic_(false), debug_(0),
    is_doing_static_link_valid_(false), doing_static_link_(false),
    is_target_valid_(false), target_(NULL)
{
}

// Set fields from the command line options.

void
Parameters::set_from_options(const General_options* options)
{
  this->threads_ = options->threads();
  this->output_file_name_ = options->output_file_name();
  this->sysroot_ = options->sysroot();
  this->allow_shlib_undefined_ = options->allow_shlib_undefined();
  this->symbolic_ = options->symbolic();
  this->demangle_ = options->demangle();
  this->detect_odr_violations_ = options->detect_odr_violations();
  this->optimization_level_ = options->optimization_level();
  this->export_dynamic_ = options->export_dynamic();
  this->debug_ = options->debug();

  if (options->is_shared())
    this->output_file_type_ = OUTPUT_SHARED;
  else if (options->is_relocatable())
    this->output_file_type_ = OUTPUT_OBJECT;
  else
    this->output_file_type_ = OUTPUT_EXECUTABLE;

  if (options->strip_all())
    this->strip_ = STRIP_ALL;
  else if (options->strip_debug())
    this->strip_ = STRIP_DEBUG;
  else if (options->strip_debug_gdb())
    this->strip_ = STRIP_DEBUG_UNUSED_BY_GDB;
  else
    this->strip_ = STRIP_NONE;

  this->options_valid_ = true;
}

// Set whether we are doing a static link.

void
Parameters::set_doing_static_link(bool doing_static_link)
{
  this->doing_static_link_ = doing_static_link;
  this->is_doing_static_link_valid_ = true;
}

// Set the target.

void
Parameters::set_target(Target* target)
{
  if (!this->is_target_valid_)
    {
      this->target_ = target;
      this->size_ = target->get_size();
      this->is_big_endian_ = target->is_big_endian();
      this->is_target_valid_ = true;
    }
  else
    gold_assert(target == this->target_);
}

// Our local version of the variable, which is not const.

static Parameters* static_parameters;

// The global variable.

const Parameters* parameters;

// Initialize the global variable.

void
initialize_parameters(Errors* errors)
{
  parameters = static_parameters = new Parameters(errors);
}

// Set values from the options.

void
set_parameters_from_options(const General_options* options)
{
  static_parameters->set_from_options(options);
}

// Set whether we are doing a static link.

void
set_parameters_doing_static_link(bool doing_static_link)
{
  static_parameters->set_doing_static_link(doing_static_link);
}

// Set the target.

void
set_parameters_target(Target* target)
{
  static_parameters->set_target(target);
}

} // End namespace gold.
