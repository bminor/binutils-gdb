// inremental.cc -- incremental linking test/deubg tool

// Copyright 2009 Free Software Foundation, Inc.
// Written by Rafael Avila de Espindola <rafael.espindola@gmail.com>

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


// This file is a (still incomplete) test/debug tool that should display
// all information available in the incremental linking sections in a
// format that is easy to read.
// Once the format is a bit more stable, this should probably be moved to
// readelf. Because of that, the use of gold's data structures and functions
// is just a short term convenience and not a design decision.

#include "gold.h"

#include <stdio.h>
#include <errno.h>

#include "incremental.h"

namespace gold
{
  class Output_file;
}

using namespace gold;

int
main(int argc, char** argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "Usage: %s <file>\n", argv[0]);
      return 1;
    }
  const char* filename = argv[1];

  Output_file* file = new Output_file(filename);

  bool t = file->open_for_modification();
  if (!t)
    {
      fprintf(stderr, "%s: open_for_modification(%s): %s\n", argv[0], filename,
              strerror(errno));
      return 1;
    }

  Incremental_binary* inc = open_incremental_binary(file);

  if (inc == NULL)
    {
      fprintf(stderr, "%s: open_incremental_binary(%s): %s\n", argv[0],
              filename, strerror(errno));
      return 1;
    }

  unsigned int strtab_shndx;
  Incremental_binary::Location location;

  t = inc->find_incremental_inputs_section(&location, &strtab_shndx);
  if (!t)
    {
      fprintf(stderr, "%s: %s: no .gnu_incremental_inputs section\n", argv[0],
              filename);
      return 1;
    }

  Incremental_binary::View inputs_view(inc->view(location));
  const unsigned char *p = inputs_view.data();

  const Incremental_inputs_header_data* incremental_header =
    reinterpret_cast<const Incremental_inputs_header_data*> (p);

  const Incremental_inputs_entry_data* incremental_inputs =
    reinterpret_cast<const Incremental_inputs_entry_data*>
    (p + sizeof(Incremental_inputs_header_data));

  if (incremental_header->version != 1)
    {
      fprintf(stderr, "%s: %s: unknown incremestal version %d\n", argv[0],
              filename, incremental_header->version);
      return 1;
    }

  elfcpp::Elf_file<64, false, Incremental_binary> elf_file(inc);

  if (elf_file.section_type(strtab_shndx) != elfcpp::SHT_STRTAB)
    {
      fprintf(stderr,
              "%s: %s: invalid string table section %u (type %d != %d)\n",
              argv[0], filename, strtab_shndx,
              elf_file.section_type(strtab_shndx), elfcpp::SHT_STRTAB);
      return 1;
    }

  Incremental_binary::Location
    strtab_location(elf_file.section_contents(strtab_shndx));

  Incremental_binary::View strtab_view(inc->view(strtab_location));
  p = strtab_view.data();

  elfcpp::Elf_strtab strtab(strtab_view.data(), strtab_location.data_size);
  const char* command_line;
  t = strtab.get_c_string(incremental_header->command_line_offset,
                          &command_line);

  if (!t)
    {
      fprintf(stderr,
              "%s: %s: failed to get link command line: %zu out of range\n",
              argv[0], filename,
              static_cast<size_t>(incremental_header->command_line_offset));
      return 1;
    }

  printf("Link command line: %s\n", command_line);

  printf("Input files:\n");
  for (unsigned i = 0; i < incremental_header->input_file_count; ++i)
    {
      const Incremental_inputs_entry_data* input =
        &incremental_inputs[i];
      const char *objname;

      t = strtab.get_c_string(input->filename_offset, &objname);
      if (!t)
        {
          fprintf(stderr,"%s: %s: failed to get file name for object %u:"
                  " %zu out of range\n", argv[0], filename, i,
                  static_cast<size_t>(input->filename_offset));
          return 1;
        }
      printf("  %s\n", objname);
      printf("    Timestamp sec = %llu\n",
             static_cast<unsigned long long>(input->timestamp_sec));
      printf("    Timestamp nsec = %d\n", input->timestamp_nsec);
      printf("    Type = ");
      // TODO: print the data at input->data_offset once we have it.
      switch (input->input_type)
      {
      case INCREMENTAL_INPUT_OBJECT:
        printf("Abject\n");
        break;
      case INCREMENTAL_INPUT_ARCHIVE:
        printf("Archive\n");
        break;
      case INCREMENTAL_INPUT_SHARED_LIBRARY:
        printf("Shared library\n");
        break;
      case INCREMENTAL_INPUT_SCRIPT:
        printf("Linker script\n");
        break;
      case INCREMENTAL_INPUT_INVALID:
      default:
        fprintf(stderr, "%s: invalid file type for object %u: %d\n",
                argv[0], i, input->input_type);
        return 1;
      }
    }

  return 0;
}
