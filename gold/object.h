// object.h -- support for an object file for linking in gold  -*- C++ -*-

#ifndef GOLD_OBJECT_H
#define GOLD_OBJECT_H

#include <cassert>
#include <vector>

#include "elfcpp.h"
#include "fileread.h"
#include "target.h"
#include "symtab.h"

namespace gold
{

class Stringpool;
class Layout;
class Output_section;
class Output_file;

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
  // Symbol names.
  File_view* symbol_names;
  // Size of symbol name data in bytes.
  off_t symbol_names_size;
};

// Object is an interface which represents either a 32-bit or a 64-bit
// input object.  This can be a regular object file (ET_REL) or a
// shared object (ET_DYN).  The actual instantiations are
// Sized_object<32> and Sized_object<64>

class Object
{
 public:
  // NAME is the name of the object as we would report it to the user
  // (e.g., libfoo.a(bar.o) if this is in an archive.  INPUT_FILE is
  // used to read the file.  OFFSET is the offset within the input
  // file--0 for a .o or .so file, something else for a .a file.
  Object(const std::string& name, Input_file* input_file, bool is_dynamic,
	 off_t offset = 0)
    : name_(name), input_file_(input_file), offset_(offset),
      shnum_(0), is_dynamic_(is_dynamic), target_(NULL),
      map_to_output_()
  { }

  virtual ~Object()
  { }

  // Return the name of the object as we would report it to the tuser.
  const std::string&
  name() const
  { return this->name_; }

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

  // Read the symbol and relocation information.
  void
  read_symbols(Read_symbols_data* sd)
  { return this->do_read_symbols(sd); }

  // Add symbol information to the global symbol table.
  void
  add_symbols(Symbol_table* symtab, Read_symbols_data* sd)
  { this->do_add_symbols(symtab, sd); }

  // Pass sections which should be included in the link to the Layout
  // object, and record where the sections go in the output file.
  void
  layout(Layout* lay, Read_symbols_data* sd)
  { this->do_layout(lay, sd); }

  // Initial local symbol processing: set the offset where local
  // symbol information will be stored; add local symbol names to
  // *POOL; return the offset following the local symbols.
  off_t
  finalize_local_symbols(off_t off, Stringpool* pool)
  { return this->do_finalize_local_symbols(off, pool); }

  // Relocate the input sections and write out the local symbols.
  void
  relocate(const General_options& options, const Symbol_table* symtab,
	   const Stringpool* sympool, Output_file* of)
  { return this->do_relocate(options, symtab, sympool, of); }

  // What we need to know to map an input section to an output
  // section.  We keep an array of these, one for each input section,
  // indexed by the input section number.
  struct Map_to_output
  {
    // The output section.  This is NULL if the input section is to be
    // discarded.
    Output_section* output_section;
    // The offset within the output section.
    off_t offset;
  };

  // Given a section index, return the corresponding Map_to_output
  // information.
  const Map_to_output*
  section_output_info(unsigned int shnum) const
  {
    assert(shnum < this->map_to_output_.size());
    return &this->map_to_output_[shnum];
  }

 protected:
  // Read the symbols--implemented by child class.
  virtual void
  do_read_symbols(Read_symbols_data*) = 0;

  // Add symbol information to the global symbol table--implemented by
  // child class.
  virtual void
  do_add_symbols(Symbol_table*, Read_symbols_data*) = 0;

  // Lay out sections--implemented by child class.
  virtual void
  do_layout(Layout*, Read_symbols_data*) = 0;

  // Finalize local symbols--implemented by child class.
  virtual off_t
  do_finalize_local_symbols(off_t, Stringpool*) = 0;

  // Relocate the input sections and write out the local
  // symbols--implemented by child class.
  virtual void
  do_relocate(const General_options& options, const Symbol_table* symtab,
	      const Stringpool*, Output_file* of) = 0;

  // Get the file.
  Input_file*
  input_file() const
  { return this->input_file_; }

  // Get the offset into the file.
  off_t
  offset() const
  { return this->offset_; }

  // Get a view into the underlying file.
  const unsigned char*
  get_view(off_t start, off_t size);

  // Get the number of sections.
  unsigned int
  shnum() const
  { return this->shnum_; }

  // Set the number of sections.
  void
  set_shnum(int shnum)
  { this->shnum_ = shnum; }

  // Set the target.
  void
  set_target(Target* target)
  { this->target_ = target; }

  // Read data from the underlying file.
  void
  read(off_t start, off_t size, void* p);

  // Get a lasting view into the underlying file.
  File_view*
  get_lasting_view(off_t start, off_t size);

  // Return the vector mapping input sections to output sections.
  std::vector<Map_to_output>&
  map_to_output()
  { return this->map_to_output_; }

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
  // Mapping from input sections to output section.
  std::vector<Map_to_output> map_to_output_;
};

