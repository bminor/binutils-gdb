// testfile.h -- test input files   -*- C++ -*-

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

#ifndef GOLD_TESTSUITE_TESTFILE_H
#define GOLD_TESTSUITE_TESTFILE_H

namespace gold
{
class Target;
}

namespace gold_testsuite
{

extern gold::Target* target_test_pointer;
extern const unsigned char test_file_1[];
extern const unsigned int test_file_1_size;

}; // End namespace gold_testsuite.

#endif // !defined(GOLD_TESTSUITE_TESTFILE_H)
