// object.h -- support for an object file for linking in gold  -*- C++ -*-

#ifndef GOLD_OBJECT_H
#define GOLD_OBJECT_H

#include "elfcpp.h"
#include "targetsize.h"
#include "target.h"
#include "fileread.h"

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
// object file.  The actual instantiations are Sized_object<32> and
// Sized_object<64>

class Object
{
 public:
  // NAME is the name of the object as we would report it to the user
  // (e.g., libfoo.a(bar.o) if this is in an archive.  INPUT_FILE is
  // used to read the file.  OFFSET is the offset within the input
  // file--0 for a .o or .so file, something else for a .a file.
  Object(const std::string& name, Input_file* input_file, off_t offset = 0)
    : name_(name), input_file_(input_file), offset_(offset)
  { }

  virtual ~Object()
  { }

  const std::string&
  name() const
  { return this->name_; }

  // Read the symbol and relocation information.
  Read_symbols_data
  read_symbols()
  { return this->do_read_symbols(); }

  // Add symbol information to the global symbol table.
  void
  add_symbols(Read_symbols_data rd)
  { this->do_add_symbols(rd); }

 protected:
  // Read the symbols--implemented by child class.
  virtual Read_symbols_data
  do_read_symbols() = 0;

  // Add symbol information to the global symbol table--implemented by
  // child class.
  virtual void
  do_add_symbols(Read_symbols_data) = 0;

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
};

// The functions of Object which are size specific.

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
  do_add_symbols(Read_symbols_data);

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
  // Target functions--may be NULL.
  Target* target_;
  // File offset of section header table.
  off_t shoff_;
  // Number of input sections.
  unsigned int shnum_;
  // Offset of SHT_STRTAB section holding section names.
  unsigned int shstrndx_;
  // Index of SHT_SYMTAB section.
  unsigned int symtab_shnum_;
};

// Return an Object appropriate for the input file.  P is BYTES long,
// and holds the ELF header.

extern Object* make_elf_object(const std::string& name, Input_file*,
			       off_t offset, const unsigned char* p,
			       off_t bytes);

} // end namespace gold

#endif // !defined(GOLD_OBJECT_H)
