// object.cc -- support for an object file for linking in gold

#include "gold.h"

#include <cerrno>
#include <cstring>
#include <cassert>

#include "object.h"
#include "target-select.h"

namespace gold
{

// Class Object.

const unsigned char*
Object::get_view(off_t start, off_t size)
{
  return this->input_file_->file().get_view(start + this->offset_, size);
}

void
Object::read(off_t start, off_t size, void* p)
{
  this->input_file_->file().read(start + this->offset_, size, p);
}

File_view*
Object::get_lasting_view(off_t start, off_t size)
{
  return this->input_file_->file().get_lasting_view(start + this->offset_,
						    size);
}

// Class Sized_object.

template<int size, bool big_endian>
Sized_object<size, big_endian>::Sized_object(
    const std::string& name,
    Input_file* input_file,
    off_t offset,
    const elfcpp::Ehdr<size, big_endian>& ehdr)
  : Object(name, input_file, false, offset),
    osabi_(ehdr.get_e_ident()[elfcpp::EI_OSABI]),
    abiversion_(ehdr.get_e_ident()[elfcpp::EI_ABIVERSION]),
    machine_(ehdr.get_e_machine()),
    flags_(ehdr.get_e_flags()),
    shoff_(ehdr.get_e_shoff()),
    shnum_(0),
    shstrndx_(0),
    symtab_shnum_(0),
    symbols_(NULL)
{
  if (ehdr.get_e_ehsize() != elfcpp::Elf_sizes<size>::ehdr_size)
    {
      fprintf(stderr, _("%s: %s: bad e_ehsize field (%d != %d)\n"),
	      program_name, this->name().c_str(), ehdr.get_e_ehsize(),
	      elfcpp::Elf_sizes<size>::ehdr_size);
      gold_exit(false);
    }
  if (ehdr.get_e_shentsize() != elfcpp::Elf_sizes<size>::shdr_size)
    {
      fprintf(stderr, _("%s: %s: bad e_shentsize field (%d != %d)\n"),
	      program_name, this->name().c_str(), ehdr.get_e_shentsize(),
	      elfcpp::Elf_sizes<size>::shdr_size);
      gold_exit(false);
    }
}

template<int size, bool big_endian>
Sized_object<size, big_endian>::~Sized_object()
{
}

// Set up an object file bsaed on the file header.  This sets up the
// target and reads the section information.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::setup(
    const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  Target* target = select_target(this->machine_, size, big_endian,
				 this->osabi_, this->abiversion_);
  if (target == NULL)
    {
      fprintf(stderr, _("%s: %s: unsupported ELF machine number %d\n"),
	      program_name, this->name().c_str(), this->machine_);
      gold_exit(false);
    }
  this->set_target(target);
  unsigned int shnum = ehdr.get_e_shnum();
  unsigned int shstrndx = ehdr.get_e_shstrndx();
  if ((shnum == 0 || shstrndx == elfcpp::SHN_XINDEX)
      && this->shoff_ != 0)
    {
      const unsigned char* p = this->get_view
	(this->shoff_, elfcpp::Elf_sizes<size>::shdr_size);
      elfcpp::Shdr<size, big_endian> shdr(p);
      if (shnum == 0)
	shnum = shdr.get_sh_size();
      if (shstrndx == elfcpp::SHN_XINDEX)
	shstrndx = shdr.get_sh_link();
    }
  this->shnum_ = shnum;
  this->shstrndx_ = shstrndx;

  if (shnum == 0)
    return;

  // Find the SHT_SYMTAB section.
  const unsigned char* p = this->get_view
    (this->shoff_, shnum * elfcpp::Elf_sizes<size>::shdr_size);
  // Skip the first section, which is always empty.
  p += elfcpp::Elf_sizes<size>::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i)
    {
      elfcpp::Shdr<size, big_endian> shdr(p);
      if (shdr.get_sh_type() == elfcpp::SHT_SYMTAB)
	{
	  this->symtab_shnum_ = i;
	  break;
	}
      p += elfcpp::Elf_sizes<size>::shdr_size;
    }
}

// Read the symbols and relocations from an object file.

