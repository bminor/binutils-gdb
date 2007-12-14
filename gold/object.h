// object.h -- support for an object file for linking in gold  -*- C++ -*-

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

#ifndef GOLD_OBJECT_H
#define GOLD_OBJECT_H

#include <string>
#include <vector>

#include "elfcpp.h"
#include "elfcpp_file.h"
#include "fileread.h"
#include "target.h"

namespace gold
{

class General_options;
class Layout;
class Output_section;
class Output_file;
class Dynobj;
class Object_merge_map;

template<typename Stringpool_char>
class Stringpool_template;

// Data to pass from read_symbols() to add_symbols().

struct Read_symbols_data
{
  // Section headers.
  File_view* section_headers;
  // Section names.
  File_view* section_names;
  // Size of section name data in bytes.
  off_t section_names_size;
  // Symbol data.
  File_view* symbols;
  // Size of symbol data in bytes.
  off_t symbols_size;
  // Offset of external symbols within symbol data.  This structure
  // sometimes contains only external symbols, in which case this will
  // be zero.  Sometimes it contains all symbols.
  off_t external_symbols_offset;
  // Symbol names.
  File_view* symbol_names;
  // Size of symbol name data in bytes.
  off_t symbol_names_size;

  // Version information.  This is only used on dynamic objects.
  // Version symbol data (from SHT_GNU_versym section).
  File_view* versym;
  off_t versym_size;
  // Version definition data (from SHT_GNU_verdef section).
  File_view* verdef;
  off_t verdef_size;
  unsigned int verdef_info;
  // Needed version data  (from SHT_GNU_verneed section).
  File_view* verneed;
  off_t verneed_size;
  unsigned int verneed_info;
};

// Information used to print error messages.

struct Symbol_location_info
{
  std::string source_file;
  std::string enclosing_symbol_name;
  int line_number;
};

// Data about a single relocation section.  This is read in
// read_relocs and processed in scan_relocs.

struct Section_relocs
{
  // Index of reloc section.
  unsigned int reloc_shndx;
  // Index of section that relocs apply to.
  unsigned int data_shndx;
  // Contents of reloc section.
  File_view* contents;
  // Reloc section type.
  unsigned int sh_type;
  // Number of reloc entries.
  size_t reloc_count;
  // Output section.
  Output_section* output_section;
  // Whether this section has special handling for offsets.
  bool needs_special_offset_handling;
};

// Relocations in an object file.  This is read in read_relocs and
// processed in scan_relocs.

struct Read_relocs_data
{
  typedef std::vector<Section_relocs> Relocs_list;
  // The relocations.
  Relocs_list relocs;
  // The local symbols.
  File_view* local_symbols;
};

// Object is an abstract base class which represents either a 32-bit
// or a 64-bit input object.  This can be a regular object file
// (ET_REL) or a shared object (ET_DYN).

class Object
{
 public:
  // NAME is the name of the object as we would report it to the user
  // (e.g., libfoo.a(bar.o) if this is in an archive.  INPUT_FILE is
  // used to read the file.  OFFSET is the offset within the input
  // file--0 for a .o or .so file, something else for a .a file.
  Object(const std::string& name, Input_file* input_file, bool is_dynamic,
	 off_t offset = 0)
    : name_(name), input_file_(input_file), offset_(offset), shnum_(-1U),
      is_dynamic_(is_dynamic), target_(NULL)
  { }

  virtual ~Object()
  { }

  // Return the name of the object as we would report it to the tuser.
  const std::string&
  name() const
  { return this->name_; }

  // Get the offset into the file.
  off_t
  offset() const
  { return this->offset_; }

  // Return whether this is a dynamic object.
  bool
  is_dynamic() const
  { return this->is_dynamic_; }

  // Return the target structure associated with this object.
  Target*
  target() const
  { return this->target_; }

  // Lock the underlying file.
  void
  lock()
  { this->input_file_->file().lock(); }

  // Unlock the underlying file.
  void
  unlock()
  { this->input_file_->file().unlock(); }

  // Return whether the underlying file is locked.
  bool
  is_locked() const
  { return this->input_file_->file().is_locked(); }

  // Return the sized target structure associated with this object.
  // This is like the target method but it returns a pointer of
  // appropriate checked type.
  template<int size, bool big_endian>
  Sized_target<size, big_endian>*
  sized_target(ACCEPT_SIZE_ENDIAN_ONLY);

  // Get the number of sections.
  unsigned int
  shnum() const
  { return this->shnum_; }

  // Return a view of the contents of a section.  Set *PLEN to the
  // size.  CACHE is a hint as in File_read::get_view.
  const unsigned char*
  section_contents(unsigned int shndx, off_t* plen, bool cache);

