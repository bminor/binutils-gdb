// object.h -- support for an object file for linking in gold  -*- C++ -*-

#ifndef GOLD_OBJECT_H
#define GOLD_OBJECT_H

#include <cassert>

#include "elfcpp.h"
#include "fileread.h"
#include "target.h"
#include "symtab.h"

namespace gold
{

// Data to pass from read_symbols() to add_symbols().

struct Read_symbols_data
{
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
      is_dynamic_(is_dynamic), target_(NULL)
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

  // Read the symbol and relocation information.
  Read_symbols_data
  read_symbols()
  { return this->do_read_symbols(); }

  // Add symbol information to the global symbol table.
  void
  add_symbols(Symbol_table* symtab, Read_symbols_data rd)
  { this->do_add_symbols(symtab, rd); }

  // Return the target structure associated with this object.
  Target*
  target()
  { return this->target_; }

  // Return the sized target structure associated with this object.
  // This is like the target method but it returns a pointer of
  // appropriate checked type.
  template<int size, bool big_endian>
  Sized_target<size, big_endian>*
  sized_target();

 protected:
  // Read the symbols--implemented by child class.
  virtual Read_symbols_data
  do_read_symbols() = 0;

  // Add symbol information to the global symbol table--implemented by
  // child class.
  virtual void
  do_add_symbols(Symbol_table*, Read_symbols_data) = 0;

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

 private:
  // This class may not be copied.
  Object(const Object&);
  Object& operator=(const Object&);

  // Name of object as printed to use.
  std::string name_;
  // For reading the file.
  Input_file* input_file_;
  // Offset within the file--0 for an object file, non-0 for an
  // archive.
  off_t offset_;
  // Whether this is a dynamic object.
  bool is_dynamic_;
  // Target functions--may be NULL if the target is not known.
  Target* target_;
};

// Implement sized_target inline for efficiency.  This approach breaks
// static type checking, but is made safe using asserts.

template<int size, bool big_endian>
inline Sized_target<size, big_endian>*
Object::sized_target()
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

  void
  setup(const typename elfcpp::Ehdr<size, big_endian>&);

  Read_symbols_data
  do_read_symbols();

  void
  do_add_symbols(Symbol_table*, Read_symbols_data);

  Sized_target<size, big_endian>*
  sized_target()
  { return this->Object::sized_target<size, big_endian>(); }

 private:
  // This object may not be copied.
  Sized_object(const Sized_object&);
  Sized_object& operator=(const Sized_object&);

  // ELF file header EI_OSABI field.
  unsigned char osabi_;
  // ELF file header EI_ABIVERSION field.
  unsigned char abiversion_;
  // ELF file header e_machine field.
  elfcpp::Elf_Half machine_;
  // ELF file header e_flags field.
  unsigned int flags_;
  // File offset of section header table.
  off_t shoff_;
  // Number of input sections.
  unsigned int shnum_;
  // Offset of SHT_STRTAB section holding section names.
  unsigned int shstrndx_;
  // Index of SHT_SYMTAB section.
  unsigned int symtab_shnum_;
  // The entries in the symbol table for the external symbols.
  Symbol** symbols_;
};

// Return an Object appropriate for the input file.  P is BYTES long,
// and holds the ELF header.

extern Object* make_elf_object(const std::string& name, Input_file*,
			       off_t offset, const unsigned char* p,
			       off_t bytes);

} // end namespace gold

#endif // !defined(GOLD_OBJECT_H)