template<int size, bool big_endian>
Read_symbols_data
Sized_object<size, big_endian>::do_read_symbols()
{
  if (this->symtab_shnum_ == 0)
    {
      // No symbol table.  Weird but legal.
      Read_symbols_data ret;
      ret.symbols = NULL;
      ret.symbols_size = 0;
      ret.symbol_names = NULL;
      ret.symbol_names_size = 0;
      return ret;
    }

  int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;

  // Read the symbol table section header.
  off_t symtabshdroff = this->shoff_ + (this->symtab_shnum_ * shdr_size);
  const unsigned char* psymtabshdr = this->get_view(symtabshdroff, shdr_size);
  elfcpp::Shdr<size, big_endian> symtabshdr(psymtabshdr);
  assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // We only need the external symbols.
  int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  off_t locsize = symtabshdr.get_sh_info() * sym_size;
  off_t extoff = symtabshdr.get_sh_offset() + locsize;
  off_t extsize = symtabshdr.get_sh_size() - locsize;

  // Read the symbol table.
  File_view* fvsymtab = this->get_lasting_view(extoff, extsize);

  // Read the section header for the symbol names.
  unsigned int strtab_shnum = symtabshdr.get_sh_link();
  if (strtab_shnum == 0 || strtab_shnum >= this->shnum_)
    {
      fprintf(stderr, _("%s: %s: invalid symbol table name index: %u\n"),
	      program_name, this->name().c_str(), strtab_shnum);
      gold_exit(false);
    }
  off_t strtabshdroff = this->shoff_ + (strtab_shnum * shdr_size);
  const unsigned char *pstrtabshdr = this->get_view(strtabshdroff, shdr_size);
  elfcpp::Shdr<size, big_endian> strtabshdr(pstrtabshdr);
  if (strtabshdr.get_sh_type() != elfcpp::SHT_STRTAB)
    {
      fprintf(stderr,
	      _("%s: %s: symbol table name section has wrong type: %u\n"),
	      program_name, this->name().c_str(),
	      static_cast<unsigned int>(strtabshdr.get_sh_type()));
      gold_exit(false);
    }

  // Read the symbol names.
  File_view* fvstrtab = this->get_lasting_view(strtabshdr.get_sh_offset(),
					       strtabshdr.get_sh_size());

  Read_symbols_data ret;
  ret.symbols = fvsymtab;
  ret.symbols_size = extsize;
  ret.symbol_names = fvstrtab;
  ret.symbol_names_size = strtabshdr.get_sh_size();

  return ret;
}

// Add the symbols to the symbol table.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::do_add_symbols(Symbol_table* symtab,
					       Read_symbols_data sd)
{
  if (sd.symbols == NULL)
    {
      assert(sd.symbol_names == NULL);
      return;
    }

  unsigned int sym_size = elfcpp::Elf_sizes<size>::sym_size;
  size_t symcount = sd.symbols_size / sym_size;
  if (symcount * sym_size != sd.symbols_size)
    {
      fprintf(stderr,
	      _("%s: %s: size of symbols is not multiple of symbol size\n"),
	      program_name, this->name().c_str());
      gold_exit(false);
    }

  this->symbols_ = new Symbol*[symcount];

  const elfcpp::Sym<size, big_endian>* syms =
    reinterpret_cast<const elfcpp::Sym<size, big_endian>*>(sd.symbols->data());
  const char* sym_names =
    reinterpret_cast<const char*>(sd.symbol_names->data());
  symtab->add_from_object(this, syms, symcount, sym_names, 
			  sd.symbol_names_size,  this->symbols_);
}

} // End namespace gold.

