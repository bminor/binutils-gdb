// archive.h -- archive support for gold      -*- C++ -*-

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

#ifndef GOLD_ARCHIVE_H
#define GOLD_ARCHIVE_H

#include <string>
#include <vector>

#include "workqueue.h"

namespace gold
{

class Task;
class Input_file;
class Input_objects;
class Input_group;
class Layout;
class Symbol_table;

// This class represents an archive--generally a libNAME.a file.
// Archives have a symbol table and a list of objects.

class Archive
{
 public:
  Archive(const std::string& name, Input_file* input_file)
    : name_(name), input_file_(input_file), armap_(), armap_names_(),
      extended_names_(), armap_checked_(), seen_offsets_()
  { }

  // The length of the magic string at the start of an archive.
  static const int sarmag = 8;

  // The magic string at the start of an archive.
  static const char armag[sarmag];

  // The string expected at the end of an archive member header.
  static const char arfmag[2];

  // The name of the object.
  const std::string&
  name() const
  { return this->name_; }

  // Set up the archive: read the symbol map.
  void
  setup(Task*);

  // Get a reference to the underlying file.
  File_read&
  file()
  { return this->input_file_->file(); }

  // Lock the underlying file.
  void
  lock(const Task* t)
  { this->input_file_->file().lock(t); }

  // Unlock the underlying file.
  void
  unlock(const Task* t)
  { this->input_file_->file().unlock(t); }

  // Return whether the underlying file is locked.
  bool
  is_locked() const
  { return this->input_file_->file().is_locked(); }

  // Return the token, so that the task can be queued.
  Task_token*
  token()
  { return this->input_file_->file().token(); }

  // Release the underlying file.
  void
  release()
  { this->input_file_->file().release(); }

  // Select members from the archive as needed and add them to the
  // link.
  void
  add_symbols(Symbol_table*, Layout*, Input_objects*);

 private:
  Archive(const Archive&);
  Archive& operator=(const Archive&);

  struct Archive_header;

  // Get a view into the underlying file.
  const unsigned char*
  get_view(off_t start, section_size_type size, bool cache)
  { return this->input_file_->file().get_view(start, size, cache); }

  // Read the archive symbol map.
  void
  read_armap(off_t start, section_size_type size);

  // Read an archive member header at OFF.  CACHE is whether to cache
  // the file view.  Return the size of the member, and set *PNAME to
  // the name.
  off_t
  read_header(off_t off, bool cache, std::string* pname);

  // Interpret an archive header HDR at OFF.  Return the size of the
  // member, and set *PNAME to the name.
  off_t
  interpret_header(const Archive_header* hdr, off_t off, std::string* pname);

  // Include all the archive members in the link.
  void
  include_all_members(Symbol_table*, Layout*, Input_objects*);

  // Include an archive member in the link.
  void
  include_member(Symbol_table*, Layout*, Input_objects*, off_t off);

  // An entry in the archive map of symbols to object files.
  struct Armap_entry
  {
    // The offset to the symbol name in armap_names_.
    off_t name_offset;
    // The file offset to the object in the archive.
    off_t file_offset;
  };

  // A simple hash code for off_t values.
  class Seen_hash
  {
   public:
    size_t operator()(off_t val) const
    { return static_cast<size_t>(val); }
  };

  // Name of object as printed to user.
  std::string name_;
  // For reading the file.
  Input_file* input_file_;
  // The archive map.
  std::vector<Armap_entry> armap_;
  // The names in the archive map.
  std::string armap_names_;
  // The extended name table.
  std::string extended_names_;
  // Track which symbols in the archive map are for elements which are
  // defined or which have already been included in the link.
  std::vector<bool> armap_checked_;
  // Track which elements have been included by offset.
  Unordered_set<off_t, Seen_hash> seen_offsets_;
};

// This class is used to read an archive and pick out the desired
// elements and add them to the link.

class Add_archive_symbols : public Task
{
 public:
  Add_archive_symbols(Symbol_table* symtab, Layout* layout,
		      Input_objects* input_objects,
		      Archive* archive, Input_group* input_group,
		      Task_token* this_blocker,
		      Task_token* next_blocker)
    : symtab_(symtab), layout_(layout), input_objects_(input_objects),
      archive_(archive), input_group_(input_group),
      this_blocker_(this_blocker), next_blocker_(next_blocker)
  { }

  ~Add_archive_symbols();

  // The standard Task methods.

  Task_token*
  is_runnable();

  void
  locks(Task_locker*);

  void
  run(Workqueue*);

  std::string
  get_name() const
  {
    if (this->archive_ == NULL)
      return "Add_archive_symbols";
    return "Add_archive_symbols " + this->archive_->file().filename();
  }

 private:
  Symbol_table* symtab_;
  Layout* layout_;
  Input_objects* input_objects_;
  Archive* archive_;
  Input_group* input_group_;
  Task_token* this_blocker_;
  Task_token* next_blocker_;
};

} // End namespace gold.

#endif // !defined(GOLD_ARCHIVE_H)
