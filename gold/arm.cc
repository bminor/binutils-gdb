// arm.cc -- arm target support for gold.

// Copyright 2009 Free Software Foundation, Inc.
// Written by Doug Kwan <dougkwan@google.com> based on the i386 code
// by Ian Lance Taylor <iant@google.com>.

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

#include <cstring>
#include <limits>
#include <cstdio>
#include <string>

#include "elfcpp.h"
#include "parameters.h"
#include "reloc.h"
#include "arm.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "copy-relocs.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"
#include "tls.h"
#include "defstd.h"

namespace
{

using namespace gold;

template<bool big_endian>
class Output_data_plt_arm;

// The arm target class.
//
// This is a very simple port of gold for ARM-EABI.  It is intended for
// supporting Android only for the time being.  Only these relocation types
// are supported.
//
// R_ARM_NONE
// R_ARM_ABS32
// R_ARM_REL32
// R_ARM_THM_CALL
// R_ARM_COPY
// R_ARM_GLOB_DAT
// R_ARM_BASE_PREL
// R_ARM_JUMP_SLOT
// R_ARM_RELATIVE
// R_ARM_GOTOFF32
// R_ARM_GOT_BREL
// R_ARM_PLT32
// R_ARM_CALL
// R_ARM_JUMP24
// R_ARM_TARGET1
// R_ARM_PREL31
// 
// Coming soon (pending patches):
// - Local scanner
// - Global scanner
// - Relocation
// - Defining section symbols __exidx_start and __exidx_stop.
// - Support interworking.
// - Mergeing all .ARM.xxx.yyy sections into .ARM.xxx.  Currently, they
//   are incorrectly merged into an .ARM section.
//
// TODOs:
// - Create a PT_ARM_EXIDX program header for a shared object that
//   might throw an exception.
// - Support more relocation types as needed. 
// - Make PLTs more flexible for different architecture features like
//   Thumb-2 and BE8.

template<bool big_endian>
class Target_arm : public Sized_target<32, big_endian>
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_REL, true, 32, big_endian>
    Reloc_section;

  Target_arm()
    : Sized_target<32, big_endian>(&arm_info),
      got_(NULL), plt_(NULL), got_plt_(NULL), rel_dyn_(NULL),
      copy_relocs_(elfcpp::R_ARM_COPY), dynbss_(NULL)
  { }

  // Process the relocations to determine unreferenced sections for 
  // garbage collection.
  void
  gc_process_relocs(const General_options& options,
		    Symbol_table* symtab,
		    Layout* layout,
		    Sized_relobj<32, big_endian>* object,
		    unsigned int data_shndx,
		    unsigned int sh_type,
		    const unsigned char* prelocs,
		    size_t reloc_count,
		    Output_section* output_section,
		    bool needs_special_offset_handling,
		    size_t local_symbol_count,
		    const unsigned char* plocal_symbols);

  // Scan the relocations to look for symbol adjustments.
  void
  scan_relocs(const General_options& options,
	      Symbol_table* symtab,
	      Layout* layout,
	      Sized_relobj<32, big_endian>* object,
	      unsigned int data_shndx,
	      unsigned int sh_type,
	      const unsigned char* prelocs,
	      size_t reloc_count,
	      Output_section* output_section,
	      bool needs_special_offset_handling,
	      size_t local_symbol_count,
	      const unsigned char* plocal_symbols);

  // Finalize the sections.
  void
  do_finalize_sections(Layout*);

  // Return the value to use for a dynamic symbol which requires special
  // treatment.
  uint64_t
  do_dynsym_value(const Symbol*) const;

  // Relocate a section.
  void
  relocate_section(const Relocate_info<32, big_endian>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   Output_section* output_section,
		   bool needs_special_offset_handling,
		   unsigned char* view,
		   elfcpp::Elf_types<32>::Elf_Addr view_address,
		   section_size_type view_size);

  // Scan the relocs during a relocatable link.
  void
  scan_relocatable_relocs(const General_options& options,
			  Symbol_table* symtab,
			  Layout* layout,
			  Sized_relobj<32, big_endian>* object,
			  unsigned int data_shndx,
			  unsigned int sh_type,
			  const unsigned char* prelocs,
			  size_t reloc_count,
			  Output_section* output_section,
			  bool needs_special_offset_handling,
			  size_t local_symbol_count,
			  const unsigned char* plocal_symbols,
			  Relocatable_relocs*);

