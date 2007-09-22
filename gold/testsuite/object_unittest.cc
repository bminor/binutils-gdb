// object_unittest.cc -- test Object, Relobj, etc.

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

#include "object.h"

#include "test.h"
#include "testfile.h"

namespace gold_testsuite
{

using namespace gold;

// Test basic Object functionality.

bool
Object_test(Test_report*)
{
  Input_file input_file("test.o", test_file_1, test_file_1_size);
  Object* object = make_elf_object("test.o", &input_file, 0,
				   test_file_1, test_file_1_size);
  CHECK(object->name() == "test.o");
  CHECK(!object->is_dynamic());
  CHECK(object->target() == target_test_pointer);
  CHECK(object->is_locked());
  object->unlock();
  CHECK(!object->is_locked());
  object->lock();
  CHECK(object->shnum() == 5);
  CHECK(object->section_name(0).empty());
  CHECK(object->section_name(1) == ".test");
  CHECK(object->section_flags(0) == 0);
  CHECK(object->section_flags(1) == elfcpp::SHF_ALLOC);
  object->unlock();
  return true;
}

Register_test object_register("Object", Object_test);

} // End namespace gold_testsuite.
