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

template<int size, bool big_endian>
static void
dump_incremental_inputs(const char* argv0,
                        const char* filename, Incremental_binary* inc)
{
  bool t;
  unsigned int strtab_shndx;
  Incremental_binary::Location location;

  t = inc->find_incremental_inputs_section(&location, &strtab_shndx);
  if (!t)
    {
      fprintf(stderr, "%s: %s: no .gnu_incremental_inputs section\n", argv0,
              filename);
      exit (1);
    }

  Incremental_binary::View inputs_view(inc->view(location));
  const unsigned char* p = inputs_view.data();

  Incremental_inputs_header<size, big_endian> incremental_header(p);

  const unsigned char* incremental_inputs_base =
    (p + sizeof(Incremental_inputs_header_data));

  if (incremental_header.get_version() != 1)
    {
      fprintf(stderr, "%s: %s: unknown incremental version %d\n", argv0,
              filename, incremental_header.get_version());
      exit(1);
    }

  elfcpp::Elf_file<size, big_endian, Incremental_binary> elf_file(inc);

  if (elf_file.section_type(strtab_shndx) != elfcpp::SHT_STRTAB)
    {
      fprintf(stderr,
              "%s: %s: invalid string table section %u (type %d != %d)\n",
              argv0, filename, strtab_shndx,
              elf_file.section_type(strtab_shndx), elfcpp::SHT_STRTAB);
      exit(1);
    }

  Incremental_binary::Location
    strtab_location(elf_file.section_contents(strtab_shndx));

  Incremental_binary::View strtab_view(inc->view(strtab_location));
  p = strtab_view.data();

  elfcpp::Elf_strtab strtab(strtab_view.data(), strtab_location.data_size);
  const char* command_line;
  elfcpp::Elf_Word command_line_offset =
    incremental_header.get_command_line_offset();
  t = strtab.get_c_string(command_line_offset, &command_line);

  if (!t)
    {
      fprintf(stderr,
              "%s: %s: failed to get link command line: %zu out of range\n",
              argv0, filename,
              static_cast<size_t>(command_line_offset));
      exit(1);
    }

  printf("Link command line: %s\n", command_line);

  printf("Input files:\n");
  for (unsigned i = 0; i < incremental_header.get_input_file_count(); ++i)
    {
      const unsigned char* input_p = incremental_inputs_base +
        i * sizeof(Incremental_inputs_entry_data);
      Incremental_inputs_entry<size, big_endian> input(input_p);
      const char* objname;

      t = strtab.get_c_string(input.get_filename_offset(), &objname);
      if (!t)
        {
          fprintf(stderr,"%s: %s: failed to get file name for object %u:"
                  " %zu out of range\n", argv0, filename, i,
                  static_cast<size_t>(input.get_filename_offset()));
          exit(1);
        }
      printf("  %s\n", objname);
      printf("    Timestamp sec = %llu\n",
             static_cast<unsigned long long>(input.get_timestamp_sec()));
      printf("    Timestamp nsec = %d\n", input.get_timestamp_nsec());
      printf("    Type = ");
      // TODO: print the data at input->data_offset once we have it.
      elfcpp::Elf_Word input_type = input.get_input_type();
      switch (input_type)
      {
      case INCREMENTAL_INPUT_OBJECT:
        printf("Object\n");
        break;
      case INCREMENTAL_INPUT_ARCHIVE:
        printf("Archive\n");
        break;
      case INCREMENTAL_INPUT_SHARED_LIBRARY:
        printf("Shared library\n");
        break;
      case INCREMENTAL_INPUT_SCRIPT:
        printf("Linker script\n");
        if (input.get_data_offset() != 0)
          {
            fprintf(stderr,"%s: %s: %u is a script but offset is not zero",
                    argv0, filename, i);
            exit(1);
          }
        break;
      case INCREMENTAL_INPUT_INVALID:
      default:
        fprintf(stderr, "%s: invalid file type for object %u: %d\n",
                argv0, i, input_type);
        exit(1);
      }
    }
}

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

  switch (parameters->size_and_endianness())
    {
#ifdef HAVE_TARGET_32_LITTLE
    case Parameters::TARGET_32_LITTLE:
      dump_incremental_inputs<32, false>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_32_BIG
    case Parameters::TARGET_32_BIG:
      dump_incremental_inputs<32, true>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_64_LITTLE
    case Parameters::TARGET_64_LITTLE:
      dump_incremental_inputs<64, false>(argv[0], filename, inc);
      break;
#endif
#ifdef HAVE_TARGET_64_BIG
    case Parameters::TARGET_64_BIG:
      dump_incremental_inputs<64, true>(argv[0], filename, inc);
      break;
#endif
    default:
      gold_unreachable();
    }

  return 0;
}