  // Relocate a section during a relocatable link.
  void
  relocate_for_relocatable(const Relocate_info<32, big_endian>*,
			   unsigned int sh_type,
			   const unsigned char* prelocs,
			   size_t reloc_count,
			   Output_section* output_section,
			   off_t offset_in_output_section,
			   const Relocatable_relocs*,
			   unsigned char* view,
			   elfcpp::Elf_types<32>::Elf_Addr view_address,
			   section_size_type view_size,
			   unsigned char* reloc_view,
			   section_size_type reloc_view_size);

  // Return whether SYM is defined by the ABI.
  bool
  do_is_defined_by_abi(Symbol* sym) const
  { return strcmp(sym->name(), "__tls_get_addr") == 0; }

  // Return the size of the GOT section.
  section_size_type
  got_size()
  {
    gold_assert(this->got_ != NULL);
    return this->got_->data_size();
  }

  // Map platform-specific reloc types
  static unsigned int
  get_real_reloc_type (unsigned int r_type);

 private:
  // The class which scans relocations.
  class Scan
  {
   public:
    Scan()
    { }

    inline void
    local(const General_options& options, Symbol_table* symtab,
	  Layout* layout, Target_arm* target,
	  Sized_relobj<32, big_endian>* object,
	  unsigned int data_shndx,
	  Output_section* output_section,
	  const elfcpp::Rel<32, big_endian>& reloc, unsigned int r_type,
	  const elfcpp::Sym<32, big_endian>& lsym);

    inline void
    global(const General_options& options, Symbol_table* symtab,
	   Layout* layout, Target_arm* target,
	   Sized_relobj<32, big_endian>* object,
	   unsigned int data_shndx,
	   Output_section* output_section,
	   const elfcpp::Rel<32, big_endian>& reloc, unsigned int r_type,
	   Symbol* gsym);

   private:
    static void
    unsupported_reloc_local(Sized_relobj<32, big_endian>*,
			    unsigned int r_type);

    static void
    unsupported_reloc_global(Sized_relobj<32, big_endian>*,
			     unsigned int r_type, Symbol*);
  };

  // The class which implements relocation.
  class Relocate
  {
   public:
    Relocate()
    { }

    ~Relocate()
    { }

    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool
    relocate(const Relocate_info<32, big_endian>*, Target_arm*,
	     Output_section*,  size_t relnum,
	     const elfcpp::Rel<32, big_endian>&,
	     unsigned int r_type, const Sized_symbol<32>*,
	     const Symbol_value<32>*,
	     unsigned char*, elfcpp::Elf_types<32>::Elf_Addr,
	     section_size_type);
  };

  // A class which returns the size required for a relocation type,
  // used while scanning relocs during a relocatable link.
  class Relocatable_size_for_reloc
  {
   public:
    unsigned int
    get_size_for_reloc(unsigned int, Relobj*);
  };

  // Get the GOT section, creating it if necessary.
  Output_data_got<32, big_endian>*
  got_section(Symbol_table*, Layout*);

  // Get the GOT PLT section.
  Output_data_space*
  got_plt_section() const
  {
    gold_assert(this->got_plt_ != NULL);
    return this->got_plt_;
  }

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(Symbol_table*, Layout*, Symbol*);

  // Get the PLT section.
  const Output_data_plt_arm<big_endian>*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Get the dynamic reloc section, creating it if necessary.
  Reloc_section*
  rel_dyn_section(Layout*);

  // Return true if the symbol may need a COPY relocation.
  // References from an executable object to non-function symbols
  // defined in a dynamic object may need a COPY relocation.
  bool
  may_need_copy_reloc(Symbol* gsym)
  {
    return (!parameters->options().shared()
	    && gsym->is_from_dynobj()
	    && gsym->type() != elfcpp::STT_FUNC);
  }

  // Add a potential copy relocation.
  void
  copy_reloc(Symbol_table* symtab, Layout* layout,
	     Sized_relobj<32, big_endian>* object,
	     unsigned int shndx, Output_section* output_section,
	     Symbol* sym, const elfcpp::Rel<32, big_endian>& reloc)
  {
    this->copy_relocs_.copy_reloc(symtab, layout,
				  symtab->get_sized_symbol<32>(sym),
				  object, shndx, output_section, reloc,
				  this->rel_dyn_section(layout));
  }

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info arm_info;