  // Return the name of a section given a section index.  This is only
  // used for error messages.
  std::string
  section_name(unsigned int shndx)
  { return this->do_section_name(shndx); }

  // Return the section flags given a section index.
  uint64_t
  section_flags(unsigned int shndx)
  { return this->do_section_flags(shndx); }

  // Return the section type given a section index.
  unsigned int
  section_type(unsigned int shndx)
  { return this->do_section_type(shndx); }

  // Return the section link field given a section index.
  unsigned int
  section_link(unsigned int shndx)
  { return this->do_section_link(shndx); }

  // Return the section info field given a section index.
  unsigned int
  section_info(unsigned int shndx)
  { return this->do_section_info(shndx); }

  // Read the symbol information.
  void
  read_symbols(Read_symbols_data* sd)
  { return this->do_read_symbols(sd); }

  // Pass sections which should be included in the link to the Layout
  // object, and record where the sections go in the output file.
  void
  layout(Symbol_table* symtab, Layout* layout, Read_symbols_data* sd)
  { this->do_layout(symtab, layout, sd); }

  // Add symbol information to the global symbol table.
  void
  add_symbols(Symbol_table* symtab, Read_symbols_data* sd)
  { this->do_add_symbols(symtab, sd); }

  // Functions and types for the elfcpp::Elf_file interface.  This
  // permit us to use Object as the File template parameter for
  // elfcpp::Elf_file.

  // The View class is returned by view.  It must support a single
  // method, data().  This is trivial, because get_view does what we
  // need.
  class View
  {
   public:
    View(const unsigned char* p)
      : p_(p)
    { }

    const unsigned char*
    data() const
    { return this->p_; }

   private:
    const unsigned char* p_;
  };

  // Return a View.
  View
  view(off_t file_offset, off_t data_size)
  { return View(this->get_view(file_offset, data_size, true)); }

  // Report an error.
  void
  error(const char* format, ...) const ATTRIBUTE_PRINTF_2;

  // A location in the file.
  struct Location
  {
    off_t file_offset;
    off_t data_size;

    Location(off_t fo, off_t ds)
      : file_offset(fo), data_size(ds)
    { }
  };

  // Get a View given a Location.
  View view(Location loc)
  { return View(this->get_view(loc.file_offset, loc.data_size, true)); }

 protected:
  // Read the symbols--implemented by child class.
  virtual void
  do_read_symbols(Read_symbols_data*) = 0;

  // Lay out sections--implemented by child class.
  virtual void
  do_layout(Symbol_table*, Layout*, Read_symbols_data*) = 0;

  // Add symbol information to the global symbol table--implemented by
  // child class.
  virtual void
  do_add_symbols(Symbol_table*, Read_symbols_data*) = 0;

  // Return the location of the contents of a section.  Implemented by
  // child class.
  virtual Location
  do_section_contents(unsigned int shndx) = 0;

  // Get the name of a section--implemented by child class.
  virtual std::string
  do_section_name(unsigned int shndx) = 0;

  // Get section flags--implemented by child class.
  virtual uint64_t
  do_section_flags(unsigned int shndx) = 0;

  // Get section type--implemented by child class.
  virtual unsigned int
  do_section_type(unsigned int shndx) = 0;

  // Get section link field--implemented by child class.
  virtual unsigned int
  do_section_link(unsigned int shndx) = 0;

  // Get section info field--implemented by child class.
  virtual unsigned int
  do_section_info(unsigned int shndx) = 0;

  // Get the file.
  Input_file*
  input_file() const
  { return this->input_file_; }

  // Get a view into the underlying file.
  const unsigned char*
  get_view(off_t start, off_t size, bool cache)
  {
    return this->input_file_->file().get_view(start + this->offset_, size,
					      cache);
  }

  // Get a lasting view into the underlying file.
  File_view*
  get_lasting_view(off_t start, off_t size, bool cache)
  {
    return this->input_file_->file().get_lasting_view(start + this->offset_,
						      size, cache);
  }

  // Read data from the underlying file.
  void
  read(off_t start, off_t size, void* p)
  { this->input_file_->file().read(start + this->offset_, size, p); }

  // Set the target.
  void
  set_target(int machine, int size, bool big_endian, int osabi,
	     int abiversion);

  // Set the number of sections.
  void
  set_shnum(int shnum)
  { this->shnum_ = shnum; }

  // Functions used by both Sized_relobj and Sized_dynobj.

  // Read the section data into a Read_symbols_data object.
  template<int size, bool big_endian>
  void
  read_section_data(elfcpp::Elf_file<size, big_endian, Object>*,
		    Read_symbols_data*);

  // If NAME is the name of a special .gnu.warning section, arrange
  // for the warning to be issued.  SHNDX is the section index.
  // Return whether it is a warning section.
  bool
  handle_gnu_warning_section(const char* name, unsigned int shndx,
			     Symbol_table*);

