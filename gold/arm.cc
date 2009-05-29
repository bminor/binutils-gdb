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
// - Support for dynamic symbols (GOT, PLT and etc).
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

template<bool big_endian>
class Target_arm : public Sized_target<32, big_endian>
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_REL, true, 32, big_endian>
    Reloc_section;

  Target_arm()
    : Sized_target<32, big_endian>(&arm_info)
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

  // Return the value to use for a dynamic which requires special
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

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info arm_info;
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
Target_arm<big_endian>::do_finalize_sections(Layout* /* layout */)
{
  gold_unreachable ();
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

template<bool big_endian>
uint64_t
Target_arm<big_endian>::do_dynsym_value(const Symbol* /*gsym*/) const
{
  gold_unreachable ();
  return 0;
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
