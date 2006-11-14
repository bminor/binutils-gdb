// dynobj.h -- dynamic object support for gold   -*- C++ -*-

#ifndef GOLD_DYNOBJ_H
#define GOLD_DYNOBJ_H

#include <vector>

#include "object.h"

namespace gold
{

// A dynamic object (ET_DYN).  This is an abstract base class itself.
// The implementations is the template class Sized_dynobj.

class Dynobj : public Object
{
 public:
  Dynobj(const std::string& name, Input_file* input_file, off_t offset = 0)
    : Object(name, input_file, true, offset)
  { }
};

// A dynamic object, size and endian specific version.

template<int size, bool big_endian>
class Sized_dynobj : public Dynobj
{
 public:
  Sized_dynobj(const std::string& name, Input_file* input_file, off_t offset,
	       const typename elfcpp::Ehdr<size, big_endian>&);

  // Set up the object file based on the ELF header.
  void
  setup(const typename elfcpp::Ehdr<size, big_endian>&);

  // Read the symbols.
  void
  do_read_symbols(Read_symbols_data*);

  // Lay out the input sections.
  void
  do_layout(const General_options&, Symbol_table*, Layout*,
	    Read_symbols_data*);

  // Add the symbols to the symbol table.
  void
  do_add_symbols(Symbol_table*, Read_symbols_data*);

  // Get the name of a section.
  std::string
  do_section_name(unsigned int shndx)
  { return this->elf_file_.section_name(shndx); }

  // Return a view of the contents of a section.  Set *PLEN to the
  // size.
  Object::Location
  do_section_contents(unsigned int shndx)
  { return this->elf_file_.section_contents(shndx); }

 private:
  // For convenience.
  typedef Sized_dynobj<size, big_endian> This;
  static const int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;
  static const int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  static const int dyn_size = elfcpp::Elf_sizes<size>::dyn_size;
  typedef elfcpp::Shdr<size, big_endian> Shdr;
  typedef elfcpp::Dyn<size, big_endian> Dyn;

  // Find the dynamic symbol table and the version sections, given the
  // section headers.
  void
  find_dynsym_sections(const unsigned char* pshdrs,
		       unsigned int* pdynshm_shndx,
		       unsigned int* pversym_shndx,
		       unsigned int* pverdef_shndx,
		       unsigned int* pverneed_shndx,
		       unsigned int* pdynamic_shndx);

  // Read the dynamic symbol section SHNDX.
  void
  read_dynsym_section(const unsigned char* pshdrs, unsigned int shndx,
		      elfcpp::SHT type, unsigned int link,
		      File_view** view, off_t* view_size,
		      unsigned int* view_info);

  // Set the SONAME from the SHT_DYNAMIC section at DYNAMIC_SHNDX.
  // The STRTAB parameters may have the relevant string table.
  void
  set_soname(const unsigned char* pshdrs, unsigned int dynamic_shndx,
	     unsigned int strtab_shndx, const unsigned char* strtabu,
	     off_t strtab_size);

  // Mapping from version number to version name.
  typedef std::vector<const char*> Version_map;

  // Create the version map.
  void
  make_version_map(Read_symbols_data* sd, Version_map*) const;

  // Add an entry to the version map.
  void
  set_version_map(Version_map*, unsigned int ndx, const char* name) const;

  // General access to the ELF file.
  elfcpp::Elf_file<size, big_endian, Object> elf_file_;
  // The DT_SONAME name, if any.
  std::string soname_;
};

} // End namespace gold.

#endif // !defined(GOLD_DYNOBJ_H)