 private:
  // This class may not be copied.
  Object(const Object&);
  Object& operator=(const Object&);

  // Name of object as printed to user.
  std::string name_;
  // For reading the file.
  Input_file* input_file_;
  // Offset within the file--0 for an object file, non-0 for an
  // archive.
  off_t offset_;
  // Number of input sections.
  unsigned int shnum_;
  // Whether this is a dynamic object.
  bool is_dynamic_;
  // Target functions--may be NULL if the target is not known.
  Target* target_;
};

// Implement sized_target inline for efficiency.  This approach breaks
// static type checking, but is made safe using asserts.

template<int size, bool big_endian>
inline Sized_target<size, big_endian>*
Object::sized_target(ACCEPT_SIZE_ENDIAN_ONLY)
{
  gold_assert(this->target_->get_size() == size);
  gold_assert(this->target_->is_big_endian() ? big_endian : !big_endian);
  return static_cast<Sized_target<size, big_endian>*>(this->target_);
}

// A regular object (ET_REL).  This is an abstract base class itself.
// The implementation is the template class Sized_relobj.

class Relobj : public Object
{
 public:
  Relobj(const std::string& name, Input_file* input_file, off_t offset = 0)
    : Object(name, input_file, false, offset),
      map_to_output_(),
      object_merge_map_(NULL),
      relocs_must_follow_section_writes_(false)
  { }

  // Read the relocs.
  void
  read_relocs(Read_relocs_data* rd)
  { return this->do_read_relocs(rd); }

  // Scan the relocs and adjust the symbol table.
  void
  scan_relocs(const General_options& options, Symbol_table* symtab,
	      Layout* layout, Read_relocs_data* rd)
  { return this->do_scan_relocs(options, symtab, layout, rd); }

  // The number of local symbols in the input symbol table.
  virtual unsigned int
  local_symbol_count() const
  { return this->do_local_symbol_count(); }

  // Initial local symbol processing: count the number of local symbols
  // in the output symbol table and dynamic symbol table; add local symbol
  // names to *POOL and *DYNPOOL.
  void
  count_local_symbols(Stringpool_template<char>* pool,
                      Stringpool_template<char>* dynpool)
  { return this->do_count_local_symbols(pool, dynpool); }

  // Set the values of the local symbols, set the output symbol table
  // indexes for the local variables, and set the offset where local
  // symbol information will be stored. Returns the new local symbol index.
  unsigned int
  finalize_local_symbols(unsigned int index, off_t off)
  { return this->do_finalize_local_symbols(index, off); }

  // Set the output dynamic symbol table indexes for the local variables.
  unsigned int
  set_local_dynsym_indexes(unsigned int index)
  { return this->do_set_local_dynsym_indexes(index); }

  // Set the offset where local dynamic symbol information will be stored.
  unsigned int
  set_local_dynsym_offset(off_t off)
  { return this->do_set_local_dynsym_offset(off); }

  // Relocate the input sections and write out the local symbols.
  void
  relocate(const General_options& options, const Symbol_table* symtab,
	   const Layout* layout, Output_file* of)
  { return this->do_relocate(options, symtab, layout, of); }

  // Return whether an input section is being included in the link.
  bool
  is_section_included(unsigned int shndx) const
  {
    gold_assert(shndx < this->map_to_output_.size());
    return this->map_to_output_[shndx].output_section != NULL;
  }

  // Return whether an input section requires special
  // handling--whether it is not simply mapped from the input file to
  // the output file.
  bool
  is_section_specially_mapped(unsigned int shndx) const
  {
    gold_assert(shndx < this->map_to_output_.size());
    return (this->map_to_output_[shndx].output_section != NULL
	    && this->map_to_output_[shndx].offset == -1);
  }

  // Given a section index, return the corresponding Output_section
  // (which will be NULL if the section is not included in the link)
  // and set *POFF to the offset within that section.  *POFF will be
  // set to -1 if the section requires special handling.
  inline Output_section*
  output_section(unsigned int shndx, off_t* poff) const;

  // Set the offset of an input section within its output section.
  void
  set_section_offset(unsigned int shndx, off_t off)
  {
    gold_assert(shndx < this->map_to_output_.size());
    this->map_to_output_[shndx].offset = off;
  }

  // Return true if we need to wait for output sections to be written
  // before we can apply relocations.  This is true if the object has
  // any relocations for sections which require special handling, such
  // as the exception frame section.
  bool
  relocs_must_follow_section_writes()
  { return this->relocs_must_follow_section_writes_; }

  // Return the object merge map.
  Object_merge_map*
  merge_map() const
  { return this->object_merge_map_; }