  // The types of GOT entries needed for this platform.
  enum Got_type
  {
    GOT_TYPE_STANDARD = 0	// GOT entry for a regular symbol
  };

  // The GOT section.
  Output_data_got<32, big_endian>* got_;
  // The PLT section.
  Output_data_plt_arm<big_endian>* plt_;
  // The GOT PLT section.
  Output_data_space* got_plt_;
  // The dynamic reloc section.
  Reloc_section* rel_dyn_;
  // Relocs saved to avoid a COPY reloc.
  Copy_relocs<elfcpp::SHT_REL, 32, big_endian> copy_relocs_;
  // Space for variables copied with a COPY reloc.
  Output_data_space* dynbss_;
};

template<bool big_endian>
const Target::Target_info Target_arm<big_endian>::arm_info =
{
  32,			// size
  big_endian,		// is_big_endian
  elfcpp::EM_ARM,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  '\0',			// wrap_char
  "/usr/lib/libc.so.1",	// dynamic_linker
  0x8000,		// default_text_segment_address
  0x1000,		// abi_pagesize (overridable by -z max-page-size)
  0x1000		// common_pagesize (overridable by -z common-page-size)
};

// Get the GOT section, creating it if necessary.

template<bool big_endian>
Output_data_got<32, big_endian>*
Target_arm<big_endian>::got_section(Symbol_table* symtab, Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(symtab != NULL && layout != NULL);

      this->got_ = new Output_data_got<32, big_endian>();

      Output_section* os;
      os = layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
					   (elfcpp::SHF_ALLOC
					    | elfcpp::SHF_WRITE),
					   this->got_);
      os->set_is_relro();

      // The old GNU linker creates a .got.plt section.  We just
      // create another set of data in the .got section.  Note that we
      // always create a PLT if we create a GOT, although the PLT
      // might be empty.
      this->got_plt_ = new Output_data_space(4, "** GOT PLT");
      os = layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
					   (elfcpp::SHF_ALLOC
					    | elfcpp::SHF_WRITE),
					   this->got_plt_);
      os->set_is_relro();

      // The first three entries are reserved.
      this->got_plt_->set_current_data_size(3 * 4);

      // Define _GLOBAL_OFFSET_TABLE_ at the start of the PLT.
      symtab->define_in_output_data("_GLOBAL_OFFSET_TABLE_", NULL,
				    this->got_plt_,
				    0, 0, elfcpp::STT_OBJECT,
				    elfcpp::STB_LOCAL,
				    elfcpp::STV_HIDDEN, 0,
				    false, false);
    }
  return this->got_;
}

// Get the dynamic reloc section, creating it if necessary.

template<bool big_endian>
typename Target_arm<big_endian>::Reloc_section*
Target_arm<big_endian>::rel_dyn_section(Layout* layout)
{
  if (this->rel_dyn_ == NULL)
    {
      gold_assert(layout != NULL);
      this->rel_dyn_ = new Reloc_section(parameters->options().combreloc());
      layout->add_output_section_data(".rel.dyn", elfcpp::SHT_REL,
				      elfcpp::SHF_ALLOC, this->rel_dyn_);
    }
  return this->rel_dyn_;
}

// A class to handle the PLT data.

template<bool big_endian>
class Output_data_plt_arm : public Output_section_data
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_REL, true, 32, big_endian>
    Reloc_section;

  Output_data_plt_arm(Layout*, Output_data_space*);

  // Add an entry to the PLT.
  void
  add_entry(Symbol* gsym);

  // Return the .rel.plt section data.
  const Reloc_section*
  rel_plt() const
  { return this->rel_; }

 protected:
  void
  do_adjust_output_section(Output_section* os);

  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** PLT")); }

 private:
  // Template for the first PLT entry.
  static const uint32_t first_plt_entry[5];

  // Template for subsequent PLT entries. 
  static const uint32_t plt_entry[3];

  // Set the final size.
  void
  set_final_data_size()
  {
    this->set_data_size(sizeof(first_plt_entry)
			+ this->count_ * sizeof(plt_entry));
  }

  // Write out the PLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  // The .got.plt section.
  Output_data_space* got_plt_;
  // The number of PLT entries.
  unsigned int count_;
};

// Create the PLT section.  The ordinary .got section is an argument,
// since we need to refer to the start.  We also create our own .got
// section just for PLT entries.

