// object.cc -- support for an object file for linking in gold

#include "gold.h"

#include <cerrno>
#include <cstring>
#include <cassert>

#include "object.h"
#include "target-select.h"
#include "layout.h"
#include "output.h"

namespace gold
{

// Class Object.

// Class Sized_object.

template<int size, bool big_endian>
Sized_object<size, big_endian>::Sized_object(
    const std::string& name,
    Input_file* input_file,
    off_t offset,
    const elfcpp::Ehdr<size, big_endian>& ehdr)
  : Object(name, input_file, false, offset),
    section_headers_(NULL),
    flags_(ehdr.get_e_flags()),
    shoff_(ehdr.get_e_shoff()),
    shstrndx_(0),
    symtab_shnum_(0),
    local_symbol_count_(0),
    output_local_symbol_count_(0),
    symbols_(NULL),
    local_symbol_offset_(0),
    values_(NULL)
{
  if (ehdr.get_e_ehsize() != This::ehdr_size)
    {
      fprintf(stderr, _("%s: %s: bad e_ehsize field (%d != %d)\n"),
	      program_name, this->name().c_str(), ehdr.get_e_ehsize(),
	      This::ehdr_size);
      gold_exit(false);
    }
  if (ehdr.get_e_shentsize() != This::shdr_size)
    {
      fprintf(stderr, _("%s: %s: bad e_shentsize field (%d != %d)\n"),
	      program_name, this->name().c_str(), ehdr.get_e_shentsize(),
	      This::shdr_size);
      gold_exit(false);
    }
}

template<int size, bool big_endian>
Sized_object<size, big_endian>::~Sized_object()
{
}

// Read the section header for section SHNUM.

template<int size, bool big_endian>
inline const unsigned char*
Sized_object<size, big_endian>::section_header(unsigned int shnum)
{
  assert(shnum < this->shnum());
  off_t symtabshdroff = this->shoff_ + shnum * This::shdr_size;
  return this->get_view(symtabshdroff, This::shdr_size);
}

// Return the name of section SHNUM.

template<int size, bool big_endian>
std::string
Sized_object<size, big_endian>::do_section_name(unsigned int shnum)
{
  Task_lock_obj<Object> tl(*this);

  // Read the section names.
  typename This::Shdr shdrnames(this->section_header(this->shstrndx_));
  const unsigned char* pnamesu = this->get_view(shdrnames.get_sh_offset(),
						shdrnames.get_sh_size());
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  typename This::Shdr shdr(this->section_header(shnum));
  if (shdr.get_sh_name() >= shdrnames.get_sh_size())
    {
      fprintf(stderr,
	      _("%s: %s: bad section name offset for section %u: %lu\n"),
	      program_name, this->name().c_str(), shnum,
	      static_cast<unsigned long>(shdr.get_sh_name()));
      gold_exit(false);
    }

  return std::string(pnames + shdr.get_sh_name());
}

// Set up an object file bsaed on the file header.  This sets up the
// target and reads the section information.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::setup(
    const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  int machine = ehdr.get_e_machine();
  Target* target = select_target(machine, size, big_endian,
				 ehdr.get_e_ident()[elfcpp::EI_OSABI],
				 ehdr.get_e_ident()[elfcpp::EI_ABIVERSION]);
  if (target == NULL)
    {
      fprintf(stderr, _("%s: %s: unsupported ELF machine number %d\n"),
	      program_name, this->name().c_str(), machine);
      gold_exit(false);
    }
  this->set_target(target);

  unsigned int shnum = ehdr.get_e_shnum();
  unsigned int shstrndx = ehdr.get_e_shstrndx();
  if ((shnum == 0 || shstrndx == elfcpp::SHN_XINDEX)
      && this->shoff_ != 0)
    {
      typename This::Shdr shdr(this->section_header(0));
      if (shnum == 0)
	shnum = shdr.get_sh_size();
      if (shstrndx == elfcpp::SHN_XINDEX)
	shstrndx = shdr.get_sh_link();
    }
  this->set_shnum(shnum);
  this->shstrndx_ = shstrndx;

