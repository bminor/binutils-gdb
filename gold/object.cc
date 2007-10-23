// object.cc -- support for an object file for linking in gold

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

#include "gold.h"

#include <cerrno>
#include <cstring>
#include <cstdarg>

#include "target-select.h"
#include "layout.h"
#include "output.h"
#include "symtab.h"
#include "object.h"
#include "dynobj.h"

namespace gold
{

// Class Object.

// Set the target based on fields in the ELF file header.

void
Object::set_target(int machine, int size, bool big_endian, int osabi,
		   int abiversion)
{
  Target* target = select_target(machine, size, big_endian, osabi, abiversion);
  if (target == NULL)
    gold_fatal(_("%s: unsupported ELF machine number %d"),
	       this->name().c_str(), machine);
  this->target_ = target;
}

// Report an error for this object file.  This is used by the
// elfcpp::Elf_file interface, and also called by the Object code
// itself.

void
Object::error(const char* format, ...) const
{
  va_list args;
  va_start(args, format);
  char* buf = NULL;
  if (vasprintf(&buf, format, args) < 0)
    gold_nomem();
  va_end(args);
  gold_error(_("%s: %s"), this->name().c_str(), buf);
  free(buf);
}

// Return a view of the contents of a section.

const unsigned char*
Object::section_contents(unsigned int shndx, off_t* plen, bool cache)
{
  Location loc(this->do_section_contents(shndx));
  *plen = loc.data_size;
  return this->get_view(loc.file_offset, loc.data_size, cache);
}

// Read the section data into SD.  This is code common to Sized_relobj
// and Sized_dynobj, so we put it into Object.

template<int size, bool big_endian>
void
Object::read_section_data(elfcpp::Elf_file<size, big_endian, Object>* elf_file,
			  Read_symbols_data* sd)
{
  const int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;

  // Read the section headers.
  const off_t shoff = elf_file->shoff();
  const unsigned int shnum = this->shnum();
  sd->section_headers = this->get_lasting_view(shoff, shnum * shdr_size, true);

  // Read the section names.
  const unsigned char* pshdrs = sd->section_headers->data();
  const unsigned char* pshdrnames = pshdrs + elf_file->shstrndx() * shdr_size;
  typename elfcpp::Shdr<size, big_endian> shdrnames(pshdrnames);

  if (shdrnames.get_sh_type() != elfcpp::SHT_STRTAB)
    this->error(_("section name section has wrong type: %u"),
		static_cast<unsigned int>(shdrnames.get_sh_type()));

  sd->section_names_size = shdrnames.get_sh_size();
  sd->section_names = this->get_lasting_view(shdrnames.get_sh_offset(),
					     sd->section_names_size, false);
}

// If NAME is the name of a special .gnu.warning section, arrange for
// the warning to be issued.  SHNDX is the section index.  Return
// whether it is a warning section.

bool
Object::handle_gnu_warning_section(const char* name, unsigned int shndx,
				   Symbol_table* symtab)
{
  const char warn_prefix[] = ".gnu.warning.";
  const int warn_prefix_len = sizeof warn_prefix - 1;
  if (strncmp(name, warn_prefix, warn_prefix_len) == 0)
    {
      symtab->add_warning(name + warn_prefix_len, this, shndx);
      return true;
    }
  return false;
}

// Class Sized_relobj.

template<int size, bool big_endian>
Sized_relobj<size, big_endian>::Sized_relobj(
    const std::string& name,
    Input_file* input_file,
    off_t offset,
    const elfcpp::Ehdr<size, big_endian>& ehdr)
  : Relobj(name, input_file, offset),
    elf_file_(this, ehdr),
    symtab_shndx_(-1U),
    local_symbol_count_(0),
    output_local_symbol_count_(0),
    symbols_(NULL),
    local_symbol_offset_(0),
    local_values_(),
    local_got_offsets_()
{
}

template<int size, bool big_endian>
Sized_relobj<size, big_endian>::~Sized_relobj()
{
}

// Set up an object file based on the file header.  This sets up the
// target and reads the section information.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::setup(
    const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  this->set_target(ehdr.get_e_machine(), size, big_endian,
		   ehdr.get_e_ident()[elfcpp::EI_OSABI],
		   ehdr.get_e_ident()[elfcpp::EI_ABIVERSION]);