  // Set the object merge map.
  void
  set_merge_map(Object_merge_map* object_merge_map)
  {
    gold_assert(this->object_merge_map_ == NULL);
    this->object_merge_map_ = object_merge_map;
  }

 protected:
  // What we need to know to map an input section to an output
  // section.  We keep an array of these, one for each input section,
  // indexed by the input section number.
  struct Map_to_output
  {
    // The output section.  This is NULL if the input section is to be
    // discarded.
    Output_section* output_section;
    // The offset within the output section.  This is -1 if the
    // section requires special handling.
    off_t offset;
  };

  // Read the relocs--implemented by child class.
  virtual void
  do_read_relocs(Read_relocs_data*) = 0;

  // Scan the relocs--implemented by child class.
  virtual void
  do_scan_relocs(const General_options&, Symbol_table*, Layout*,
		 Read_relocs_data*) = 0;

  // Return the number of local symbols--implemented by child class.
  virtual unsigned int
  do_local_symbol_count() const = 0;

  // Count local symbols--implemented by child class.
  virtual void
  do_count_local_symbols(Stringpool_template<char>*,
			    Stringpool_template<char>*) = 0;

  // Finalize the local symbols.  Set the output symbol table indexes for the local variables, and set the
  // offset where local symbol information will be stored.
  virtual unsigned int
  do_finalize_local_symbols(unsigned int, off_t) = 0;

  // Set the output dynamic symbol table indexes for the local variables.
  virtual unsigned int
  do_set_local_dynsym_indexes(unsigned int) = 0;

  // Set the offset where local dynamic symbol information will be stored.
  virtual unsigned int
  do_set_local_dynsym_offset(off_t) = 0;

  // Relocate the input sections and write out the local
  // symbols--implemented by child class.
  virtual void
  do_relocate(const General_options& options, const Symbol_table* symtab,
	      const Layout*, Output_file* of) = 0;

  // Return the vector mapping input sections to output sections.
  std::vector<Map_to_output>&
  map_to_output()
  { return this->map_to_output_; }

  const std::vector<Map_to_output>&
  map_to_output() const
  { return this->map_to_output_; }

  // Record that we must wait for the output sections to be written
  // before applying relocations.
  void
  set_relocs_must_follow_section_writes()
  { this->relocs_must_follow_section_writes_ = true; }

 private:
  // Mapping from input sections to output section.
  std::vector<Map_to_output> map_to_output_;
  // Mappings for merge sections.  This is managed by the code in the
  // Merge_map class.
  Object_merge_map* object_merge_map_;
  // Whether we need to wait for output sections to be written before
  // we can apply relocations.
  bool relocs_must_follow_section_writes_;
};

// Implement Object::output_section inline for efficiency.
inline Output_section*
Relobj::output_section(unsigned int shndx, off_t* poff) const
{
  gold_assert(shndx < this->map_to_output_.size());
  const Map_to_output& mo(this->map_to_output_[shndx]);
  *poff = mo.offset;
  return mo.output_section;
}

// This POD class is holds the value of a symbol.  This is used for
// local symbols, and for all symbols during relocation processing.
// For special sections, such as SHF_MERGE sections, this calls a
// function to get the final symbol value.

template<int size>
class Symbol_value
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Value;

  Symbol_value()
    : output_symtab_index_(0), output_dynsym_index_(-1U), input_shndx_(0),
      is_section_symbol_(false), is_tls_symbol_(false),
      needs_output_address_(false), value_(0)
  { }

  // Get the value of this symbol.  OBJECT is the object in which this
  // symbol is defined, and ADDEND is an addend to add to the value.
  template<bool big_endian>
  Value
  value(const Sized_relobj<size, big_endian>* object, Value addend) const
  {
    if (!this->needs_output_address_)
      return this->value_ + addend;
    return object->local_value(this->input_shndx_, this->value_,
			       this->is_section_symbol_, addend);
  }

  // Set the value of this symbol in the output symbol table.
  void
  set_output_value(Value value)
  {
    this->value_ = value;
    this->needs_output_address_ = false;
  }

  // Set the value of the symbol from the input file.  This value
  // will usually be replaced during finalization with the output
  // value, but if the symbol is mapped to an output section which
  // requires special handling to determine the output value, we
  // leave the input value in place until later.  This is used for
  // SHF_MERGE sections.
  void
  set_input_value(Value value)
  {
    this->value_ = value;
    this->needs_output_address_ = true;
  }

  // Return the input value.
  Value
  input_value() const
  {
    gold_assert(this->needs_output_address_);
    return this->value_;
  }

  // Return whether this symbol should go into the output symbol
  // table.
  bool
  needs_output_symtab_entry() const
  { return this->output_symtab_index_ != -1U; }