  if (shnum == 0)
    return;

  // We store the section headers in a File_view until do_read_symbols.
  this->section_headers_ = this->get_lasting_view(this->shoff_,
						  shnum * This::shdr_size);

  // Find the SHT_SYMTAB section.  The ELF standard says that maybe in
  // the future there can be more than one SHT_SYMTAB section.  Until
  // somebody figures out how that could work, we assume there is only
  // one.
  const unsigned char* p = this->section_headers_->data();

  // Skip the first section, which is always empty.
  p += This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, p += This::shdr_size)
    {
      typename This::Shdr shdr(p);
      if (shdr.get_sh_type() == elfcpp::SHT_SYMTAB)
	{
	  this->symtab_shnum_ = i;
	  break;
	}
    }
}

// Read the sections and symbols from an object file.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  // Transfer our view of the section headers to SD.
  sd->section_headers = this->section_headers_;
  this->section_headers_ = NULL;

  // Read the section names.
  const unsigned char* pshdrs = sd->section_headers->data();
  const unsigned char* pshdrnames = pshdrs + this->shstrndx_ * This::shdr_size;
  typename This::Shdr shdrnames(pshdrnames);
  sd->section_names_size = shdrnames.get_sh_size();
  sd->section_names = this->get_lasting_view(shdrnames.get_sh_offset(),
					     sd->section_names_size);

  if (this->symtab_shnum_ == 0)
    {
      // No symbol table.  Weird but legal.
      sd->symbols = NULL;
      sd->symbols_size = 0;
      sd->symbol_names = NULL;
      sd->symbol_names_size = 0;
      return;
    }

  // Get the symbol table section header.
  typename This::Shdr symtabshdr(pshdrs
				 + this->symtab_shnum_ * This::shdr_size);
  assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // We only need the external symbols.
  const int sym_size = This::sym_size;
  const unsigned int loccount = symtabshdr.get_sh_info();
  this->local_symbol_count_ = loccount;
  off_t locsize = loccount * sym_size;
  off_t extoff = symtabshdr.get_sh_offset() + locsize;
  off_t extsize = symtabshdr.get_sh_size() - locsize;

  // Read the symbol table.
  File_view* fvsymtab = this->get_lasting_view(extoff, extsize);

  // Read the section header for the symbol names.
  unsigned int shnum = this->shnum();
  unsigned int strtab_shnum = symtabshdr.get_sh_link();
  if (strtab_shnum == 0 || strtab_shnum >= shnum)
    {
      fprintf(stderr, _("%s: %s: invalid symbol table name index: %u\n"),
	      program_name, this->name().c_str(), strtab_shnum);
      gold_exit(false);
    }
  typename This::Shdr strtabshdr(pshdrs + strtab_shnum * This::shdr_size);
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

  sd->symbols = fvsymtab;
  sd->symbols_size = extsize;
  sd->symbol_names = fvstrtab;
  sd->symbol_names_size = strtabshdr.get_sh_size();
}

// Return whether to include a section group in the link.  LAYOUT is
// used to keep track of which section groups we have already seen.
// INDEX is the index of the section group and SHDR is the section
// header.  If we do not want to include this group, we set bits in
// OMIT for each section which should be discarded.