// Implement sized_target inline for efficiency.  This approach breaks
// static type checking, but is made safe using asserts.

template<int size, bool big_endian>
inline Sized_target<size, big_endian>*
Object::sized_target(ACCEPT_SIZE_ENDIAN_ONLY)
{
  assert(this->target_->get_size() == size);
  assert(this->target_->is_big_endian() ? big_endian : !big_endian);
  return static_cast<Sized_target<size, big_endian>*>(this->target_);
}

// A regular object file.  This is size and endian specific.

template<int size, bool big_endian>
class Sized_object : public Object
{
 public:
  Sized_object(const std::string& name, Input_file* input_file, off_t offset,
	       const typename elfcpp::Ehdr<size, big_endian>&);

  ~Sized_object();

  // Set up the object file based on the ELF header.
  void
  setup(const typename elfcpp::Ehdr<size, big_endian>&);

  // Read the symbols.
  void
  do_read_symbols(Read_symbols_data*);

  // Lay out the input sections.
  void
  do_layout(Layout*, Read_symbols_data*);

  // Add the symbols to the symbol table.
  void
  do_add_symbols(Symbol_table*, Read_symbols_data*);

  // Finalize the local symbols.
  off_t
  do_finalize_local_symbols(off_t, Stringpool*);

  // Relocate the input sections and write out the local symbols.
  void
  do_relocate(const General_options& options, const Symbol_table* symtab,
	      const Stringpool*, Output_file* of);

  // Return the appropriate Sized_target structure.
  Sized_target<size, big_endian>*
  sized_target()
  {
    return this->Object::sized_target SELECT_SIZE_ENDIAN_NAME (
      SELECT_SIZE_ENDIAN_ONLY(size, big_endian));
  }

 private:
  // This object may not be copied.
  Sized_object(const Sized_object&);
  Sized_object& operator=(const Sized_object&);

  // For convenience.
  typedef Sized_object<size, big_endian> This;
  static const int ehdr_size = elfcpp::Elf_sizes<size>::ehdr_size;
  static const int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;
  static const int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  typedef elfcpp::Shdr<size, big_endian> Shdr;

  // Read the section header for section SHNUM.
  const unsigned char*
  section_header(unsigned int shnum);

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
  };

  typedef std::vector<View_size> Views;

  // Write section data to the output file.  Record the views and
  // sizes in VIEWS for use when relocating.
  void
  write_sections(const unsigned char* pshdrs, Output_file*, Views*);

  // Relocate the sections in the output file.
  void
  relocate_sections(const Symbol_table*, const unsigned char* pshdrs, Views*);

  // Write out the local symbols.
  void
  write_local_symbols(Output_file*, const Stringpool*);

  // If non-NULL, a view of the section header data.
  File_view* section_headers_;
  // ELF file header e_flags field.
  unsigned int flags_;
  // File offset of section header table.
  off_t shoff_;
  // Offset of SHT_STRTAB section holding section names.
  unsigned int shstrndx_;
  // Index of SHT_SYMTAB section.
  unsigned int symtab_shnum_;
  // The number of local symbols.
  unsigned int local_symbol_count_;
  // The number of local symbols which go into the output file.
  unsigned int output_local_symbol_count_;
  // The entries in the symbol table for the external symbols.
  Symbol** symbols_;
  // File offset for local symbols.
  off_t local_symbol_offset_;
  // Values of local symbols.
  typename elfcpp::Elf_types<size>::Elf_Addr *values_;
};

// A class to manage the list of all objects.

class Input_objects
{
 public:
  Input_objects()
    : object_list_(), target_(NULL), any_dynamic_(false)
  { }

  // The type of the list of input objects.
  typedef std::list<Object*> Object_list;

  // Add an object to the list.
  void
  add_object(Object*);

  // Get the target we should use for the output file.
  Target*
  target() const
  { return this->target_; }

  // Iterate over all objects.
  Object_list::const_iterator
  begin() const
  { return this->object_list_.begin(); }

  Object_list::const_iterator
  end() const
  { return this->object_list_.end(); }

  // Return whether we have seen any dynamic objects.
  bool
  any_dynamic() const
  { return this->any_dynamic_; }

 private:
  Input_objects(const Input_objects&);
  Input_objects& operator=(const Input_objects&);

  Object_list object_list_;
  Target* target_;
  bool any_dynamic_;
};

// Return an Object appropriate for the input file.  P is BYTES long,
// and holds the ELF header.

extern Object*
make_elf_object(const std::string& name, Input_file*,
		off_t offset, const unsigned char* p,
		off_t bytes);

} // end namespace gold

#endif // !defined(GOLD_OBJECT_H)