  // Return the index in the output symbol table.
  unsigned int
  output_symtab_index() const
  {
    gold_assert(this->output_symtab_index_ != 0);
    return this->output_symtab_index_;
  }

  // Set the index in the output symbol table.
  void
  set_output_symtab_index(unsigned int i)
  {
    gold_assert(this->output_symtab_index_ == 0);
    this->output_symtab_index_ = i;
  }

  // Record that this symbol should not go into the output symbol
  // table.
  void
  set_no_output_symtab_entry()
  {
    gold_assert(this->output_symtab_index_ == 0);
    this->output_symtab_index_ = -1U;
  }

  // Set the index in the output dynamic symbol table.
  void
  set_needs_output_dynsym_entry()
  {
    this->output_dynsym_index_ = 0;
  }

  // Return whether this symbol should go into the output symbol
  // table.
  bool
  needs_output_dynsym_entry() const
  {
    return this->output_dynsym_index_ != -1U;
  }

  // Record that this symbol should go into the dynamic symbol table.
  void
  set_output_dynsym_index(unsigned int i)
  {
    gold_assert(this->output_dynsym_index_ == 0);
    this->output_dynsym_index_ = i;
  }

  // Return the index in the output dynamic symbol table.
  unsigned int
  output_dynsym_index() const
  {
    gold_assert(this->output_dynsym_index_ != 0);
    return this->output_dynsym_index_;
  }

  // Set the index of the input section in the input file.
  void
  set_input_shndx(unsigned int i)
  {
    this->input_shndx_ = i;
    // input_shndx_ field is a bitfield, so make sure that the value
    // fits.
    gold_assert(this->input_shndx_ == i);
  }

  // Return the index of the input section in the input file.
  unsigned int
  input_shndx() const
  { return this->input_shndx_; }

  // Record that this is a section symbol.
  void
  set_is_section_symbol()
  { this->is_section_symbol_ = true; }

  // Record that this is a TLS symbol.
  void
  set_is_tls_symbol()
  { this->is_tls_symbol_ = true; }

  // Return TRUE if this is a TLS symbol.
  bool
  is_tls_symbol() const
  { return this->is_tls_symbol_; }

 private:
  // The index of this local symbol in the output symbol table.  This
  // will be -1 if the symbol should not go into the symbol table.
  unsigned int output_symtab_index_;
  // The index of this local symbol in the dynamic symbol table.  This
  // will be -1 if the symbol should not go into the symbol table.
  unsigned int output_dynsym_index_;
  // The section index in the input file in which this symbol is
  // defined.
  unsigned int input_shndx_ : 29;
  // Whether this is a STT_SECTION symbol.
  bool is_section_symbol_ : 1;
  // Whether this is a STT_TLS symbol.
  bool is_tls_symbol_ : 1;
  // Whether getting the value of this symbol requires calling an
  // Output_section method.  For example, this will be true of a
  // symbol in a SHF_MERGE section.
  bool needs_output_address_ : 1;
  // The value of the symbol.  If !needs_output_address_, this is the
  // value in the output file.  If needs_output_address_, this is the
  // value in the input file.
  Value value_;
};

// A regular object file.  This is size and endian specific.

