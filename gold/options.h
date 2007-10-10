// options.h -- handle command line options for gold  -*- C++ -*-

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

// Command_line
//   Holds everything we get from the command line.
// General_options (from Command_line::options())
//   Options which are not position dependent.
// Input_argument (from Command_line::inputs())
//   The list of input files, including -l options.
// Position_dependent_options (from Input_argument::options())
//   Position dependent options which apply to this argument.

#ifndef GOLD_OPTIONS_H
#define GOLD_OPTIONS_H

#include <cstdlib>
#include <list>
#include <string>
#include <vector>

namespace gold
{

class Command_line;
class Input_file_group;

namespace options {

class Command_line_options;
struct One_option;

} // End namespace gold::options.

// A directory to search.  For each directory we record whether it is
// in the sysroot.  We need to know this so that, if a linker script
// is found within the sysroot, we will apply the sysroot to any files
// named by that script.

class Search_directory
{
 public:
  // We need a default constructor because we put this in a
  // std::vector.
  Search_directory()
    : name_(NULL), put_in_sysroot_(false), is_in_sysroot_(false)
  { }

  // This is the usual constructor.
  Search_directory(const char* name, bool put_in_sysroot)
    : name_(name), put_in_sysroot_(put_in_sysroot), is_in_sysroot_(false)
  { gold_assert(!this->name_.empty()); }

  // This is called if we have a sysroot.  The sysroot is prefixed to
  // any entries for which put_in_sysroot_ is true.  is_in_sysroot_ is
  // set to true for any enries which are in the sysroot (this will
  // naturally include any entries for which put_in_sysroot_ is true).
  // SYSROOT is the sysroot, CANONICAL_SYSROOT is the result of
  // passing SYSROOT to lrealpath.
  void
  add_sysroot(const char* sysroot, const char* canonical_sysroot);

  // Get the directory name.
  const std::string&
  name() const
  { return this->name_; }

  // Return whether this directory is in the sysroot.
  bool
  is_in_sysroot() const
  { return this->is_in_sysroot_; }

 private:
  std::string name_;
  bool put_in_sysroot_;
  bool is_in_sysroot_;
};

// The position independent options which apply to the whole link.
// There are a lot of them.

class General_options
{
 public:
  General_options();

  // -E: export dynamic symbols.
  bool
  export_dynamic() const
  { return this->export_dynamic_; }

  // -I: dynamic linker name.
  const char*
  dynamic_linker() const
  { return this->dynamic_linker_; }

  // -L: Library search path.
  typedef std::vector<Search_directory> Dir_list;

  const Dir_list&
  search_path() const
  { return this->search_path_; }

  // -O: optimization level (0: don't try to optimize output size).
  int
  optimization_level() const
  { return this->optimization_level_; }

  // -o: Output file name.
  const char*
  output_file_name() const
  { return this->output_file_name_; }

  // -r: Whether we are doing a relocatable link.
  bool
  is_relocatable() const
  { return this->is_relocatable_; }

  // -s: Strip all symbols.
  bool
  strip_all() const
  { return this->strip_ == STRIP_ALL; }

  // -S: Strip debugging information.
  bool
  strip_debug() const
  { return this->strip_ == STRIP_ALL || this->strip_ == STRIP_DEBUG; }

  // --eh-frame-hdr: Whether to generate an exception frame header.
  bool
  create_eh_frame_hdr() const
  { return this->create_eh_frame_hdr_; }

  // --rpath: The runtime search path.
  const Dir_list&
  rpath() const
  { return this->rpath_; }

  // --rpath-link: The link time search patch for shared libraries.
  const Dir_list&
  rpath_link() const
  { return this->rpath_link_; }

  // --shared: Whether generating a shared object.
  bool
  is_shared() const
  { return this->is_shared_; }

  // --static: Whether doing a static link.
  bool
  is_static() const
  { return this->is_static_; }

  // --sysroot: The system root of a cross-linker.
  const std::string&
  sysroot() const
  { return this->sysroot_; }

 private:
  // Don't copy this structure.
  General_options(const General_options&);
  General_options& operator=(const General_options&);

  friend class Command_line;
  friend class options::Command_line_options;

  // Which symbols to strip.
  enum Strip
  {
    // Don't strip any symbols.
    STRIP_NONE,
    // Strip all symbols.
    STRIP_ALL,
    // Strip debugging information.
    STRIP_DEBUG
  };