template<int size, bool big_endian>
bool
Sized_object<size, big_endian>::include_section_group(
    Layout* layout,
    unsigned int index,
    const elfcpp::Shdr<size, big_endian>& shdr,
    std::vector<bool>* omit)
{
  // Read the section contents.
  const unsigned char* pcon = this->get_view(shdr.get_sh_offset(),
					     shdr.get_sh_size());
  const elfcpp::Elf_Word* pword =
    reinterpret_cast<const elfcpp::Elf_Word*>(pcon);

  // The first word contains flags.  We only care about COMDAT section
  // groups.  Other section groups are always included in the link
  // just like ordinary sections.
  elfcpp::Elf_Word flags = elfcpp::read_elf_word<big_endian>(pword);
  if ((flags & elfcpp::GRP_COMDAT) == 0)
    return true;

  // Look up the group signature, which is the name of a symbol.  This
  // is a lot of effort to go to to read a string.  Why didn't they
  // just use the name of the SHT_GROUP section as the group
  // signature?

  // Get the appropriate symbol table header (this will normally be
  // the single SHT_SYMTAB section, but in principle it need not be).
  if (shdr.get_sh_link() >= this->shnum())
    {
      fprintf(stderr, _("%s: %s: section group %u link %u out of range\n"),
	      program_name, this->name().c_str(), index, shdr.get_sh_link());
      gold_exit(false);
    }

  typename This::Shdr symshdr(this->section_header(shdr.get_sh_link()));

  // Read the symbol table entry.
  if (shdr.get_sh_info() >= symshdr.get_sh_size() / This::sym_size)
    {
      fprintf(stderr, _("%s: %s: section group %u info %u out of range\n"),
	      program_name, this->name().c_str(), index, shdr.get_sh_info());
      gold_exit(false);
    }
  off_t symoff = symshdr.get_sh_offset() + shdr.get_sh_info() * This::sym_size;
  const unsigned char* psym = this->get_view(symoff, This::sym_size);
  elfcpp::Sym<size, big_endian> sym(psym);

  // Read the section header for the symbol table names.
  if (symshdr.get_sh_link() >= this->shnum())
    {
      fprintf(stderr, _("%s; %s: symtab section %u link %u out of range\n"),
	      program_name, this->name().c_str(), shdr.get_sh_link(),
	      symshdr.get_sh_link());
      gold_exit(false);
    }

  typename This::Shdr symnamehdr(this->section_header(symshdr.get_sh_link()));

  // Read the symbol table names.
  const unsigned char *psymnamesu = this->get_view(symnamehdr.get_sh_offset(),
						   symnamehdr.get_sh_size());
  const char* psymnames = reinterpret_cast<const char*>(psymnamesu);

  // Get the section group signature.
  if (sym.get_st_name() >= symnamehdr.get_sh_size())
    {
      fprintf(stderr, _("%s: %s: symbol %u name offset %u out of range\n"),
	      program_name, this->name().c_str(), shdr.get_sh_info(),
	      sym.get_st_name());
      gold_exit(false);
    }

  const char* signature = psymnames + sym.get_st_name();

  // It seems that some versions of gas will create a section group
  // associated with a section symbol, and then fail to give a name to
  // the section symbol.  In such a case, use the name of the section.
  // FIXME.
  if (signature[0] == '\0'
      && sym.get_st_type() == elfcpp::STT_SECTION
      && sym.get_st_shndx() < this->shnum())
    {
      typename This::Shdr shdrnames(this->section_header(this->shstrndx_));
      const unsigned char* pnamesu = this->get_view(shdrnames.get_sh_offset(),
						    shdrnames.get_sh_size());
      const char* pnames = reinterpret_cast<const char*>(pnamesu);
      
      typename This::Shdr sechdr(this->section_header(sym.get_st_shndx()));
      if (sechdr.get_sh_name() >= shdrnames.get_sh_size())
	{
	  fprintf(stderr,
		  _("%s: %s: bad section name offset for section %u: %lu\n"),
		  program_name, this->name().c_str(), sym.get_st_shndx(),
		  static_cast<unsigned long>(sechdr.get_sh_name()));
	  gold_exit(false);
	}

      signature = pnames + sechdr.get_sh_name();
    }

  // Record this section group, and see whether we've already seen one
  // with the same signature.
  if (layout->add_comdat(signature, true))
    return true;

  // This is a duplicate.  We want to discard the sections in this
  // group.
  size_t count = shdr.get_sh_size() / sizeof(elfcpp::Elf_Word);
  for (size_t i = 1; i < count; ++i)
    {
      elfcpp::Elf_Word secnum = elfcpp::read_elf_word<big_endian>(pword + i);
      if (secnum >= this->shnum())
	{
	  fprintf(stderr,
		  _("%s: %s: section %u in section group %u out of range"),
		  program_name, this->name().c_str(), secnum,
		  index);
	  gold_exit(false);
	}
      (*omit)[secnum] = true;
    }

  return false;
}