template<int size, bool big_endian>
class Sized_relobj : public Relobj
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef std::vector<Symbol*> Symbols;
  typedef std::vector<Symbol_value<size> > Local_values;

  Sized_relobj(const std::string& name, Input_file* input_file, off_t offset,
	       const typename elfcpp::Ehdr<size, big_endian>&);

  ~Sized_relobj();

  // Set up the object file based on the ELF header.
  void
  setup(const typename elfcpp::Ehdr<size, big_endian>&);

  // If SYM is the index of a global symbol in the object file's
  // symbol table, return the Symbol object.  Otherwise, return NULL.
  Symbol*
  global_symbol(unsigned int sym) const
  {
    if (sym >= this->local_symbol_count_)
      {
	gold_assert(sym - this->local_symbol_count_ < this->symbols_.size());
	return this->symbols_[sym - this->local_symbol_count_];
      }
    return NULL;
  }

  // Return the section index of symbol SYM.  Set *VALUE to its value
  // in the object file.  Note that for a symbol which is not defined
  // in this object file, this will set *VALUE to 0 and return
  // SHN_UNDEF; it will not return the final value of the symbol in
  // the link.
  unsigned int
  symbol_section_and_value(unsigned int sym, Address* value);

  // Return a pointer to the Symbol_value structure which holds the
  // value of a local symbol.
  const Symbol_value<size>*
  local_symbol(unsigned int sym) const
  {
    gold_assert(sym < this->local_values_.size());
    return &this->local_values_[sym];
  }

  // Return the index of local symbol SYM in the ordinary symbol
  // table.  A value of -1U means that the symbol is not being output.
  unsigned int
  symtab_index(unsigned int sym) const
  {
    gold_assert(sym < this->local_values_.size());
    return this->local_values_[sym].output_symtab_index();
  }

  // Return the index of local symbol SYM in the dynamic symbol
  // table.  A value of -1U means that the symbol is not being output.
  unsigned int
  dynsym_index(unsigned int sym) const
  {
    gold_assert(sym < this->local_values_.size());
    return this->local_values_[sym].output_dynsym_index();
  }

  // Return the appropriate Sized_target structure.
  Sized_target<size, big_endian>*
  sized_target()
  {
    return this->Object::sized_target
      SELECT_SIZE_ENDIAN_NAME(size, big_endian) (
          SELECT_SIZE_ENDIAN_ONLY(size, big_endian));
  }

  // Return the value of the local symbol symndx.
  Address
  local_symbol_value(unsigned int symndx) const;

  // Return the value of a local symbol defined in input section
  // SHNDX, with value VALUE, adding addend ADDEND.  IS_SECTION_SYMBOL
  // indicates whether the symbol is a section symbol.  This handles
  // SHF_MERGE sections.
  Address
  local_value(unsigned int shndx, Address value, bool is_section_symbol,
	      Address addend) const;

  void
  set_needs_output_dynsym_entry(unsigned int sym)
  {
    gold_assert(sym < this->local_values_.size());
    this->local_values_[sym].set_needs_output_dynsym_entry();
  }

  // Return whether the local symbol SYMNDX has a GOT offset.
  // For TLS symbols, the GOT entry will hold its tp-relative offset.
  bool
  local_has_got_offset(unsigned int symndx) const
  {
    return (this->local_got_offsets_.find(symndx)
            != this->local_got_offsets_.end());
  }

  // Return the GOT offset of the local symbol SYMNDX.
  unsigned int
  local_got_offset(unsigned int symndx) const
  {
    Local_got_offsets::const_iterator p =
        this->local_got_offsets_.find(symndx);
    gold_assert(p != this->local_got_offsets_.end());
    return p->second;
  }

  // Set the GOT offset of the local symbol SYMNDX to GOT_OFFSET.
  void
  set_local_got_offset(unsigned int symndx, unsigned int got_offset)
  {
    std::pair<Local_got_offsets::iterator, bool> ins =
        this->local_got_offsets_.insert(std::make_pair(symndx, got_offset));
    gold_assert(ins.second);
  }

  // Return whether the local TLS symbol SYMNDX has a GOT offset.
  // The GOT entry at this offset will contain a module index. If
  // NEED_PAIR is true, a second entry immediately following the first
  // will contain the dtv-relative offset.
  bool
  local_has_tls_got_offset(unsigned int symndx, bool need_pair) const
  {
    typename Local_tls_got_offsets::const_iterator p =
        this->local_tls_got_offsets_.find(symndx);
    if (p == this->local_tls_got_offsets_.end()
        || (need_pair && !p->second.have_pair_))
      return false;
    return true;
  }

  // Return the offset of the GOT entry for the local TLS symbol SYMNDX.
  // If NEED_PAIR is true, we need the offset of a pair of GOT entries;
  // otherwise we need the offset of the GOT entry for the module index.
  unsigned int
  local_tls_got_offset(unsigned int symndx, bool need_pair) const
  {
    typename Local_tls_got_offsets::const_iterator p =
        this->local_tls_got_offsets_.find(symndx);
    gold_assert(p != this->local_tls_got_offsets_.end());
    gold_assert(!need_pair || p->second.have_pair_);
    return p->second.got_offset_;
  }

  // Set the offset of the GOT entry for the local TLS symbol SYMNDX
  // to GOT_OFFSET. If HAVE_PAIR is true, we have a pair of GOT entries;
  // otherwise, we have just a single entry for the module index.
  void
  set_local_tls_got_offset(unsigned int symndx, unsigned int got_offset,
                           bool have_pair)
  {
    typename Local_tls_got_offsets::iterator p =
        this->local_tls_got_offsets_.find(symndx);
    if (p != this->local_tls_got_offsets_.end())
      {
        // An entry already existed for this symbol. This can happen
        // if we see a relocation asking for the module index before
        // a relocation asking for the pair. In that case, the original
        // GOT entry will remain, but won't get used by any further
        // relocations.
        p->second.got_offset_ = got_offset;
	gold_assert(have_pair);
        p->second.have_pair_ = true;
      }
    else
      {
        std::pair<typename Local_tls_got_offsets::iterator, bool> ins =
            this->local_tls_got_offsets_.insert(
              std::make_pair(symndx, Tls_got_entry(got_offset, have_pair)));
        gold_assert(ins.second);
      }
  }

  // Return the name of the symbol that spans the given offset in the
  // specified section in this object.  This is used only for error
  // messages and is not particularly efficient.
  bool
  get_symbol_location_info(unsigned int shndx, off_t offset,
			   Symbol_location_info* info);

 protected:
  // Read the symbols.
  void
  do_read_symbols(Read_symbols_data*);

  // Return the number of local symbols.
  unsigned int
  do_local_symbol_count() const
  { return this->local_symbol_count_; }

  // Lay out the input sections.
  void
  do_layout(Symbol_table*, Layout*, Read_symbols_data*);

  // Add the symbols to the symbol table.
  void
  do_add_symbols(Symbol_table*, Read_symbols_data*);

  // Read the relocs.
  void
  do_read_relocs(Read_relocs_data*);

  // Scan the relocs and adjust the symbol table.
  void
  do_scan_relocs(const General_options&, Symbol_table*, Layout*,
		 Read_relocs_data*);

  // Count the local symbols.
  void
  do_count_local_symbols(Stringpool_template<char>*,
                            Stringpool_template<char>*);

  // Finalize the local symbols.
  unsigned int
  do_finalize_local_symbols(unsigned int, off_t);

  // Set the offset where local dynamic symbol information will be stored.
  unsigned int
  do_set_local_dynsym_indexes(unsigned int);

  // Set the offset where local dynamic symbol information will be stored.
  unsigned int
  do_set_local_dynsym_offset(off_t);

  // Relocate the input sections and write out the local symbols.
  void
  do_relocate(const General_options& options, const Symbol_table* symtab,
	      const Layout*, Output_file* of);

  // Get the name of a section.
  std::string
  do_section_name(unsigned int shndx)
  { return this->elf_file_.section_name(shndx); }

  // Return the location of the contents of a section.
  Object::Location
  do_section_contents(unsigned int shndx)
  { return this->elf_file_.section_contents(shndx); }

  // Return section flags.
  uint64_t
  do_section_flags(unsigned int shndx)
  { return this->elf_file_.section_flags(shndx); }

  // Return section type.
  unsigned int
  do_section_type(unsigned int shndx)
  { return this->elf_file_.section_type(shndx); }

  // Return the section link field.
  unsigned int
  do_section_link(unsigned int shndx)
  { return this->elf_file_.section_link(shndx); }

  // Return the section info field.
  unsigned int
  do_section_info(unsigned int shndx)
  { return this->elf_file_.section_info(shndx); }

 private:
  // For convenience.
  typedef Sized_relobj<size, big_endian> This;
  static const int ehdr_size = elfcpp::Elf_sizes<size>::ehdr_size;
  static const int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;
  static const int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  typedef elfcpp::Shdr<size, big_endian> Shdr;

  // Find the SHT_SYMTAB section, given the section headers.
  void
  find_symtab(const unsigned char* pshdrs);

  // Return whether SHDR has the right flags for a GNU style exception
  // frame section.
  bool
  check_eh_frame_flags(const elfcpp::Shdr<size, big_endian>* shdr) const;

  // Return whether there is a section named .eh_frame which might be
  // a GNU style exception frame section.
  bool
  find_eh_frame(const unsigned char* pshdrs, const char* names,
		off_t names_size) const;

  // Whether to include a section group in the link.
  bool
  include_section_group(Layout*, unsigned int,
			const elfcpp::Shdr<size, big_endian>&,
			std::vector<bool>*);

  // Whether to include a linkonce section in the link.
  bool
  include_linkonce_section(Layout*, const char*,
			   const elfcpp::Shdr<size, big_endian>&);

  // Views and sizes when relocating.
  struct View_size
  {
    unsigned char* view;
    typename elfcpp::Elf_types<size>::Elf_Addr address;
    off_t offset;
    off_t view_size;
    bool is_input_output_view;
    bool is_postprocessing_view;
  };

  typedef std::vector<View_size> Views;

  // Write section data to the output file.  Record the views and
  // sizes in VIEWS for use when relocating.
  void
  write_sections(const unsigned char* pshdrs, Output_file*, Views*);

  // Relocate the sections in the output file.
  void
  relocate_sections(const General_options& options, const Symbol_table*,
		    const Layout*, const unsigned char* pshdrs, Views*);

  // Write out the local symbols.
  void
  write_local_symbols(Output_file*,
		      const Stringpool_template<char>*,
		      const Stringpool_template<char>*);

  // The GOT offsets of local symbols. This map also stores GOT offsets
  // for tp-relative offsets for TLS symbols.
  typedef Unordered_map<unsigned int, unsigned int> Local_got_offsets;

  // The TLS GOT offsets of local symbols. The map stores the offsets
  // for either a single GOT entry that holds the module index of a TLS
  // symbol, or a pair of GOT entries containing the module index and
  // dtv-relative offset.
  struct Tls_got_entry
  {
    Tls_got_entry(int got_offset, bool have_pair)
      : got_offset_(got_offset),
        have_pair_(have_pair)
    { }
    int got_offset_;
    bool have_pair_;
  };
  typedef Unordered_map<unsigned int, Tls_got_entry> Local_tls_got_offsets;

  // General access to the ELF file.
  elfcpp::Elf_file<size, big_endian, Object> elf_file_;
  // Index of SHT_SYMTAB section.
  unsigned int symtab_shndx_;
  // The number of local symbols.
  unsigned int local_symbol_count_;
  // The number of local symbols which go into the output file.
  unsigned int output_local_symbol_count_;
  // The number of local symbols which go into the output file's dynamic
  // symbol table.
  unsigned int output_local_dynsym_count_;
  // The entries in the symbol table for the external symbols.
  Symbols symbols_;
  // File offset for local symbols.
  off_t local_symbol_offset_;
  // File offset for local dynamic symbols.
  off_t local_dynsym_offset_;
  // Values of local symbols.
  Local_values local_values_;
  // GOT offsets for local non-TLS symbols, and tp-relative offsets
  // for TLS symbols, indexed by symbol number.
  Local_got_offsets local_got_offsets_;
  // GOT offsets for local TLS symbols, indexed by symbol number
  // and GOT entry type.
  Local_tls_got_offsets local_tls_got_offsets_;
  // Whether this object has a GNU style .eh_frame section.
  bool has_eh_frame_;
};