  void
  set_export_dynamic()
  { this->export_dynamic_ = true; }

  void
  set_dynamic_linker(const char* arg)
  { this->dynamic_linker_ = arg; }

  void
  add_to_search_path(const char* arg)
  { this->search_path_.push_back(Search_directory(arg, false)); }

  void
  add_to_search_path_with_sysroot(const char* arg)
  { this->search_path_.push_back(Search_directory(arg, true)); }

  void
  set_optimization_level(const char* arg)
  { this->optimization_level_ = atoi(arg); }

  void
  set_output_file_name(const char* arg)
  { this->output_file_name_ = arg; }

  void
  set_relocatable()
  { this->is_relocatable_ = true; }

  void
  set_strip_all()
  { this->strip_ = STRIP_ALL; }

  // Note: normalize_options() depends on the fact that this turns off
  // STRIP_ALL if it were already set.
  void
  set_strip_debug()
  { this->strip_ = STRIP_DEBUG; }

  void
  set_create_eh_frame_hdr()
  { this->create_eh_frame_hdr_ = true; }

  void
  add_to_rpath(const char* arg)
  { this->rpath_.push_back(Search_directory(arg, false)); }

  void
  add_to_rpath_link(const char* arg)
  { this->rpath_link_.push_back(Search_directory(arg, false)); }

  void
  set_shared()
  { this->is_shared_ = true; }

  void
  set_static()
  { this->is_static_ = true; }

  void
  set_sysroot(const char* arg)
  { this->sysroot_ = arg; }

  void
  ignore(const char*)
  { }

  // Apply any sysroot to the directory lists.
  void
  add_sysroot();

  bool export_dynamic_;
  const char* dynamic_linker_;
  Dir_list search_path_;
  int optimization_level_;
  const char* output_file_name_;
  bool is_relocatable_;
  Strip strip_;
  bool create_eh_frame_hdr_;
  Dir_list rpath_;
  Dir_list rpath_link_;
  bool is_shared_;
  bool is_static_;
  std::string sysroot_;
};

// The current state of the position dependent options.

class Position_dependent_options
{
 public:
  Position_dependent_options();

  // -Bstatic: Whether we are searching for a static archive rather
  // than a shared object.
  bool
  do_static_search() const
  { return this->do_static_search_; }

  // --as-needed: Whether to add a DT_NEEDED argument only if the
  // dynamic object is used.
  bool
  as_needed() const
  { return this->as_needed_; }

  // --whole-archive: Whether to include the entire contents of an
  // --archive.
  bool
  include_whole_archive() const
  { return this->include_whole_archive_; }

  void
  set_static_search()
  { this->do_static_search_ = true; }

  void
  set_dynamic_search()
  { this->do_static_search_ = false; }

  void
  set_as_needed()
  { this->as_needed_ = true; }

  void
  clear_as_needed()
  { this->as_needed_ = false; }

  void
  set_whole_archive()
  { this->include_whole_archive_ = true; }

  void
  clear_whole_archive()
  { this->include_whole_archive_ = false; }

 private:
  bool do_static_search_;
  bool as_needed_;
  bool include_whole_archive_;
};

// A single file or library argument from the command line.

class Input_file_argument
{
 public:
  // name: file name or library name
  // is_lib: true if name is a library name: that is, emits the leading
  //         "lib" and trailing ".so"/".a" from the name
  // extra_search_path: an extra directory to look for the file, prior
  //         to checking the normal library search path.  If this is "",
  //         then no extra directory is added.
  // options: The position dependent options at this point in the
  //         command line, such as --whole-archive.
  Input_file_argument()
    : name_(), is_lib_(false), extra_search_path_(""), options_()
  { }

  Input_file_argument(const char* name, bool is_lib,
                      const char* extra_search_path,
		      const Position_dependent_options& options)
    : name_(name), is_lib_(is_lib), extra_search_path_(extra_search_path),
      options_(options)
  { }

  const char*
  name() const
  { return this->name_.c_str(); }

  const Position_dependent_options&
  options() const
  { return this->options_; }

  bool
  is_lib() const
  { return this->is_lib_; }

  const char*
  extra_search_path() const
  {
    return (this->extra_search_path_.empty()
            ? NULL
	    : this->extra_search_path_.c_str());
  }

  // Return whether this file may require a search using the -L
  // options.
  bool
  may_need_search() const
  { return this->is_lib_ || !this->extra_search_path_.empty(); }