// Whether to include a linkonce section in the link.  NAME is the
// name of the section and SHDR is the section header.

// Linkonce sections are a GNU extension implemented in the original
// GNU linker before section groups were defined.  The semantics are
// that we only include one linkonce section with a given name.  The
// name of a linkonce section is normally .gnu.linkonce.T.SYMNAME,
// where T is the type of section and SYMNAME is the name of a symbol.
// In an attempt to make linkonce sections interact well with section
// groups, we try to identify SYMNAME and use it like a section group
// signature.  We want to block section groups with that signature,
// but not other linkonce sections with that signature.  We also use
// the full name of the linkonce section as a normal section group
// signature.

template<int size, bool big_endian>
bool
Sized_object<size, big_endian>::include_linkonce_section(
    Layout* layout,
    const char* name,
    const elfcpp::Shdr<size, big_endian>&)
{
  const char* symname = strrchr(name, '.') + 1;
  bool include1 = layout->add_comdat(symname, false);
  bool include2 = layout->add_comdat(name, true);
  return include1 && include2;
}

// Lay out the input sections.  We walk through the sections and check
// whether they should be included in the link.  If they should, we
// pass them to the Layout object, which will return an output section
// and an offset.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::do_layout(Layout* layout,
					  Read_symbols_data* sd)
{
  unsigned int shnum = this->shnum();
  if (shnum == 0)
    return;

  // Get the section headers.
  const unsigned char* pshdrs = sd->section_headers->data();

  // Get the section names.
  const unsigned char* pnamesu = sd->section_names->data();
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  std::vector<Map_to_output>& map_sections(this->map_to_output());
  map_sections.resize(shnum);

  // Keep track of which sections to omit.
  std::vector<bool> omit(shnum, false);

  for (unsigned int i = 0; i < shnum; ++i, pshdrs += This::shdr_size)
    {
      typename This::Shdr shdr(pshdrs);

      if (shdr.get_sh_name() >= sd->section_names_size)
	{
	  fprintf(stderr,
		  _("%s: %s: bad section name offset for section %u: %lu\n"),
		  program_name, this->name().c_str(), i,
		  static_cast<unsigned long>(shdr.get_sh_name()));
	  gold_exit(false);
	}

      const char* name = pnames + shdr.get_sh_name();

      bool discard = omit[i];
      if (!discard)
	{
	  if (shdr.get_sh_type() == elfcpp::SHT_GROUP)
	    {
	      if (!this->include_section_group(layout, i, shdr, &omit))
		discard = true;
	    }
	  else if (Layout::is_linkonce(name))
	    {
	      if (!this->include_linkonce_section(layout, name, shdr))
		discard = true;
	    }
	}

      if (discard)
	{
	  // Do not include this section in the link.
	  map_sections[i].output_section = NULL;
	  continue;
	}

      off_t offset;
      Output_section* os = layout->layout(this, i, name, shdr, &offset);

      map_sections[i].output_section = os;
      map_sections[i].offset = offset;
    }

  delete sd->section_headers;
  sd->section_headers = NULL;
  delete sd->section_names;
  sd->section_names = NULL;
}