// A class to manage the list of all objects.

class Input_objects
{
 public:
  Input_objects()
    : relobj_list_(), dynobj_list_(), target_(NULL), sonames_(),
      system_library_directory_()
  { }

  // The type of the list of input relocateable objects.
  typedef std::vector<Relobj*> Relobj_list;
  typedef Relobj_list::const_iterator Relobj_iterator;

  // The type of the list of input dynamic objects.
  typedef std::vector<Dynobj*> Dynobj_list;
  typedef Dynobj_list::const_iterator Dynobj_iterator;

  // Add an object to the list.  Return true if all is well, or false
  // if this object should be ignored.
  bool
  add_object(Object*);

  // Get the target we should use for the output file.
  Target*
  target() const
  { return this->target_; }

  // For each dynamic object, check whether we've seen all of its
  // explicit dependencies.
  void
  check_dynamic_dependencies() const;

  // Return whether an object was found in the system library
  // directory.
  bool
  found_in_system_library_directory(const Object*) const;

  // Iterate over all regular objects.

  Relobj_iterator
  relobj_begin() const
  { return this->relobj_list_.begin(); }

  Relobj_iterator
  relobj_end() const
  { return this->relobj_list_.end(); }

  // Iterate over all dynamic objects.

  Dynobj_iterator
  dynobj_begin() const
  { return this->dynobj_list_.begin(); }