template<bool big_endian>
Output_data_plt_arm<big_endian>::Output_data_plt_arm(Layout* layout,
						     Output_data_space* got_plt)
  : Output_section_data(4), got_plt_(got_plt), count_(0)
{
  this->rel_ = new Reloc_section(false);
  layout->add_output_section_data(".rel.plt", elfcpp::SHT_REL,
				  elfcpp::SHF_ALLOC, this->rel_);
}

template<bool big_endian>
void
Output_data_plt_arm<big_endian>::do_adjust_output_section(Output_section* os)
{
  os->set_entsize(0);
}

// Add an entry to the PLT.

template<bool big_endian>
void
Output_data_plt_arm<big_endian>::add_entry(Symbol* gsym)
{
  gold_assert(!gsym->has_plt_offset());

  // Note that when setting the PLT offset we skip the initial
  // reserved PLT entry.
  gsym->set_plt_offset((this->count_) * sizeof(plt_entry)
		       + sizeof(first_plt_entry));

  ++this->count_;

  section_offset_type got_offset = this->got_plt_->current_data_size();

  // Every PLT entry needs a GOT entry which points back to the PLT
  // entry (this will be changed by the dynamic linker, normally
  // lazily when the function is called).
  this->got_plt_->set_current_data_size(got_offset + 4);

  // Every PLT entry needs a reloc.
  gsym->set_needs_dynsym_entry();
  this->rel_->add_global(gsym, elfcpp::R_ARM_JUMP_SLOT, this->got_plt_,
			 got_offset);

  // Note that we don't need to save the symbol.  The contents of the
  // PLT are independent of which symbols are used.  The symbols only
  // appear in the relocations.
}

// ARM PLTs.
// FIXME:  This is not very flexible.  Right now this has only been tested
// on armv5te.  If we are to support additional architecture features like
// Thumb-2 or BE8, we need to make this more flexible like GNU ld.

// The first entry in the PLT.
template<bool big_endian>
const uint32_t Output_data_plt_arm<big_endian>::first_plt_entry[5] =
{
  0xe52de004,	// str   lr, [sp, #-4]!
  0xe59fe004,   // ldr   lr, [pc, #4]
  0xe08fe00e,	// add   lr, pc, lr 
  0xe5bef008,	// ldr   pc, [lr, #8]!
  0x00000000,	// &GOT[0] - .
};

// Subsequent entries in the PLT.

template<bool big_endian>
const uint32_t Output_data_plt_arm<big_endian>::plt_entry[3] =
{
  0xe28fc600,	// add   ip, pc, #0xNN00000
  0xe28cca00,	// add   ip, ip, #0xNN000
  0xe5bcf000,	// ldr   pc, [ip, #0xNNN]!
};

// Write out the PLT.  This uses the hand-coded instructions above,
// and adjusts them as needed.  This is all specified by the arm ELF
// Processor Supplement.

template<bool big_endian>
void
Output_data_plt_arm<big_endian>::do_write(Output_file* of)
{
  const off_t offset = this->offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size());
  unsigned char* const oview = of->get_output_view(offset, oview_size);

  const off_t got_file_offset = this->got_plt_->offset();
  const section_size_type got_size =
    convert_to_section_size_type(this->got_plt_->data_size());
  unsigned char* const got_view = of->get_output_view(got_file_offset,
						      got_size);
  unsigned char* pov = oview;

  elfcpp::Elf_types<32>::Elf_Addr plt_address = this->address();
  elfcpp::Elf_types<32>::Elf_Addr got_address = this->got_plt_->address();

  // Write first PLT entry.  All but the last word are constants.
  const size_t num_first_plt_words = (sizeof(first_plt_entry)
				      / sizeof(plt_entry[0]));
  for (size_t i = 0; i < num_first_plt_words - 1; i++)
    elfcpp::Swap<32, big_endian>::writeval(pov + i * 4, first_plt_entry[i]);
  // Last word in first PLT entry is &GOT[0] - .
  elfcpp::Swap<32, big_endian>::writeval(pov + 16,
					 got_address - (plt_address + 16));
  pov += sizeof(first_plt_entry);

  unsigned char* got_pov = got_view;

  memset(got_pov, 0, 12);
  got_pov += 12;

  const int rel_size = elfcpp::Elf_sizes<32>::rel_size;
  unsigned int plt_offset = sizeof(first_plt_entry);
  unsigned int plt_rel_offset = 0;
  unsigned int got_offset = 12;
  const unsigned int count = this->count_;
  for (unsigned int i = 0;
       i < count;
       ++i,
	 pov += sizeof(plt_entry),
	 got_pov += 4,
	 plt_offset += sizeof(plt_entry),
	 plt_rel_offset += rel_size,
	 got_offset += 4)
    {
      // Set and adjust the PLT entry itself.
      int32_t offset = ((got_address + got_offset)
			 - (plt_address + plt_offset + 8));

      gold_assert(offset >= 0 && offset < 0x0fffffff);
      uint32_t plt_insn0 = plt_entry[0] | ((offset >> 20) & 0xff);
      elfcpp::Swap<32, big_endian>::writeval(pov, plt_insn0);
      uint32_t plt_insn1 = plt_entry[1] | ((offset >> 12) & 0xff);
      elfcpp::Swap<32, big_endian>::writeval(pov + 4, plt_insn1);
      uint32_t plt_insn2 = plt_entry[2] | (offset & 0xfff);
      elfcpp::Swap<32, big_endian>::writeval(pov + 8, plt_insn2);

      // Set the entry in the GOT.
      elfcpp::Swap<32, big_endian>::writeval(got_pov, plt_address);
    }

  gold_assert(static_cast<section_size_type>(pov - oview) == oview_size);
  gold_assert(static_cast<section_size_type>(got_pov - got_view) == got_size);

  of->write_output_view(offset, oview_size, oview);
  of->write_output_view(got_file_offset, got_size, got_view);
}