// Add the symbols to the symbol table.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::do_add_symbols(Symbol_table* symtab,
					       Read_symbols_data* sd)
{
  if (sd->symbols == NULL)
    {
      assert(sd->symbol_names == NULL);
      return;
    }

  const int sym_size = This::sym_size;
  size_t symcount = sd->symbols_size / sym_size;
  if (symcount * sym_size != sd->symbols_size)
    {
      fprintf(stderr,
	      _("%s: %s: size of symbols is not multiple of symbol size\n"),
	      program_name, this->name().c_str());
      gold_exit(false);
    }

  this->symbols_ = new Symbol*[symcount];

  const unsigned char* psyms = sd->symbols->data();
  const elfcpp::Sym<size, big_endian>* syms =
    reinterpret_cast<const elfcpp::Sym<size, big_endian>*>(psyms);
  const char* sym_names =
    reinterpret_cast<const char*>(sd->symbol_names->data());
  symtab->add_from_object(this, syms, symcount, sym_names, 
			  sd->symbol_names_size,  this->symbols_);

  delete sd->symbols;
  sd->symbols = NULL;
  delete sd->symbol_names;
  sd->symbol_names = NULL;
}

// Finalize the local symbols.  Here we record the file offset at
// which they should be output, we add their names to *POOL, and we
// add their values to THIS->VALUES_.  Return the new file offset.
// This function is always called from the main thread.  The actual
// output of the local symbols will occur in a separate task.

template<int size, bool big_endian>
off_t
Sized_object<size, big_endian>::do_finalize_local_symbols(off_t off,
							  Stringpool* pool)
{
  if (this->symtab_shnum_ == 0)
    {
      // This object has no symbols.  Weird but legal.
      return off;
    }

  off = align_address(off, size >> 3);

  this->local_symbol_offset_ = off;

  // Read the symbol table section header.
  typename This::Shdr symtabshdr(this->section_header(this->symtab_shnum_));
  assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // Read the local symbols.
  const int sym_size = This::sym_size;
  const unsigned int loccount = this->local_symbol_count_;
  assert(loccount == symtabshdr.get_sh_info());
  off_t locsize = loccount * sym_size;
  const unsigned char* psyms = this->get_view(symtabshdr.get_sh_offset(),
					      locsize);

  this->values_ = new typename elfcpp::Elf_types<size>::Elf_Addr[loccount];

  // Read the section header for the symbol names.
  typename This::Shdr strtabshdr(
    this->section_header(symtabshdr.get_sh_link()));
  assert(strtabshdr.get_sh_type() == elfcpp::SHT_STRTAB);

  // Read the symbol names.
  const unsigned char* pnamesu = this->get_view(strtabshdr.get_sh_offset(),
						strtabshdr.get_sh_size());
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  // Loop over the local symbols.

  std::vector<Map_to_output>& mo(this->map_to_output());
  unsigned int shnum = this->shnum();
  unsigned int count = 0;
  // Skip the first, dummy, symbol.
  psyms += sym_size;
  for (unsigned int i = 1; i < loccount; ++i, psyms += sym_size)
    {
      elfcpp::Sym<size, big_endian> sym(psyms);

      unsigned int shndx = sym.get_st_shndx();

      if (shndx >= elfcpp::SHN_LORESERVE)
	{
	  if (shndx == elfcpp::SHN_ABS)
	    this->values_[i] = sym.get_st_value();
	  else
	    {
	      // FIXME: Handle SHN_XINDEX.
	      fprintf(stderr,
		      _("%s: %s: unknown section index %u "
			"for local symbol %u\n"),
		      program_name, this->name().c_str(), shndx, i);
	      gold_exit(false);
	    }
	}
      else
	{
	  if (shndx >= shnum)
	    {
	      fprintf(stderr,
		      _("%s: %s: local symbol %u section index %u "
			"out of range\n"),
		      program_name, this->name().c_str(), i, shndx);
	      gold_exit(false);
	    }

	  if (mo[shndx].output_section == NULL)
	    {
	      this->values_[i] = 0;
	      continue;
	    }

	  this->values_[i] = (mo[shndx].output_section->address()
			      + mo[shndx].offset
			      + sym.get_st_value());
	}

      pool->add(pnames + sym.get_st_name());
      off += sym_size;
      ++count;
    }

  this->output_local_symbol_count_ = count;

  return off;
}