  Dynobj_iterator
  dynobj_end() const
  { return this->dynobj_list_.end(); }

  // Return whether we have seen any dynamic objects.
  bool
  any_dynamic() const
  { return !this->dynobj_list_.empty(); }

  // Return the number of input objects.
  int
  number_of_input_objects() const
  { return this->relobj_list_.size() + this->dynobj_list_.size(); }

 private:
  Input_objects(const Input_objects&);
  Input_objects& operator=(const Input_objects&);

  // The list of ordinary objects included in the link.
  Relobj_list relobj_list_;
  // The list of dynamic objects included in the link.
  Dynobj_list dynobj_list_;
  // The target.
  Target* target_;
  // SONAMEs that we have seen.
  Unordered_set<std::string> sonames_;
  // The directory in which we find the libc.so.
  std::string system_library_directory_;
};

// Some of the information we pass to the relocation routines.  We
// group this together to avoid passing a dozen different arguments.

template<int size, bool big_endian>
struct Relocate_info
{
  // Command line options.
  const General_options* options;
  // Symbol table.
  const Symbol_table* symtab;
  // Layout.
  const Layout* layout;
  // Object being relocated.
  Sized_relobj<size, big_endian>* object;
  // Section index of relocation section.
  unsigned int reloc_shndx;
  // Section index of section being relocated.
  unsigned int data_shndx;

  // Return a string showing the location of a relocation.  This is
  // only used for error messages.
  std::string
  location(size_t relnum, off_t reloffset) const;
};

// Return an Object appropriate for the input file.  P is BYTES long,
// and holds the ELF header.

extern Object*
make_elf_object(const std::string& name, Input_file*,
		off_t offset, const unsigned char* p,
		off_t bytes);

} // end namespace gold

#endif // !defined(GOLD_OBJECT_H)
