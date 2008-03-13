// target-select.h -- select a target for an object file  -*- C++ -*-

// Copyright 2006, 2007, 2008 Free Software Foundation, Inc.
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

#ifndef GOLD_TARGET_SELECT_H
#define GOLD_TARGET_SELECT_H

namespace gold
{

class Target;

// We want to avoid a master list of targets, which implies using a
// global constructor.  And we also want the program to start up as
// quickly as possible, which implies avoiding global constructors.
// We compromise on a very simple global constructor.  We use a target
// selector, which specifies an ELF machine number and a recognition
// function.  We use global constructors to build a linked list of
// target selectors--a simple pointer list, not a std::list.

class Target_selector
{
 public:
  // Create a target selector for a specific machine number, size (32
  // or 64), and endianness.  The machine number can be EM_NONE to
  // test for any machine number.
  Target_selector(int machine, int size, bool is_big_endian);

  virtual ~Target_selector()
  { }

  // If we can handle this target, return a pointer to a target
  // structure.  The size and endianness are known.
  virtual Target*
  recognize(int machine, int osabi, int abiversion) = 0;

  // If NAME matches the target, return a pointer to a target
  // structure.
  virtual Target*
  recognize_by_name(const char* name) = 0;

  // Return the next Target_selector in the linked list.
  Target_selector*
  next() const
  { return this->next_; }

  // Return the machine number this selector is looking for, which can
  // be EM_NONE to match any machine number.
  int
  machine() const
  { return this->machine_; }

  // Return the size this is looking for (32 or 64).
  int
  get_size() const
  { return this->size_; }

  // Return the endianness this is looking for.
  bool
  is_big_endian() const
  { return this->is_big_endian_; }

 private:
  int machine_;
  int size_;
  bool is_big_endian_;
  Target_selector* next_;
};

// Select the target for an ELF file.

extern Target* select_target(int machine, int size, bool big_endian,
			     int osabi, int abiversion);

// Select a target using a BFD name.

extern Target* select_target_by_name(const char* name);

} // End namespace gold.

#endif // !defined(GOLD_TARGET_SELECT_H)
