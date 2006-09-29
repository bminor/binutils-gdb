// options.h -- handle command line options for gold  -*- C++ -*-

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

#include <list>
#include <string>

namespace gold
{

class Command_line;

namespace options {

class Command_line_options;
struct One_option;

} // End namespace gold::options.

// The position independent options which apply to the whole link.
// There are a lot of them.

class General_options
{
 public:
  General_options();

  // -L: Library search path.
  typedef std::list<const char*> Dir_list;

  const Dir_list&
  search_path() const
  { return this->search_path_; }

  // -o: Output file name.
  const char*
  output_file_name() const
  { return this->output_file_name_; }

  // -r: Whether we are doing a relocatable link.
  bool
  is_relocatable() const
  { return this->is_relocatable_; }

  // --static: Whether doing a static link.
  bool
  is_static() const
  { return this->is_static_; }

 private:
  friend class Command_line;
  friend class options::Command_line_options;

  void
  add_to_search_path(const char* arg)
  { this->search_path_.push_back(arg); }

  void
  set_output_file_name(const char* arg)
  { this->output_file_name_ = arg; }

  void
  set_relocatable()
  { this->is_relocatable_ = true; }

  void
  set_static()
  { this->is_static_ = true; }

  Dir_list search_path_;
  const char* output_file_name_;
  bool is_relocatable_;
  bool is_static_;

  // Don't copy this structure.
  General_options(const General_options&);
  General_options& operator=(const General_options&);
};

// The current state of the position dependent options.

class Position_dependent_options
{
 public:
  Position_dependent_options();

  // -Bstatic: Whether we are searching for a static archive rather
  // -than a shared object.
  bool
  do_static_search()
  { return this->do_static_search_; }

 private:
  friend class Command_line;
  friend class options::Command_line_options;

  void
  set_static_search()
  { this->do_static_search_ = true; }

  void
  set_dynamic_search()
  { this->do_static_search_ = false; }

  bool do_static_search_;
};

// A single file or library argument from the command line.

class Input_argument
{
 public:
  Input_argument(const char* name, bool is_lib,
		 const Position_dependent_options& options)
    : name_(name), is_lib_(is_lib), options_(options)
  { }

  const char*
  name() const
  { return this->name_; }

  const Position_dependent_options&
  options() const
  { return this->options_; }

  bool
  is_lib() const
  { return this->is_lib_; }

 private:
  const char* name_;
  bool is_lib_;
  Position_dependent_options options_;
};

// All the information read from the command line.

class Command_line
{
 public:
  Command_line();

  // Process the command line options.  This will exit with an
  // appropriate error message if an unrecognized option is seen.
  void
  process(int argc, char** argv);

  // Handle a -l option.
  int
  process_l_option(int, char**, char*);

  // Get the general options.
  const General_options&
  options() const
  { return this->options_; }

  typedef std::list<Input_argument> Input_argument_list;

  // Get the list of input files.
  const Input_argument_list&
  inputs() const
  { return this->inputs_; }

 private:
  void usage() ATTRIBUTE_NORETURN;
  void usage(const char* msg, const char* opt) ATTRIBUTE_NORETURN;
  void usage(const char* msg, char opt) ATTRIBUTE_NORETURN;
  void apply_option(const gold::options::One_option&, const char*);

  General_options options_;
  Position_dependent_options position_options_;
  Input_argument_list inputs_;
};

} // End namespace gold.

#endif // !defined(GOLD_OPTIONS_H)