// Write out the local symbols.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::write_local_symbols(Output_file* of,
						    const Stringpool* sympool)
{
  if (this->symtab_shnum_ == 0)
    {
      // This object has no symbols.  Weird but legal.
      return;
    }

  // Read the symbol table section header.
  typename This::Shdr symtabshdr(this->section_header(this->symtab_shnum_));
  assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);
  const unsigned int loccount = this->local_symbol_count_;
  assert(loccount == symtabshdr.get_sh_info());

  // Read the local symbols.
  const int sym_size = This::sym_size;
  off_t locsize = loccount * sym_size;
  const unsigned char* psyms = this->get_view(symtabshdr.get_sh_offset(),
					      locsize);

  // Read the section header for the symbol names.
  typename This::Shdr strtabshdr(
    this->section_header(symtabshdr.get_sh_link()));
  assert(strtabshdr.get_sh_type() == elfcpp::SHT_STRTAB);

  // Read the symbol names.
  const unsigned char* pnamesu = this->get_view(strtabshdr.get_sh_offset(),
						strtabshdr.get_sh_size());
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  // Get a view into the output file.
  off_t output_size = this->output_local_symbol_count_ * sym_size;
  unsigned char* oview = of->get_output_view(this->local_symbol_offset_,
					     output_size);

  std::vector<Map_to_output>& mo(this->map_to_output());

  psyms += sym_size;
  unsigned char* ov = oview;
  for (unsigned int i = 1; i < loccount; ++i, psyms += sym_size)
    {
      elfcpp::Sym<size, big_endian> isym(psyms);
      elfcpp::Sym_write<size, big_endian> osym(ov);

      unsigned int st_shndx = isym.get_st_shndx();
      if (st_shndx < elfcpp::SHN_LORESERVE)
	{
	  assert(st_shndx < mo.size());
	  if (mo[st_shndx].output_section == NULL)
	    continue;
	  st_shndx = mo[st_shndx].output_section->out_shndx();
	}

      osym.put_st_name(sympool->get_offset(pnames + isym.get_st_name()));
      osym.put_st_value(this->values_[i]);
      osym.put_st_size(isym.get_st_size());
      osym.put_st_info(isym.get_st_info());
      osym.put_st_other(isym.get_st_other());
      osym.put_st_shndx(st_shndx);

      ov += sym_size;
    }

  assert(ov - oview == output_size);

  of->write_output_view(this->local_symbol_offset_, output_size, oview);
}

// Input_objects methods.

void
Input_objects::add_object(Object* obj)
{
  this->object_list_.push_back(obj);

  Target* target = obj->target();
  if (this->target_ == NULL)
    this->target_ = target;
  else if (this->target_ != target)
    {
      fprintf(stderr, "%s: %s: incompatible target\n",
	      program_name, obj->name().c_str());
      gold_exit(false);
    }

  if (obj->is_dynamic())
    this->any_dynamic_ = true;
}

// Relocate_info methods.

// Return a string describing the location of a relocation.  This is
// only used in error messages.

template<int size, bool big_endian>
std::string
Relocate_info<size, big_endian>::location(size_t relnum, off_t) const
{
  std::string ret(this->object->name());
  ret += ": reloc ";
  char buf[100];
  snprintf(buf, sizeof buf, "%zu", relnum);
  ret += buf;
  ret += " in reloc section ";
  snprintf(buf, sizeof buf, "%u", this->reloc_shndx);
  ret += buf;
  ret += " (" + this->object->section_name(this->reloc_shndx);
  ret += ") for section ";
  snprintf(buf, sizeof buf, "%u", this->data_shndx);
  ret += buf;
  ret += " (" + this->object->section_name(this->data_shndx) + ")";
  return ret;
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

template
struct Relocate_info<32, false>;

template
struct Relocate_info<32, true>;

template
struct Relocate_info<64, false>;

template
struct Relocate_info<64, true>;

} // End namespace gold.