  const unsigned int shnum = this->elf_file_.shnum();
  this->set_shnum(shnum);
}

// Find the SHT_SYMTAB section, given the section headers.  The ELF
// standard says that maybe in the future there can be more than one
// SHT_SYMTAB section.  Until somebody figures out how that could
// work, we assume there is only one.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::find_symtab(const unsigned char* pshdrs)
{
  const unsigned int shnum = this->shnum();
  this->symtab_shndx_ = 0;
  if (shnum > 0)
    {
      // Look through the sections in reverse order, since gas tends
      // to put the symbol table at the end.
      const unsigned char* p = pshdrs + shnum * This::shdr_size;
      unsigned int i = shnum;
      while (i > 0)
	{
	  --i;
	  p -= This::shdr_size;
	  typename This::Shdr shdr(p);
	  if (shdr.get_sh_type() == elfcpp::SHT_SYMTAB)
	    {
	      this->symtab_shndx_ = i;
	      break;
	    }
	}
    }
}

// Read the sections and symbols from an object file.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  this->read_section_data(&this->elf_file_, sd);

  const unsigned char* const pshdrs = sd->section_headers->data();

  this->find_symtab(pshdrs);

  sd->symbols = NULL;
  sd->symbols_size = 0;
  sd->symbol_names = NULL;
  sd->symbol_names_size = 0;

  if (this->symtab_shndx_ == 0)
    {
      // No symbol table.  Weird but legal.
      return;
    }

  // Get the symbol table section header.
  typename This::Shdr symtabshdr(pshdrs
				 + this->symtab_shndx_ * This::shdr_size);
  gold_assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // We only need the external symbols.
  const int sym_size = This::sym_size;
  const unsigned int loccount = symtabshdr.get_sh_info();
  this->local_symbol_count_ = loccount;
  off_t locsize = loccount * sym_size;
  off_t extoff = symtabshdr.get_sh_offset() + locsize;
  off_t extsize = symtabshdr.get_sh_size() - locsize;

  // Read the symbol table.
  File_view* fvsymtab = this->get_lasting_view(extoff, extsize, false);

  // Read the section header for the symbol names.
  unsigned int strtab_shndx = symtabshdr.get_sh_link();
  if (strtab_shndx >= this->shnum())
    {
      this->error(_("invalid symbol table name index: %u"), strtab_shndx);
      return;
    }
  typename This::Shdr strtabshdr(pshdrs + strtab_shndx * This::shdr_size);
  if (strtabshdr.get_sh_type() != elfcpp::SHT_STRTAB)
    {
      this->error(_("symbol table name section has wrong type: %u"),
		  static_cast<unsigned int>(strtabshdr.get_sh_type()));
      return;
    }

  // Read the symbol names.
  File_view* fvstrtab = this->get_lasting_view(strtabshdr.get_sh_offset(),
					       strtabshdr.get_sh_size(), true);

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
Sized_relobj<size, big_endian>::include_section_group(
    Layout* layout,
    unsigned int index,
    const elfcpp::Shdr<size, big_endian>& shdr,
    std::vector<bool>* omit)
{
  // Read the section contents.
  const unsigned char* pcon = this->get_view(shdr.get_sh_offset(),
					     shdr.get_sh_size(), false);
  const elfcpp::Elf_Word* pword =
    reinterpret_cast<const elfcpp::Elf_Word*>(pcon);

  // The first word contains flags.  We only care about COMDAT section
  // groups.  Other section groups are always included in the link
  // just like ordinary sections.
  elfcpp::Elf_Word flags = elfcpp::Swap<32, big_endian>::readval(pword);
  if ((flags & elfcpp::GRP_COMDAT) == 0)
    return true;

  // Look up the group signature, which is the name of a symbol.  This
  // is a lot of effort to go to to read a string.  Why didn't they
  // just use the name of the SHT_GROUP section as the group
  // signature?

  // Get the appropriate symbol table header (this will normally be
  // the single SHT_SYMTAB section, but in principle it need not be).
  const unsigned int link = shdr.get_sh_link();
  typename This::Shdr symshdr(this, this->elf_file_.section_header(link));

  // Read the symbol table entry.
  if (shdr.get_sh_info() >= symshdr.get_sh_size() / This::sym_size)
    {
      this->error(_("section group %u info %u out of range"),
		  index, shdr.get_sh_info());
      return false;
    }
  off_t symoff = symshdr.get_sh_offset() + shdr.get_sh_info() * This::sym_size;
  const unsigned char* psym = this->get_view(symoff, This::sym_size, true);
  elfcpp::Sym<size, big_endian> sym(psym);

  // Read the symbol table names.
  off_t symnamelen;
  const unsigned char* psymnamesu;
  psymnamesu = this->section_contents(symshdr.get_sh_link(), &symnamelen,
				      true);
  const char* psymnames = reinterpret_cast<const char*>(psymnamesu);

  // Get the section group signature.
  if (sym.get_st_name() >= symnamelen)
    {
      this->error(_("symbol %u name offset %u out of range"),
		  shdr.get_sh_info(), sym.get_st_name());
      return false;
    }

  const char* signature = psymnames + sym.get_st_name();

  // It seems that some versions of gas will create a section group
  // associated with a section symbol, and then fail to give a name to
  // the section symbol.  In such a case, use the name of the section.
  // FIXME.
  std::string secname;
  if (signature[0] == '\0' && sym.get_st_type() == elfcpp::STT_SECTION)
    {
      secname = this->section_name(sym.get_st_shndx());
      signature = secname.c_str();
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
      elfcpp::Elf_Word secnum =
	elfcpp::Swap<32, big_endian>::readval(pword + i);
      if (secnum >= this->shnum())
	{
	  this->error(_("section %u in section group %u out of range"),
		      secnum, index);
	  continue;
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
Sized_relobj<size, big_endian>::include_linkonce_section(
    Layout* layout,
    const char* name,
    const elfcpp::Shdr<size, big_endian>&)
{
  // In general the symbol name we want will be the string following
  // the last '.'.  However, we have to handle the case of
  // .gnu.linkonce.t.__i686.get_pc_thunk.bx, which was generated by
  // some versions of gcc.  So we use a heuristic: if the name starts
  // with ".gnu.linkonce.t.", we use everything after that.  Otherwise
  // we look for the last '.'.  We can't always simply skip
  // ".gnu.linkonce.X", because we have to deal with cases like
  // ".gnu.linkonce.d.rel.ro.local".
  const char* const linkonce_t = ".gnu.linkonce.t.";
  const char* symname;
  if (strncmp(name, linkonce_t, strlen(linkonce_t)) == 0)
    symname = name + strlen(linkonce_t);
  else
    symname = strrchr(name, '.') + 1;
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
Sized_relobj<size, big_endian>::do_layout(Symbol_table* symtab,
					  Layout* layout,
					  Read_symbols_data* sd)
{
  const unsigned int shnum = this->shnum();
  if (shnum == 0)
    return;

  // Get the section headers.
  const unsigned char* pshdrs = sd->section_headers->data();

  // Get the section names.
  const unsigned char* pnamesu = sd->section_names->data();
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  std::vector<Map_to_output>& map_sections(this->map_to_output());
  map_sections.resize(shnum);

  // Whether we've seen a .note.GNU-stack section.
  bool seen_gnu_stack = false;
  // The flags of a .note.GNU-stack section.
  uint64_t gnu_stack_flags = 0;

  // Keep track of which sections to omit.
  std::vector<bool> omit(shnum, false);

  // Skip the first, dummy, section.
  pshdrs += This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, pshdrs += This::shdr_size)
    {
      typename This::Shdr shdr(pshdrs);

      if (shdr.get_sh_name() >= sd->section_names_size)
	{
	  this->error(_("bad section name offset for section %u: %lu"),
		      i, static_cast<unsigned long>(shdr.get_sh_name()));
	  return;
	}

      const char* name = pnames + shdr.get_sh_name();

      if (this->handle_gnu_warning_section(name, i, symtab))
	{
	  if (!parameters->output_is_object())
	    omit[i] = true;
	}

      // The .note.GNU-stack section is special.  It gives the
      // protection flags that this object file requires for the stack
      // in memory.
      if (strcmp(name, ".note.GNU-stack") == 0)
	{
	  seen_gnu_stack = true;
	  gnu_stack_flags |= shdr.get_sh_flags();
	  omit[i] = true;
	}

      bool discard = omit[i];
      if (!discard)
	{
	  if (shdr.get_sh_type() == elfcpp::SHT_GROUP)
	    {
	      if (!this->include_section_group(layout, i, shdr, &omit))
		discard = true;
	    }
          else if ((shdr.get_sh_flags() & elfcpp::SHF_GROUP) == 0
                   && Layout::is_linkonce(name))
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

  layout->layout_gnu_stack(seen_gnu_stack, gnu_stack_flags);

  delete sd->section_headers;
  sd->section_headers = NULL;
  delete sd->section_names;
  sd->section_names = NULL;
}

// Add the symbols to the symbol table.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::do_add_symbols(Symbol_table* symtab,
					       Read_symbols_data* sd)
{
  if (sd->symbols == NULL)
    {
      gold_assert(sd->symbol_names == NULL);
      return;
    }

  const int sym_size = This::sym_size;
  size_t symcount = sd->symbols_size / sym_size;
  if (static_cast<off_t>(symcount * sym_size) != sd->symbols_size)
    {
      this->error(_("size of symbols is not multiple of symbol size"));
      return;
    }

  this->symbols_ = new Symbol*[symcount];

  const char* sym_names =
    reinterpret_cast<const char*>(sd->symbol_names->data());
  symtab->add_from_relobj(this, sd->symbols->data(), symcount, sym_names,
			  sd->symbol_names_size, this->symbols_);

  delete sd->symbols;
  sd->symbols = NULL;
  delete sd->symbol_names;
  sd->symbol_names = NULL;
}

// Finalize the local symbols.  Here we record the file offset at
// which they should be output, we add their names to *POOL, and we
// add their values to THIS->LOCAL_VALUES_.  Return the symbol index.
// This function is always called from the main thread.  The actual
// output of the local symbols will occur in a separate task.

template<int size, bool big_endian>
unsigned int
Sized_relobj<size, big_endian>::do_finalize_local_symbols(unsigned int index,
							  off_t off,
							  Stringpool* pool)
{
  gold_assert(this->symtab_shndx_ != -1U);
  if (this->symtab_shndx_ == 0)
    {
      // This object has no symbols.  Weird but legal.
      return index;
    }

  gold_assert(off == static_cast<off_t>(align_address(off, size >> 3)));

  this->local_symbol_offset_ = off;

  // Read the symbol table section header.
  const unsigned int symtab_shndx = this->symtab_shndx_;
  typename This::Shdr symtabshdr(this,
				 this->elf_file_.section_header(symtab_shndx));
  gold_assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // Read the local symbols.
  const int sym_size = This::sym_size;
  const unsigned int loccount = this->local_symbol_count_;
  gold_assert(loccount == symtabshdr.get_sh_info());
  off_t locsize = loccount * sym_size;
  const unsigned char* psyms = this->get_view(symtabshdr.get_sh_offset(),
					      locsize, true);

  this->local_values_.resize(loccount);

  // Read the symbol names.
  const unsigned int strtab_shndx = symtabshdr.get_sh_link();
  off_t strtab_size;
  const unsigned char* pnamesu = this->section_contents(strtab_shndx,
							&strtab_size,
							true);
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  // Loop over the local symbols.

  const std::vector<Map_to_output>& mo(this->map_to_output());
  unsigned int shnum = this->shnum();
  unsigned int count = 0;
  // Skip the first, dummy, symbol.
  psyms += sym_size;
  for (unsigned int i = 1; i < loccount; ++i, psyms += sym_size)
    {
      elfcpp::Sym<size, big_endian> sym(psyms);

      Symbol_value<size>& lv(this->local_values_[i]);

      unsigned int shndx = sym.get_st_shndx();
      lv.set_input_shndx(shndx);

      if (sym.get_st_type() == elfcpp::STT_SECTION)
	lv.set_is_section_symbol();

      if (shndx >= elfcpp::SHN_LORESERVE)
	{
	  if (shndx == elfcpp::SHN_ABS)
	    lv.set_output_value(sym.get_st_value());
	  else
	    {
	      // FIXME: Handle SHN_XINDEX.
	      this->error(_("unknown section index %u for local symbol %u"),
			  shndx, i);
	      lv.set_output_value(0);
	    }
	}
      else
	{
	  if (shndx >= shnum)
	    {
	      this->error(_("local symbol %u section index %u out of range"),
			  i, shndx);
	      shndx = 0;
	    }

	  Output_section* os = mo[shndx].output_section;

	  if (os == NULL)
	    {
	      lv.set_output_value(0);
	      lv.set_no_output_symtab_entry();
	      continue;
	    }

	  if (mo[shndx].offset == -1)
	    lv.set_input_value(sym.get_st_value());
	  else
	    lv.set_output_value(mo[shndx].output_section->address()
				+ mo[shndx].offset
				+ sym.get_st_value());
	}

      // Decide whether this symbol should go into the output file.

      if (sym.get_st_type() == elfcpp::STT_SECTION)
	{
	  lv.set_no_output_symtab_entry();
	  continue;
	}

      if (sym.get_st_name() >= strtab_size)
	{
	  this->error(_("local symbol %u section name out of range: %u >= %u"),
		      i, sym.get_st_name(),
		      static_cast<unsigned int>(strtab_size));
	  lv.set_no_output_symtab_entry();
	  continue;
	}

      const char* name = pnames + sym.get_st_name();
      pool->add(name, true, NULL);
      lv.set_output_symtab_index(index);
      ++index;
      ++count;
    }

  this->output_local_symbol_count_ = count;

  return index;
}

// Return the value of the local symbol symndx.
template<int size, bool big_endian>
typename elfcpp::Elf_types<size>::Elf_Addr
Sized_relobj<size, big_endian>::local_symbol_value(unsigned int symndx) const
{
  gold_assert(symndx < this->local_symbol_count_);
  gold_assert(symndx < this->local_values_.size());
  const Symbol_value<size>& lv(this->local_values_[symndx]);
  return lv.value(this, 0);
}

// Return the value of a local symbol defined in input section SHNDX,
// with value VALUE, adding addend ADDEND.  IS_SECTION_SYMBOL
// indicates whether the symbol is a section symbol.  This handles
// SHF_MERGE sections.
template<int size, bool big_endian>
typename elfcpp::Elf_types<size>::Elf_Addr
Sized_relobj<size, big_endian>::local_value(unsigned int shndx,
					    Address value,
					    bool is_section_symbol,
					    Address addend) const
{
  const std::vector<Map_to_output>& mo(this->map_to_output());
  Output_section* os = mo[shndx].output_section;
  if (os == NULL)
    return addend;
  gold_assert(mo[shndx].offset == -1);

  // Do the mapping required by the output section.  If this is not a
  // section symbol, then we want to map the symbol value, and then
  // include the addend.  If this is a section symbol, then we need to
  // include the addend to figure out where in the section we are,
  // before we do the mapping.  This will do the right thing provided
  // the assembler is careful to only convert a relocation in a merged
  // section to a section symbol if there is a zero addend.  If the
  // assembler does not do this, then in general we can't know what to
  // do, because we can't distinguish the addend for the instruction
  // format from the addend for the section offset.

  if (is_section_symbol)
    return os->output_address(this, shndx, value + addend);
  else
    return addend + os->output_address(this, shndx, value);
}

// Write out the local symbols.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::write_local_symbols(Output_file* of,
						    const Stringpool* sympool)
{
  if (parameters->strip_all())
    return;

  gold_assert(this->symtab_shndx_ != -1U);
  if (this->symtab_shndx_ == 0)
    {
      // This object has no symbols.  Weird but legal.
      return;
    }

  // Read the symbol table section header.
  const unsigned int symtab_shndx = this->symtab_shndx_;
  typename This::Shdr symtabshdr(this,
				 this->elf_file_.section_header(symtab_shndx));
  gold_assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);
  const unsigned int loccount = this->local_symbol_count_;
  gold_assert(loccount == symtabshdr.get_sh_info());

  // Read the local symbols.
  const int sym_size = This::sym_size;
  off_t locsize = loccount * sym_size;
  const unsigned char* psyms = this->get_view(symtabshdr.get_sh_offset(),
					      locsize, false);

  // Read the symbol names.
  const unsigned int strtab_shndx = symtabshdr.get_sh_link();
  off_t strtab_size;
  const unsigned char* pnamesu = this->section_contents(strtab_shndx,
							&strtab_size,
							true);
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  // Get a view into the output file.
  off_t output_size = this->output_local_symbol_count_ * sym_size;
  unsigned char* oview = of->get_output_view(this->local_symbol_offset_,
					     output_size);

  const std::vector<Map_to_output>& mo(this->map_to_output());

  gold_assert(this->local_values_.size() == loccount);

  unsigned char* ov = oview;
  psyms += sym_size;
  for (unsigned int i = 1; i < loccount; ++i, psyms += sym_size)
    {
      elfcpp::Sym<size, big_endian> isym(psyms);

      if (!this->local_values_[i].needs_output_symtab_entry())
	continue;

      unsigned int st_shndx = isym.get_st_shndx();
      if (st_shndx < elfcpp::SHN_LORESERVE)
	{
	  gold_assert(st_shndx < mo.size());
	  if (mo[st_shndx].output_section == NULL)
	    continue;
	  st_shndx = mo[st_shndx].output_section->out_shndx();
	}

      elfcpp::Sym_write<size, big_endian> osym(ov);

      gold_assert(isym.get_st_name() < strtab_size);
      const char* name = pnames + isym.get_st_name();
      osym.put_st_name(sympool->get_offset(name));
      osym.put_st_value(this->local_values_[i].value(this, 0));
      osym.put_st_size(isym.get_st_size());
      osym.put_st_info(isym.get_st_info());
      osym.put_st_other(isym.get_st_other());
      osym.put_st_shndx(st_shndx);

      ov += sym_size;
    }

  gold_assert(ov - oview == output_size);

  of->write_output_view(this->local_symbol_offset_, output_size, oview);
}

// Input_objects methods.

// Add a regular relocatable object to the list.  Return false if this
// object should be ignored.

bool
Input_objects::add_object(Object* obj)
{
  if (!obj->is_dynamic())
    this->relobj_list_.push_back(static_cast<Relobj*>(obj));
  else
    {
      // See if this is a duplicate SONAME.
      Dynobj* dynobj = static_cast<Dynobj*>(obj);

      std::pair<Unordered_set<std::string>::iterator, bool> ins =
	this->sonames_.insert(dynobj->soname());
      if (!ins.second)
	{
	  // We have already seen a dynamic object with this soname.
	  return false;
	}

      this->dynobj_list_.push_back(dynobj);
    }

  Target* target = obj->target();
  if (this->target_ == NULL)
    this->target_ = target;
  else if (this->target_ != target)
    {
      gold_error(_("%s: incompatible target"), obj->name().c_str());
      return false;
    }

  set_parameters_size_and_endianness(target->get_size(),
				     target->is_big_endian());

  return true;
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
  if (et == elfcpp::ET_REL)
    {
      Sized_relobj<size, big_endian>* obj =
	new Sized_relobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup(ehdr);
      return obj;
    }
  else if (et == elfcpp::ET_DYN)
    {
      Sized_dynobj<size, big_endian>* obj =
	new Sized_dynobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup(ehdr);
      return obj;
    }
  else
    {
      gold_error(_("%s: unsupported ELF file type %d"),
		 name.c_str(), et);
      return NULL;
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
      gold_error(_("%s: ELF file too short"), name.c_str());
      return NULL;
    }

  int v = p[elfcpp::EI_VERSION];
  if (v != elfcpp::EV_CURRENT)
    {
      if (v == elfcpp::EV_NONE)
	gold_error(_("%s: invalid ELF version 0"), name.c_str());
      else
	gold_error(_("%s: unsupported ELF version %d"), name.c_str(), v);
      return NULL;
    }

  int c = p[elfcpp::EI_CLASS];
  if (c == elfcpp::ELFCLASSNONE)
    {
      gold_error(_("%s: invalid ELF class 0"), name.c_str());
      return NULL;
    }
  else if (c != elfcpp::ELFCLASS32
	   && c != elfcpp::ELFCLASS64)
    {
      gold_error(_("%s: unsupported ELF class %d"), name.c_str(), c);
      return NULL;
    }

  int d = p[elfcpp::EI_DATA];
  if (d == elfcpp::ELFDATANONE)
    {
      gold_error(_("%s: invalid ELF data encoding"), name.c_str());
      return NULL;
    }
  else if (d != elfcpp::ELFDATA2LSB
	   && d != elfcpp::ELFDATA2MSB)
    {
      gold_error(_("%s: unsupported ELF data encoding %d"), name.c_str(), d);
      return NULL;
    }

  bool big_endian = d == elfcpp::ELFDATA2MSB;

  if (c == elfcpp::ELFCLASS32)
    {
      if (bytes < elfcpp::Elf_sizes<32>::ehdr_size)
	{
	  gold_error(_("%s: ELF file too short"), name.c_str());
	  return NULL;
	}
      if (big_endian)
	{
#ifdef HAVE_TARGET_32_BIG
	  elfcpp::Ehdr<32, true> ehdr(p);
	  return make_elf_sized_object<32, true>(name, input_file,
						 offset, ehdr);
#else
          gold_error(_("%s: not configured to support "
		       "32-bit big-endian object"),
		     name.c_str());
	  return NULL;
#endif
	}
      else
	{
#ifdef HAVE_TARGET_32_LITTLE
	  elfcpp::Ehdr<32, false> ehdr(p);
	  return make_elf_sized_object<32, false>(name, input_file,
						  offset, ehdr);
#else
          gold_error(_("%s: not configured to support "
		       "32-bit little-endian object"),
		     name.c_str());
	  return NULL;
#endif
	}
    }
  else
    {
      if (bytes < elfcpp::Elf_sizes<32>::ehdr_size)
	{
	  gold_error(_("%s: ELF file too short"), name.c_str());
	  return NULL;
	}
      if (big_endian)
	{
#ifdef HAVE_TARGET_64_BIG
	  elfcpp::Ehdr<64, true> ehdr(p);
	  return make_elf_sized_object<64, true>(name, input_file,
						 offset, ehdr);
#else
          gold_error(_("%s: not configured to support "
		       "64-bit big-endian object"),
		     name.c_str());
	  return NULL;
#endif
	}
      else
	{
#ifdef HAVE_TARGET_64_LITTLE
	  elfcpp::Ehdr<64, false> ehdr(p);
	  return make_elf_sized_object<64, false>(name, input_file,
						  offset, ehdr);
#else
          gold_error(_("%s: not configured to support "
		       "64-bit little-endian object"),
		     name.c_str());
	  return NULL;
#endif
	}
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

#ifdef HAVE_TARGET_32_LITTLE
template
class Sized_relobj<32, false>;
#endif

#ifdef HAVE_TARGET_32_BIG
template
class Sized_relobj<32, true>;
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
class Sized_relobj<64, false>;
#endif

#ifdef HAVE_TARGET_64_BIG
template
class Sized_relobj<64, true>;
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
struct Relocate_info<32, false>;
#endif

#ifdef HAVE_TARGET_32_BIG
template
struct Relocate_info<32, true>;
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
struct Relocate_info<64, false>;
#endif

#ifdef HAVE_TARGET_64_BIG
template
struct Relocate_info<64, true>;
#endif

} // End namespace gold.