// Create a PLT entry for a global symbol.

template<bool big_endian>
void
Target_arm<big_endian>::make_plt_entry(Symbol_table* symtab, Layout* layout,
				       Symbol* gsym)
{
  if (gsym->has_plt_offset())
    return;

  if (this->plt_ == NULL)
    {
      // Create the GOT sections first.
      this->got_section(symtab, layout);

      this->plt_ = new Output_data_plt_arm<big_endian>(layout, this->got_plt_);
      layout->add_output_section_data(".plt", elfcpp::SHT_PROGBITS,
				      (elfcpp::SHF_ALLOC
				       | elfcpp::SHF_EXECINSTR),
				      this->plt_);
    }
  this->plt_->add_entry(gsym);
}

// Report an unsupported relocation against a local symbol.

template<bool big_endian>
void
Target_arm<big_endian>::Scan::unsupported_reloc_local(
    Sized_relobj<32, big_endian>* object,
    unsigned int r_type)
{
  gold_error(_("%s: unsupported reloc %u against local symbol"),
	     object->name().c_str(), r_type);
}

// Scan a relocation for a local symbol.

template<bool big_endian>
inline void
Target_arm<big_endian>::Scan::local(const General_options&,
				    Symbol_table* /* symtab */,
				    Layout* /* layout */,
				    Target_arm* /* target */,
				    Sized_relobj<32, big_endian>* object,
				    unsigned int /* data_shndx */,
				    Output_section* /* output_section */,
				    const elfcpp::Rel<32, big_endian>& /* reloc */,
				    unsigned int r_type,
				    const elfcpp::Sym<32, big_endian>&)
{
  r_type = get_real_reloc_type(r_type);
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    default:
      unsupported_reloc_local(object, r_type);
      break;
    }
}

// Report an unsupported relocation against a global symbol.

template<bool big_endian>
void
Target_arm<big_endian>::Scan::unsupported_reloc_global(
    Sized_relobj<32, big_endian>* object,
    unsigned int r_type,
    Symbol* gsym)
{
  gold_error(_("%s: unsupported reloc %u against global symbol %s"),
	     object->name().c_str(), r_type, gsym->demangled_name().c_str());
}

// Scan a relocation for a global symbol.

template<bool big_endian>
inline void
Target_arm<big_endian>::Scan::global(const General_options&,
				     Symbol_table* /* symtab */,
				     Layout* /* layout */,
				     Target_arm* /* target */,
				     Sized_relobj<32, big_endian>* object,
				     unsigned int /* data_shndx */,
				     Output_section* /* output_section */,
				     const elfcpp::Rel<32, big_endian>& /* reloc */,
				     unsigned int r_type,
				     Symbol* gsym)
{
  r_type = get_real_reloc_type(r_type);
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    default:
      unsupported_reloc_global(object, r_type, gsym);
      break;
    }
}

// Process relocations for gc.