 private:
  // We use std::string, not const char*, here for convenience when
  // using script files, so that we do not have to preserve the string
  // in that case.
  std::string name_;
  bool is_lib_;
  std::string extra_search_path_;
  Position_dependent_options options_;
};

// A file or library, or a group, from the command line.

class Input_argument
{
 public:
  // Create a file or library argument.
  explicit Input_argument(Input_file_argument file)
    : is_file_(true), file_(file), group_(NULL)
  { }

  // Create a group argument.
  explicit Input_argument(Input_file_group* group)
    : is_file_(false), group_(group)
  { }

  // Return whether this is a file.
  bool
  is_file() const
  { return this->is_file_; }

  // Return whether this is a group.
  bool
  is_group() const
  { return !this->is_file_; }

  // Return the information about the file.
  const Input_file_argument&
  file() const
  {
    gold_assert(this->is_file_);
    return this->file_;
  }

  // Return the information about the group.
  const Input_file_group*
  group() const
  {
    gold_assert(!this->is_file_);
    return this->group_;
  }

  Input_file_group*
  group()
  {
    gold_assert(!this->is_file_);
    return this->group_;
  }

 private:
  bool is_file_;
  Input_file_argument file_;
  Input_file_group* group_;
};

// A group from the command line.  This is a set of arguments within
// --start-group ... --end-group.

class Input_file_group
{
 public:
  typedef std::vector<Input_argument> Files;
  typedef Files::const_iterator const_iterator;

  Input_file_group()
    : files_()
  { }

  // Add a file to the end of the group.
  void
  add_file(const Input_file_argument& arg)
  { this->files_.push_back(Input_argument(arg)); }

  // Iterators to iterate over the group contents.

  const_iterator
  begin() const
  { return this->files_.begin(); }

  const_iterator
  end() const
  { return this->files_.end(); }

 private:
  Files files_;
};

// A list of files from the command line or a script.

class Input_arguments
{
 public:
  typedef std::vector<Input_argument> Input_argument_list;
  typedef Input_argument_list::const_iterator const_iterator;

  Input_arguments()
    : input_argument_list_(), in_group_(false)
  { }

  // Add a file.
  void
  add_file(const Input_file_argument& arg);

  // Start a group (the --start-group option).
  void
  start_group();

  // End a group (the --end-group option).
  void
  end_group();

  // Return whether we are currently in a group.
  bool
  in_group() const
  { return this->in_group_; }

  // Iterators to iterate over the list of input files.

  const_iterator
  begin() const
  { return this->input_argument_list_.begin(); }

  const_iterator
  end() const
  { return this->input_argument_list_.end(); }

  // Return whether the list is empty.
  bool
  empty() const
  { return this->input_argument_list_.empty(); }

 private:
  Input_argument_list input_argument_list_;
  bool in_group_;
};

// All the information read from the command line.

class Command_line
{
 public:
  typedef Input_arguments::const_iterator const_iterator;

  Command_line();

  // Process the command line options.  This will exit with an
  // appropriate error message if an unrecognized option is seen.
  void
  process(int argc, char** argv);

  // Handle a -l option.
  int
  process_l_option(int, char**, char*);

  // Handle a --start-group option.
  void
  start_group(const char* arg);

  // Handle a --end-group option.
  void
  end_group(const char* arg);

  // Get the general options.
  const General_options&
  options() const
  { return this->options_; }

  // Iterators to iterate over the list of input files.

  const_iterator
  begin() const
  { return this->inputs_.begin(); }

  const_iterator
  end() const
  { return this->inputs_.end(); }

 private:
  Command_line(const Command_line&);
  Command_line& operator=(const Command_line&);

  // Report usage error.
  void
  usage() ATTRIBUTE_NORETURN;
  void
  usage(const char* msg, const char* opt) ATTRIBUTE_NORETURN;
  void
  usage(const char* msg, char opt) ATTRIBUTE_NORETURN;

  // Apply a command line option.
  void
  apply_option(const gold::options::One_option&, const char*);

  // Add a file.
  void
  add_file(const char* name, bool is_lib);

  // Examine the result of processing the command-line, and verify
  // the flags do not contradict each other or are otherwise illegal.
  void
  normalize_options();

  General_options options_;
  Position_dependent_options position_options_;
  Input_arguments inputs_;
};

} // End namespace gold.

#endif // !defined(GOLD_OPTIONS_H)