namespace
{

using namespace gold;

// Read an ELF file with the header and return the appropriate
// instance of Object.

template<int size, bool big_endian>
Object*
make_elf_sized_object(const std::string& name, Input_file* input_file,
		      off_t offset, const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  int et = ehdr.get_e_type();
  if (et != elfcpp::ET_REL && et != elfcpp::ET_DYN)
    {
      fprintf(stderr, "%s: %s: unsupported ELF type %d\n",
	      program_name, name.c_str(), static_cast<int>(et));
      gold_exit(false);
    }

  if (et == elfcpp::ET_REL)
    {
      Sized_object<size, big_endian>* obj =
	new Sized_object<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup(ehdr);
      return obj;
    }
  else
    {
      // elfcpp::ET_DYN
      fprintf(stderr, _("%s: %s: dynamic objects are not yet supported\n"),
	      program_name, name.c_str());
      gold_exit(false);
//       Sized_dynobj<size, big_endian>* obj =
// 	new Sized_dynobj<size, big_endian>(this->input_.name(), input_file,
// 					   offset, ehdr);
//       obj->setup(ehdr);
//       return obj;
    }
}

} // End anonymous namespace.

namespace gold
{

// Read an ELF file and return the appropriate instance of Object.

Object*
make_elf_object(const std::string& name, Input_file* input_file, off_t offset,
		const unsigned char* p, off_t bytes)
{
  if (bytes < elfcpp::EI_NIDENT)
    {
      fprintf(stderr, _("%s: %s: ELF file too short\n"),
	      program_name, name.c_str());
      gold_exit(false);
    }

  int v = p[elfcpp::EI_VERSION];
  if (v != elfcpp::EV_CURRENT)
    {
      if (v == elfcpp::EV_NONE)
	fprintf(stderr, _("%s: %s: invalid ELF version 0\n"),
		program_name, name.c_str());
      else
	fprintf(stderr, _("%s: %s: unsupported ELF version %d\n"),
		program_name, name.c_str(), v);
      gold_exit(false);
    }

  int c = p[elfcpp::EI_CLASS];
  if (c == elfcpp::ELFCLASSNONE)
    {
      fprintf(stderr, _("%s: %s: invalid ELF class 0\n"),
	      program_name, name.c_str());
      gold_exit(false);
    }
  else if (c != elfcpp::ELFCLASS32
	   && c != elfcpp::ELFCLASS64)
    {
      fprintf(stderr, _("%s: %s: unsupported ELF class %d\n"),
	      program_name, name.c_str(), c);
      gold_exit(false);
    }

  int d = p[elfcpp::EI_DATA];
  if (d == elfcpp::ELFDATANONE)
    {
      fprintf(stderr, _("%s: %s: invalid ELF data encoding\n"),
	      program_name, name.c_str());
      gold_exit(false);
    }
  else if (d != elfcpp::ELFDATA2LSB
	   && d != elfcpp::ELFDATA2MSB)
    {
      fprintf(stderr, _("%s: %s: unsupported ELF data encoding %d\n"),
	      program_name, name.c_str(), d);
      gold_exit(false);
    }

  bool big_endian = d == elfcpp::ELFDATA2MSB;

  if (c == elfcpp::ELFCLASS32)
    {
      if (bytes < elfcpp::Elf_sizes<32>::ehdr_size)
	{
	  fprintf(stderr, _("%s: %s: ELF file too short\n"),
		  program_name, name.c_str());
	  gold_exit(false);
	}
      if (big_endian)
	{
	  elfcpp::Ehdr<32, true> ehdr(p);
	  return make_elf_sized_object<32, true>(name, input_file,
						 offset, ehdr);
	}
      else
	{
	  elfcpp::Ehdr<32, false> ehdr(p);
	  return make_elf_sized_object<32, false>(name, input_file,
						  offset, ehdr);
	}
    }
  else
    {
      if (bytes < elfcpp::Elf_sizes<32>::ehdr_size)
	{
	  fprintf(stderr, _("%s: %s: ELF file too short\n"),
		  program_name, name.c_str());
	  gold_exit(false);
	}
      if (big_endian)
	{
	  elfcpp::Ehdr<64, true> ehdr(p);
	  return make_elf_sized_object<64, true>(name, input_file,
						 offset, ehdr);
	}
      else
	{
	  elfcpp::Ehdr<64, false> ehdr(p);
	  return make_elf_sized_object<64, false>(name, input_file,
						  offset, ehdr);
	}
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

template
class Sized_object<32, false>;

template
class Sized_object<32, true>;

template
class Sized_object<64, false>;

template
class Sized_object<64, true>;

} // End namespace gold.