template<bool big_endian>
void
Target_arm<big_endian>::gc_process_relocs(const General_options& options,
					  Symbol_table* symtab,
					  Layout* layout,
					  Sized_relobj<32, big_endian>* object,
					  unsigned int data_shndx,
					  unsigned int,
					  const unsigned char* prelocs,
					  size_t reloc_count,
					  Output_section* output_section,
					  bool needs_special_offset_handling,
					  size_t local_symbol_count,
					  const unsigned char* plocal_symbols)
{
  typedef Target_arm<big_endian> Arm;
  typedef typename Target_arm<big_endian>::Scan Scan;

  gold::gc_process_relocs<32, big_endian, Arm, elfcpp::SHT_REL, Scan>(
    options,
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

// Scan relocations for a section.

template<bool big_endian>
void
Target_arm<big_endian>::scan_relocs(const General_options& options,
				    Symbol_table* symtab,
				    Layout* layout,
				    Sized_relobj<32, big_endian>* object,
				    unsigned int data_shndx,
				    unsigned int sh_type,
				    const unsigned char* prelocs,
				    size_t reloc_count,
				    Output_section* output_section,
				    bool needs_special_offset_handling,
				    size_t local_symbol_count,
				    const unsigned char* plocal_symbols)
{
  typedef typename Target_arm<big_endian>::Scan Scan;
  if (sh_type == elfcpp::SHT_RELA)
    {
      gold_error(_("%s: unsupported RELA reloc section"),
		 object->name().c_str());
      return;
    }

  gold::scan_relocs<32, big_endian, Target_arm, elfcpp::SHT_REL, Scan>(
    options,
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

// Finalize the sections.

template<bool big_endian>
void
Target_arm<big_endian>::do_finalize_sections(Layout* layout)
{
  // Fill in some more dynamic tags.
  Output_data_dynamic* const odyn = layout->dynamic_data();
  if (odyn != NULL)
    {
      if (this->got_plt_ != NULL)
	odyn->add_section_address(elfcpp::DT_PLTGOT, this->got_plt_);

      if (this->plt_ != NULL)
	{
	  const Output_data* od = this->plt_->rel_plt();
	  odyn->add_section_size(elfcpp::DT_PLTRELSZ, od);
	  odyn->add_section_address(elfcpp::DT_JMPREL, od);
	  odyn->add_constant(elfcpp::DT_PLTREL, elfcpp::DT_REL);
	}

      if (this->rel_dyn_ != NULL)
	{
	  const Output_data* od = this->rel_dyn_;
	  odyn->add_section_address(elfcpp::DT_REL, od);
	  odyn->add_section_size(elfcpp::DT_RELSZ, od);
	  odyn->add_constant(elfcpp::DT_RELENT,
			     elfcpp::Elf_sizes<32>::rel_size);
	}

      if (!parameters->options().shared())
	{
	  // The value of the DT_DEBUG tag is filled in by the dynamic
	  // linker at run time, and used by the debugger.
	  odyn->add_constant(elfcpp::DT_DEBUG, 0);
	}
    }

  // Emit any relocs we saved in an attempt to avoid generating COPY
  // relocs.
  if (this->copy_relocs_.any_saved_relocs())
    this->copy_relocs_.emit(this->rel_dyn_section(layout));
}

// Perform a relocation.

template<bool big_endian>
inline bool
Target_arm<big_endian>::Relocate::relocate(
    const Relocate_info<32, big_endian>* /* relinfo */,
    Target_arm* /* target */,
    Output_section* /* output_section */,
    size_t /* relnum */,
    const elfcpp::Rel<32, big_endian>& /* rel */,
    unsigned int r_type,
    const Sized_symbol<32>* /* gsym */,
    const Symbol_value<32>* /* psymval */,
    unsigned char* /* view */,
    elfcpp::Elf_types<32>::Elf_Addr /* address */,
    section_size_type /* view_size */ )
{
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    default:
      gold_unreachable();
    }

  return true;
}

// Relocate section data.

template<bool big_endian>
void
Target_arm<big_endian>::relocate_section(
    const Relocate_info<32, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    unsigned char* view,
    elfcpp::Elf_types<32>::Elf_Addr address,
    section_size_type view_size)
{
  typedef typename Target_arm<big_endian>::Relocate Arm_relocate;
  gold_assert(sh_type == elfcpp::SHT_REL);

  gold::relocate_section<32, big_endian, Target_arm, elfcpp::SHT_REL,
			 Arm_relocate>(
    relinfo,
    this,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    view,
    address,
    view_size);
}

// Return the size of a relocation while scanning during a relocatable
// link.

template<bool big_endian>
unsigned int
Target_arm<big_endian>::Relocatable_size_for_reloc::get_size_for_reloc(
    unsigned int r_type,
    Relobj* object)
{
  r_type = get_real_reloc_type(r_type);
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      return 0;

    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_REL32:
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_GOTOFF32:
    case elfcpp::R_ARM_BASE_PREL:
    case elfcpp::R_ARM_GOT_BREL:
    case elfcpp::R_ARM_PLT32:
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_PREL31:
      return 4;

    case elfcpp::R_ARM_TARGET1:
      // This should have been mapped to another type already.
      // Fall through.
    case elfcpp::R_ARM_COPY:
    case elfcpp::R_ARM_GLOB_DAT:
    case elfcpp::R_ARM_JUMP_SLOT:
    case elfcpp::R_ARM_RELATIVE:
      // These are relocations which should only be seen by the
      // dynamic linker, and should never be seen here.
      gold_error(_("%s: unexpected reloc %u in object file"),
		 object->name().c_str(), r_type);
      return 0;

    default:
      object->error(_("unsupported reloc %u in object file"), r_type);
      return 0;
    }
}

// Scan the relocs during a relocatable link.

template<bool big_endian>
void
Target_arm<big_endian>::scan_relocatable_relocs(
    const General_options& options,
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj<32, big_endian>* object,
    unsigned int data_shndx,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols,
    Relocatable_relocs* rr)
{
  gold_assert(sh_type == elfcpp::SHT_REL);

  typedef gold::Default_scan_relocatable_relocs<elfcpp::SHT_REL,
    Relocatable_size_for_reloc> Scan_relocatable_relocs;

  gold::scan_relocatable_relocs<32, big_endian, elfcpp::SHT_REL,
      Scan_relocatable_relocs>(
    options,
    symtab,
    layout,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols,
    rr);
}

// Relocate a section during a relocatable link.

template<bool big_endian>
void
Target_arm<big_endian>::relocate_for_relocatable(
    const Relocate_info<32, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    off_t offset_in_output_section,
    const Relocatable_relocs* rr,
    unsigned char* view,
    elfcpp::Elf_types<32>::Elf_Addr view_address,
    section_size_type view_size,
    unsigned char* reloc_view,
    section_size_type reloc_view_size)
{
  gold_assert(sh_type == elfcpp::SHT_REL);

  gold::relocate_for_relocatable<32, big_endian, elfcpp::SHT_REL>(
    relinfo,
    prelocs,
    reloc_count,
    output_section,
    offset_in_output_section,
    rr,
    view,
    view_address,
    view_size,
    reloc_view,
    reloc_view_size);
}

// Return the value to use for a dynamic symbol which requires special
// treatment.  This is how we support equality comparisons of function
// pointers across shared library boundaries, as described in the
// processor specific ABI supplement.

template<bool big_endian>
uint64_t
Target_arm<big_endian>::do_dynsym_value(const Symbol* gsym) const
{
  gold_assert(gsym->is_from_dynobj() && gsym->has_plt_offset());
  return this->plt_section()->address() + gsym->plt_offset();
}

// Map platform-specific relocs to real relocs
//
template<bool big_endian>
unsigned int
Target_arm<big_endian>::get_real_reloc_type (unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_ARM_TARGET1:
      // This is either R_ARM_ABS32 or R_ARM_REL32;
      return elfcpp::R_ARM_ABS32;

    case elfcpp::R_ARM_TARGET2:
      // This can be any reloc type but ususally is R_ARM_GOT_PREL
      return elfcpp::R_ARM_GOT_PREL;

    default:
      return r_type;
    }
}

// The selector for arm object files.

template<bool big_endian>
class Target_selector_arm : public Target_selector
{
 public:
  Target_selector_arm()
    : Target_selector(elfcpp::EM_ARM, 32, big_endian,
		      (big_endian ? "elf32-bigarm" : "elf32-littlearm"))
  { }

  Target*
  do_instantiate_target()
  { return new Target_arm<big_endian>(); }
};

Target_selector_arm<false> target_selector_arm;
Target_selector_arm<true> target_selector_armbe;

} // End anonymous namespace.
