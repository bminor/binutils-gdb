// aarch64.cc -- aarch64 target support for gold.

// Copyright (C) 2014 Free Software Foundation, Inc.
// Written by Jing Yu <jingyu@google.com> and Han Shen <shenhan@google.com>.

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

#include "elfcpp.h"
#include "dwarf.h"
#include "parameters.h"
#include "reloc.h"
#include "aarch64.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "copy-relocs.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"
#include "tls.h"
#include "freebsd.h"
#include "nacl.h"
#include "gc.h"
#include "icf.h"
#include "aarch64-reloc-property.h"

// The first three .got.plt entries are reserved.
const int32_t AARCH64_GOTPLT_RESERVE_COUNT = 3;

namespace
{

using namespace gold;

template<int size, bool big_endian>
class Output_data_plt_aarch64;

template<int size, bool big_endian>
class Output_data_plt_aarch64_standard;

template<int size, bool big_endian>
class Target_aarch64;

template<int size, bool big_endian>
class AArch64_relocate_functions;

// Output_data_got_aarch64 class.

template<int size, bool big_endian>
class Output_data_got_aarch64 : public Output_data_got<size, big_endian>
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Valtype;
  Output_data_got_aarch64(Symbol_table* symtab, Layout* layout)
    : Output_data_got<size, big_endian>(),
      symbol_table_(symtab), layout_(layout)
  { }

 protected:
  // Write out the GOT table.
  void
  do_write(Output_file* of) {
    // The first entry in the GOT is the address of the .dynamic section.
    gold_assert(this->data_size() >= size / 8);
    Output_section* dynamic = this->layout_->dynamic_section();
    Valtype dynamic_addr = dynamic == NULL ? 0 : dynamic->address();
    this->replace_constant(0, dynamic_addr);
    Output_data_got<size, big_endian>::do_write(of);
  }

 private:
  // Symbol table of the output object.
  Symbol_table* symbol_table_;
  // A pointer to the Layout class, so that we can find the .dynamic
  // section when we write out the GOT section.
  Layout* layout_;
};

AArch64_reloc_property_table* aarch64_reloc_property_table = NULL;

// The aarch64 target class.
// See the ABI at
// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0056b/IHI0056B_aaelf64.pdf
template<int size, bool big_endian>
class Target_aarch64 : public Sized_target<size, big_endian>
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian>
      Reloc_section;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  Target_aarch64(const Target::Target_info* info = &aarch64_info)
    : Sized_target<size, big_endian>(info),
      got_(NULL), plt_(NULL), got_plt_(NULL),
      global_offset_table_(NULL), rela_dyn_(NULL),
      copy_relocs_(elfcpp::R_AARCH64_COPY)
  { }

  // Scan the relocations to determine unreferenced sections for
  // garbage collection.
  void
  gc_process_relocs(Symbol_table* symtab,
		    Layout* layout,
		    Sized_relobj_file<size, big_endian>* object,
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
  scan_relocs(Symbol_table* symtab,
	      Layout* layout,
	      Sized_relobj_file<size, big_endian>* object,
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
  do_finalize_sections(Layout*, const Input_objects*, Symbol_table*);

  // Relocate a section.
  void
  relocate_section(const Relocate_info<size, big_endian>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   Output_section* output_section,
		   bool needs_special_offset_handling,
		   unsigned char* view,
		   typename elfcpp::Elf_types<size>::Elf_Addr view_address,
		   section_size_type view_size,
		   const Reloc_symbol_changes*);

  // Scan the relocs during a relocatable link.
  void
  scan_relocatable_relocs(Symbol_table* symtab,
			  Layout* layout,
			  Sized_relobj_file<size, big_endian>* object,
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
  relocate_relocs(
      const Relocate_info<size, big_endian>*,
      unsigned int sh_type,
      const unsigned char* prelocs,
      size_t reloc_count,
      Output_section* output_section,
      typename elfcpp::Elf_types<size>::Elf_Off offset_in_output_section,
      const Relocatable_relocs*,
      unsigned char* view,
      typename elfcpp::Elf_types<size>::Elf_Addr view_address,
      section_size_type view_size,
      unsigned char* reloc_view,
      section_size_type reloc_view_size);

  // Return the PLT section.
  uint64_t
  do_plt_address_for_global(const Symbol* gsym) const
  { return this->plt_section()->address_for_global(gsym); }

  uint64_t
  do_plt_address_for_local(const Relobj* relobj, unsigned int symndx) const
  { return this->plt_section()->address_for_local(relobj, symndx); }

  // Return the number of entries in the PLT.
  unsigned int
  plt_entry_count() const;

  //Return the offset of the first non-reserved PLT entry.
  unsigned int
  first_plt_entry_offset() const;

  // Return the size of each PLT entry.
  unsigned int
  plt_entry_size() const;

 protected:
  void
  do_select_as_default_target()
  {
    gold_assert(aarch64_reloc_property_table == NULL);
    aarch64_reloc_property_table = new AArch64_reloc_property_table();
  }

  virtual Output_data_plt_aarch64<size, big_endian>*
  do_make_data_plt(Layout* layout, Output_data_space* got_plt)
  {
    return new Output_data_plt_aarch64_standard<size, big_endian>(layout,
								  got_plt);
  }

  Output_data_plt_aarch64<size, big_endian>*
  make_data_plt(Layout* layout, Output_data_space* got_plt)
  {
    return this->do_make_data_plt(layout, got_plt);
  }

 private:
  // The class which scans relocations.
  class Scan
  {
  public:
    Scan()
      : issued_non_pic_error_(false)
    { }

    inline void
    local(Symbol_table* symtab, Layout* layout, Target_aarch64* target,
	  Sized_relobj_file<size, big_endian>* object,
	  unsigned int data_shndx,
	  Output_section* output_section,
	  const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	  const elfcpp::Sym<size, big_endian>& lsym,
	  bool is_discarded);

    inline void
    global(Symbol_table* symtab, Layout* layout, Target_aarch64* target,
	   Sized_relobj_file<size, big_endian>* object,
	   unsigned int data_shndx,
	   Output_section* output_section,
	   const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	   Symbol* gsym);

    inline bool
    local_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					Target_aarch64<size, big_endian>* ,
					Sized_relobj_file<size, big_endian>* ,
					unsigned int ,
					Output_section* ,
					const elfcpp::Rela<size, big_endian>& ,
					unsigned int r_type,
					const elfcpp::Sym<size, big_endian>&);

    inline bool
    global_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					 Target_aarch64<size, big_endian>* ,
					 Sized_relobj_file<size, big_endian>* ,
					 unsigned int ,
					 Output_section* ,
					 const elfcpp::Rela<size, big_endian>& ,
					 unsigned int r_type,
					 Symbol* gsym);

  private:
    static void
    unsupported_reloc_local(Sized_relobj_file<size, big_endian>*,
			    unsigned int r_type);

    static void
    unsupported_reloc_global(Sized_relobj_file<size, big_endian>*,
			     unsigned int r_type, Symbol*);

    inline bool
    possible_function_pointer_reloc(unsigned int r_type);

    void
    check_non_pic(Relobj*, unsigned int r_type);

    // Whether we have issued an error about a non-PIC compilation.
    bool issued_non_pic_error_;
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
    relocate(const Relocate_info<size, big_endian>*, Target_aarch64*,
	     Output_section*,
	     size_t relnum, const elfcpp::Rela<size, big_endian>&,
	     unsigned int r_type, const Sized_symbol<size>*,
	     const Symbol_value<size>*,
	     unsigned char*, typename elfcpp::Elf_types<size>::Elf_Addr,
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

  // Adjust TLS relocation type based on the options and whether this
  // is a local symbol.
  static tls::Tls_optimization
  optimize_tls_reloc(bool is_final, int r_type);

  // Get the GOT section, creating it if necessary.
  Output_data_got_aarch64<size, big_endian>*
  got_section(Symbol_table*, Layout*);

  // Get the GOT PLT section.
  Output_data_space*
  got_plt_section() const
  {
    gold_assert(this->got_plt_ != NULL);
    return this->got_plt_;
  }

  // Create the PLT section.
  void
  make_plt_section(Symbol_table* symtab, Layout* layout);

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(Symbol_table*, Layout*, Symbol*);

  // Get the PLT section.
  Output_data_plt_aarch64<size, big_endian>*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Get the dynamic reloc section, creating it if necessary.
  Reloc_section*
  rela_dyn_section(Layout*);

  // Add a potential copy relocation.
  void
  copy_reloc(Symbol_table* symtab, Layout* layout,
	     Sized_relobj_file<size, big_endian>* object,
	     unsigned int shndx, Output_section* output_section,
	     Symbol* sym, const elfcpp::Rela<size, big_endian>& reloc)
  {
    this->copy_relocs_.copy_reloc(symtab, layout,
				  symtab->get_sized_symbol<size>(sym),
				  object, shndx, output_section,
				  reloc, this->rela_dyn_section(layout));
  }

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info aarch64_info;

  // The types of GOT entries needed for this platform.
  // These values are exposed to the ABI in an incremental link.
  // Do not renumber existing values without changing the version
  // number of the .gnu_incremental_inputs section.
  enum Got_type
  {
    GOT_TYPE_STANDARD = 0,      // GOT entry for a regular symbol
    GOT_TYPE_TLS_OFFSET = 1,    // GOT entry for TLS offset
    GOT_TYPE_TLS_PAIR = 2,      // GOT entry for TLS module/offset pair
    GOT_TYPE_TLS_DESC = 3       // GOT entry for TLS_DESC pair
  };

  // The GOT section.
  Output_data_got_aarch64<size, big_endian>* got_;
  // The PLT section.
  Output_data_plt_aarch64<size, big_endian>* plt_;
  // The GOT PLT section.
  Output_data_space* got_plt_;
  // The _GLOBAL_OFFSET_TABLE_ symbol.
  Symbol* global_offset_table_;
  // The dynamic reloc section.
  Reloc_section* rela_dyn_;
  // Relocs saved to avoid a COPY reloc.
  Copy_relocs<elfcpp::SHT_RELA, size, big_endian> copy_relocs_;
};

template<>
const Target::Target_info Target_aarch64<64, false>::aarch64_info =
{
  64,			// size
  false,		// is_big_endian
  elfcpp::EM_AARCH64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/lib/ld.so.1",	// program interpreter
  0x400000,		// default_text_segment_address
  0x1000,		// abi_pagesize (overridable by -z max-page-size)
  0x1000,		// common_pagesize (overridable by -z common-page-size)
  false,                // isolate_execinstr
  0,                    // rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
const Target::Target_info Target_aarch64<32, false>::aarch64_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_AARCH64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/lib/ld.so.1",	// program interpreter
  0x400000,		// default_text_segment_address
  0x1000,		// abi_pagesize (overridable by -z max-page-size)
  0x1000,		// common_pagesize (overridable by -z common-page-size)
  false,                // isolate_execinstr
  0,                    // rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
const Target::Target_info Target_aarch64<64, true>::aarch64_info =
{
  64,			// size
  true,			// is_big_endian
  elfcpp::EM_AARCH64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/lib/ld.so.1",	// program interpreter
  0x400000,		// default_text_segment_address
  0x1000,		// abi_pagesize (overridable by -z max-page-size)
  0x1000,		// common_pagesize (overridable by -z common-page-size)
  false,                // isolate_execinstr
  0,                    // rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
const Target::Target_info Target_aarch64<32, true>::aarch64_info =
{
  32,			// size
  true,			// is_big_endian
  elfcpp::EM_AARCH64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/lib/ld.so.1",	// program interpreter
  0x400000,		// default_text_segment_address
  0x1000,		// abi_pagesize (overridable by -z max-page-size)
  0x1000,		// common_pagesize (overridable by -z common-page-size)
  false,                // isolate_execinstr
  0,                    // rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

// Get the GOT section, creating it if necessary.

template<int size, bool big_endian>
Output_data_got_aarch64<size, big_endian>*
Target_aarch64<size, big_endian>::got_section(Symbol_table* symtab,
					      Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(symtab != NULL && layout != NULL);

      // When using -z now, we can treat .got.plt as a relro section.
      // Without -z now, it is modified after program startup by lazy
      // PLT relocations.
      bool is_got_plt_relro = parameters->options().now();
      Output_section_order got_order = (is_got_plt_relro
					? ORDER_RELRO
					: ORDER_RELRO_LAST);
      Output_section_order got_plt_order = (is_got_plt_relro
					    ? ORDER_RELRO
					    : ORDER_NON_RELRO_FIRST);

      // Layout of .got and .got.plt sections.
      // .got[0] &_DYNAMIC                          <-_GLOBAL_OFFSET_TABLE_
      // ...
      // .gotplt[0] reserved for ld.so (&linkmap)   <--DT_PLTGOT
      // .gotplt[1] reserved for ld.so (resolver)
      // .gotplt[2] reserved

      // Generate .got section.
      this->got_ = new Output_data_got_aarch64<size, big_endian>(symtab,
								 layout);
      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      (elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE),
				      this->got_, got_order, true);
      // The first word of GOT is reserved for the address of .dynamic.
      // We put 0 here now. The value will be replaced later in
      // Output_data_got_aarch64::do_write.
      this->got_->add_constant(0);

      // Define _GLOBAL_OFFSET_TABLE_ at the start of the PLT.
      // _GLOBAL_OFFSET_TABLE_ value points to the start of the .got section,
      // even if there is a .got.plt section.
      this->global_offset_table_ =
	symtab->define_in_output_data("_GLOBAL_OFFSET_TABLE_", NULL,
				      Symbol_table::PREDEFINED,
				      this->got_,
				      0, 0, elfcpp::STT_OBJECT,
				      elfcpp::STB_LOCAL,
				      elfcpp::STV_HIDDEN, 0,
				      false, false);

      // Generate .got.plt section.
      this->got_plt_ = new Output_data_space(size / 8, "** GOT PLT");
      layout->add_output_section_data(".got.plt", elfcpp::SHT_PROGBITS,
				      (elfcpp::SHF_ALLOC
				       | elfcpp::SHF_WRITE),
				      this->got_plt_, got_plt_order,
				      is_got_plt_relro);

      // The first three entries are reserved.
      this->got_plt_->set_current_data_size(
	AARCH64_GOTPLT_RESERVE_COUNT * (size / 8));

      if (!is_got_plt_relro)
	{
	  // Those bytes can go into the relro segment.
	  layout->increase_relro(
	    AARCH64_GOTPLT_RESERVE_COUNT * (size / 8));
	}

    }
  return this->got_;
}

// Get the dynamic reloc section, creating it if necessary.

template<int size, bool big_endian>
typename Target_aarch64<size, big_endian>::Reloc_section*
Target_aarch64<size, big_endian>::rela_dyn_section(Layout* layout)
{
  if (this->rela_dyn_ == NULL)
    {
      gold_assert(layout != NULL);
      this->rela_dyn_ = new Reloc_section(parameters->options().combreloc());
      layout->add_output_section_data(".rela.dyn", elfcpp::SHT_RELA,
				      elfcpp::SHF_ALLOC, this->rela_dyn_,
				      ORDER_DYNAMIC_RELOCS, false);
    }
  return this->rela_dyn_;
}

// A class to handle the PLT data.
// This is an abstract base class that handles most of the linker details
// but does not know the actual contents of PLT entries.  The derived
// classes below fill in those details.

template<int size, bool big_endian>
class Output_data_plt_aarch64 : public Output_section_data
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian>
      Reloc_section;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  Output_data_plt_aarch64(Layout* layout,
			  uint64_t addralign,
			  Output_data_space* got_plt)
    : Output_section_data(addralign),
      got_plt_(got_plt),
      count_(0)
  { this->init(layout); }

  // Initialize the PLT section.
  void
  init(Layout* layout);

  // Add an entry to the PLT.
  void
  add_entry(Symbol* gsym);

  // Return the .rela.plt section data.
  Reloc_section*
  rela_plt()
  { return this->rel_; }

  // Return whether we created a section for IRELATIVE relocations.
  bool
  has_irelative_section() const
  { return this->irelative_rel_ != NULL; }

  // Return the number of PLT entries.
  unsigned int
  entry_count() const
  { return this->count_; }

  // Return the offset of the first non-reserved PLT entry.
  unsigned int
  first_plt_entry_offset()
  { return this->do_first_plt_entry_offset(); }

  // Return the size of a PLT entry.
  unsigned int
  get_plt_entry_size() const
  { return this->do_get_plt_entry_size(); }

  // Return the PLT address to use for a global symbol.
  uint64_t
  address_for_global(const Symbol*);

  // Return the PLT address to use for a local symbol.
  uint64_t
  address_for_local(const Relobj*, unsigned int symndx);

 protected:
  // Fill in the first PLT entry.
  void
  fill_first_plt_entry(unsigned char* pov,
		       Address got_address,
		       Address plt_address)
  { this->do_fill_first_plt_entry(pov, got_address, plt_address); }

  // Fill in a normal PLT entry.
  void
  fill_plt_entry(unsigned char* pov,
		 Address got_address,
		 Address plt_address,
		 unsigned int got_offset,
		 unsigned int plt_offset)
  {
    this->do_fill_plt_entry(pov, got_address, plt_address,
			    got_offset, plt_offset);
  }

  virtual unsigned int
  do_first_plt_entry_offset() const = 0;

  virtual unsigned int
  do_get_plt_entry_size() const = 0;

  virtual void
  do_fill_first_plt_entry(unsigned char* pov,
			  Address got_addr,
			  Address plt_addr) = 0;

  virtual void
  do_fill_plt_entry(unsigned char* pov,
		    Address got_address,
		    Address plt_address,
		    unsigned int got_offset,
		    unsigned int plt_offset) = 0;

  void
  do_adjust_output_section(Output_section* os);

  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** PLT")); }

 private:
  // Set the final size.
  void
  set_final_data_size();

  // Write out the PLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  // The IRELATIVE relocs, if necessary.  These must follow the
  // regular PLT relocations.
  Reloc_section* irelative_rel_;
  // The .got section.
  Output_data_got_aarch64<size, big_endian>* got_;
  // The .got.plt section.
  Output_data_space* got_plt_;
  // The number of PLT entries.
  unsigned int count_;
};

// Initialize the PLT section.

template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::init(Layout* layout)
{
  this->rel_ = new Reloc_section(false);
  layout->add_output_section_data(".rela.plt", elfcpp::SHT_RELA,
				  elfcpp::SHF_ALLOC, this->rel_,
				  ORDER_DYNAMIC_PLT_RELOCS, false);
}

template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::do_adjust_output_section(
    Output_section* os)
{
  os->set_entsize(this->get_plt_entry_size());
}

// Add an entry to the PLT.

template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::add_entry(Symbol* gsym)
{
  gold_assert(!gsym->has_plt_offset());

  gsym->set_plt_offset((this->count_) * this->get_plt_entry_size()
		       + this->first_plt_entry_offset());

  ++this->count_;

  section_offset_type got_offset = this->got_plt_->current_data_size();

  // Every PLT entry needs a GOT entry which points back to the PLT
  // entry (this will be changed by the dynamic linker, normally
  // lazily when the function is called).
  this->got_plt_->set_current_data_size(got_offset + size / 8);

  // Every PLT entry needs a reloc.
  gsym->set_needs_dynsym_entry();
  this->rel_->add_global(gsym, elfcpp::R_AARCH64_JUMP_SLOT,
			 this->got_plt_, got_offset, 0);

  // Note that we don't need to save the symbol. The contents of the
  // PLT are independent of which symbols are used. The symbols only
  // appear in the relocations.
}

// Return the PLT address to use for a global symbol.

template<int size, bool big_endian>
uint64_t
Output_data_plt_aarch64<size, big_endian>::address_for_global(
  const Symbol* gsym)
{
  uint64_t offset = 0;
  if (gsym->type() == elfcpp::STT_GNU_IFUNC
      && gsym->can_use_relative_reloc(false))
    offset = (this->first_plt_entry_offset() +
	      this->count_ * this->get_plt_entry_size());
  return this->address() + offset + gsym->plt_offset();
}

// Return the PLT address to use for a local symbol.  These are always
// IRELATIVE relocs.

template<int size, bool big_endian>
uint64_t
Output_data_plt_aarch64<size, big_endian>::address_for_local(
    const Relobj* object,
    unsigned int r_sym)
{
  return (this->address()
	  + this->first_plt_entry_offset()
	  + this->count_ * this->get_plt_entry_size()
	  + object->local_plt_offset(r_sym));
}

// Set the final size.

template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::set_final_data_size()
{
  this->set_data_size(this->first_plt_entry_offset()
                      + this->count_ * this->get_plt_entry_size());
}

template<int size, bool big_endian>
class Output_data_plt_aarch64_standard :
  public Output_data_plt_aarch64<size, big_endian>
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  Output_data_plt_aarch64_standard(Layout* layout, Output_data_space* got_plt)
    : Output_data_plt_aarch64<size, big_endian>(layout,
						size == 32 ? 4 : 8,
						got_plt)
  { }

 protected:
  // Return the offset of the first non-reserved PLT entry.
  virtual unsigned int
  do_first_plt_entry_offset() const
  { return this->first_plt_entry_size; }

  // Return the size of a PLT entry
  virtual unsigned int
  do_get_plt_entry_size() const
  { return this->plt_entry_size; }

  virtual void
  do_fill_first_plt_entry(unsigned char* pov,
			  Address got_address,
			  Address plt_address);

  virtual void
  do_fill_plt_entry(unsigned char* pov,
		    Address got_address,
		    Address plt_address,
		    unsigned int got_offset,
		    unsigned int plt_offset);

 private:
  // The size of the first plt entry size.
  static const int first_plt_entry_size = 32;
  // The size of the plt entry size.
  static const int plt_entry_size = 16;
  // Template for the first PLT entry.
  static const uint32_t first_plt_entry[first_plt_entry_size / 4];
  // Template for subsequent PLT entries.
  static const uint32_t plt_entry[plt_entry_size / 4];
};

// The first entry in the PLT for an executable.

template<>
const uint32_t
Output_data_plt_aarch64_standard<32, false>::
    first_plt_entry[first_plt_entry_size / 4] =
{
  0xa9bf7bf0,	/* stp x16, x30, [sp, #-16]!  */
  0x90000010,	/* adrp x16, PLT_GOT+0x8  */
  0xb9400A11,	/* ldr w17, [x16, #PLT_GOT+0x8]  */
  0x11002210,	/* add w16, w16,#PLT_GOT+0x8   */
  0xd61f0220,	/* br x17  */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<32, true>::
    first_plt_entry[first_plt_entry_size / 4] =
{
  0xa9bf7bf0,	/* stp x16, x30, [sp, #-16]!  */
  0x90000010,	/* adrp x16, PLT_GOT+0x8  */
  0xb9400A11,	/* ldr w17, [x16, #PLT_GOT+0x8]  */
  0x11002210,	/* add w16, w16,#PLT_GOT+0x8   */
  0xd61f0220,	/* br x17  */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<64, false>::
    first_plt_entry[first_plt_entry_size / 4] =
{
  0xa9bf7bf0,	/* stp x16, x30, [sp, #-16]!  */
  0x90000010,	/* adrp x16, PLT_GOT+16  */
  0xf9400A11,	/* ldr x17, [x16, #PLT_GOT+0x10]  */
  0x91004210,	/* add x16, x16,#PLT_GOT+0x10   */
  0xd61f0220,	/* br x17  */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<64, true>::
    first_plt_entry[first_plt_entry_size / 4] =
{
  0xa9bf7bf0,	/* stp x16, x30, [sp, #-16]!  */
  0x90000010,	/* adrp x16, PLT_GOT+16  */
  0xf9400A11,	/* ldr x17, [x16, #PLT_GOT+0x10]  */
  0x91004210,	/* add x16, x16,#PLT_GOT+0x10   */
  0xd61f0220,	/* br x17  */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
  0xd503201f,	/* nop */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<32, false>::
    plt_entry[plt_entry_size / 4] =
{
  0x90000010,	/* adrp x16, PLTGOT + n * 4  */
  0xb9400211,	/* ldr w17, [w16, PLTGOT + n * 4] */
  0x11000210,	/* add w16, w16, :lo12:PLTGOT + n * 4  */
  0xd61f0220,	/* br x17.  */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<32, true>::
    plt_entry[plt_entry_size / 4] =
{
  0x90000010,	/* adrp x16, PLTGOT + n * 4  */
  0xb9400211,	/* ldr w17, [w16, PLTGOT + n * 4] */
  0x11000210,	/* add w16, w16, :lo12:PLTGOT + n * 4  */
  0xd61f0220,	/* br x17.  */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<64, false>::
    plt_entry[plt_entry_size / 4] =
{
  0x90000010,	/* adrp x16, PLTGOT + n * 8  */
  0xf9400211,	/* ldr x17, [x16, PLTGOT + n * 8] */
  0x91000210,	/* add x16, x16, :lo12:PLTGOT + n * 8  */
  0xd61f0220,	/* br x17.  */
};

template<>
const uint32_t
Output_data_plt_aarch64_standard<64, true>::
    plt_entry[plt_entry_size / 4] =
{
  0x90000010,	/* adrp x16, PLTGOT + n * 8  */
  0xf9400211,	/* ldr x17, [x16, PLTGOT + n * 8] */
  0x91000210,	/* add x16, x16, :lo12:PLTGOT + n * 8  */
  0xd61f0220,	/* br x17.  */
};

template<int size, bool big_endian>
void
Output_data_plt_aarch64_standard<size, big_endian>::do_fill_first_plt_entry(
    unsigned char* pov,
    Address got_address,
    Address plt_address)
{
  // PLT0 of the small PLT looks like this in ELF64 -
  // stp x16, x30, [sp, #-16]!	 	Save the reloc and lr on stack.
  // adrp x16, PLT_GOT + 16		Get the page base of the GOTPLT
  // ldr  x17, [x16, #:lo12:PLT_GOT+16]	Load the address of the
  // 					symbol resolver
  // add  x16, x16, #:lo12:PLT_GOT+16	Load the lo12 bits of the
  // 					GOTPLT entry for this.
  // br   x17
  // PLT0 will be slightly different in ELF32 due to different got entry
  // size.
  memcpy(pov, this->first_plt_entry, this->first_plt_entry_size);
  Address gotplt_2nd_ent = got_address + (size / 8) * 2;

  // Fill in the top 21 bits for this: ADRP x16, PLT_GOT + 8 * 2.
  // ADRP:  (PG(S+A)-PG(P)) >> 12) & 0x1fffff.
  // FIXME: This only works for 64bit
  AArch64_relocate_functions<size, big_endian>::adrp(pov + 4,
      gotplt_2nd_ent, plt_address + 4);

  // Fill in R_AARCH64_LDST8_LO12
  elfcpp::Swap<32, big_endian>::writeval(
      pov + 8,
      ((this->first_plt_entry[2] & 0xffc003ff)
       | ((gotplt_2nd_ent & 0xff8) << 7)));

  // Fill in R_AARCH64_ADD_ABS_LO12
  elfcpp::Swap<32, big_endian>::writeval(
      pov + 12,
      ((this->first_plt_entry[3] & 0xffc003ff)
       | ((gotplt_2nd_ent & 0xfff) << 10)));
}

// Subsequent entries in the PLT for an executable.
// FIXME: This only works for 64bit

template<int size, bool big_endian>
void
Output_data_plt_aarch64_standard<size, big_endian>::do_fill_plt_entry(
    unsigned char* pov,
    Address got_address,
    Address plt_address,
    unsigned int got_offset,
    unsigned int plt_offset)
{
  memcpy(pov, this->plt_entry, this->plt_entry_size);

  Address gotplt_entry_address = got_address + got_offset;
  Address plt_entry_address = plt_address + plt_offset;

  // Fill in R_AARCH64_PCREL_ADR_HI21
  AArch64_relocate_functions<size, big_endian>::adrp(
      pov,
      gotplt_entry_address,
      plt_entry_address);

  // Fill in R_AARCH64_LDST64_ABS_LO12
  elfcpp::Swap<32, big_endian>::writeval(
      pov + 4,
      ((this->plt_entry[1] & 0xffc003ff)
       | ((gotplt_entry_address & 0xff8) << 7)));

  // Fill in R_AARCH64_ADD_ABS_LO12
  elfcpp::Swap<32, big_endian>::writeval(
      pov + 8,
      ((this->plt_entry[2] & 0xffc003ff)
       | ((gotplt_entry_address & 0xfff) <<10)));

}

// Write out the PLT.  This uses the hand-coded instructions above,
// and adjusts them as needed.  This is specified by the AMD64 ABI.

template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::do_write(Output_file* of)
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

  // The base address of the .plt section.
  typename elfcpp::Elf_types<size>::Elf_Addr plt_address = this->address();
  // The base address of the PLT portion of the .got section.
  typename elfcpp::Elf_types<size>::Elf_Addr got_address
    = this->got_plt_->address();

  this->fill_first_plt_entry(pov, got_address, plt_address);
  pov += this->first_plt_entry_offset();

  // The first three entries in .got.plt are reserved.
  unsigned char* got_pov = got_view;
  memset(got_pov, 0, size / 8 * AARCH64_GOTPLT_RESERVE_COUNT);
  got_pov += (size / 8) * AARCH64_GOTPLT_RESERVE_COUNT;

  unsigned int plt_offset = this->first_plt_entry_offset();
  unsigned int got_offset = (size / 8) * AARCH64_GOTPLT_RESERVE_COUNT;
  const unsigned int count = this->count_;
  for (unsigned int plt_index = 0;
       plt_index < count;
       ++plt_index,
	 pov += this->get_plt_entry_size(),
	 got_pov += size / 8,
	 plt_offset += this->get_plt_entry_size(),
	 got_offset += size / 8)
    {
      // Set and adjust the PLT entry itself.
      this->fill_plt_entry(pov, got_address, plt_address,
			   got_offset, plt_offset);

      // Set the entry in the GOT, which points to plt0.
      elfcpp::Swap<size, big_endian>::writeval(got_pov, plt_address);
    }

  gold_assert(static_cast<section_size_type>(pov - oview) == oview_size);
  gold_assert(static_cast<section_size_type>(got_pov - got_view) == got_size);

  of->write_output_view(offset, oview_size, oview);
  of->write_output_view(got_file_offset, got_size, got_view);
}

// Telling how to update the immediate field of an instruction.
struct AArch64_howto
{
  // The immediate field mask.
  elfcpp::Elf_Xword dst_mask;

  // The offset to apply relocation immediate
  int doffset;

  // The second part offset, if the immediate field has two parts.
  // -1 if the immediate field has only one part.
  int doffset2;
};

static const AArch64_howto aarch64_howto[AArch64_reloc_property::INST_NUM] =
{
  {0, -1, -1},		// DATA
  {0x1fffe0, 5, -1},	// MOVW  [20:5]-imm16
  {0xffffe0, 5, -1},	// LD    [23:5]-imm19
  {0x60ffffe0, 29, 5},	// ADR   [30:29]-immlo  [23:5]-immhi
  {0x60ffffe0, 29, 5},	// ADRP  [30:29]-immlo  [23:5]-immhi
  {0x3ffc00, 10, -1},	// ADD   [21:10]-imm12
  {0x3ffc00, 10, -1},	// LDST  [21:10]-imm12
  {0x7ffe0, 5, -1},	// TBZNZ [18:5]-imm14
  {0xffffe0, 5, -1},	// CONDB [23:5]-imm19
  {0x3ffffff, 0, -1},	// B     [25:0]-imm26
  {0x3ffffff, 0, -1},	// CALL  [25:0]-imm26
};

// AArch64 relocate function class

template<int size, bool big_endian>
class AArch64_relocate_functions
{
 public:
  typedef enum
  {
    STATUS_OKAY,	// No error during relocation.
    STATUS_OVERFLOW,	// Relocation overflow.
    STATUS_BAD_RELOC,	// Relocation cannot be applied.
  } Status;

 private:
  typedef AArch64_relocate_functions<size, big_endian> This;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  // Return the page address of the address.
  // Page(address) = address & ~0xFFF

  static inline typename elfcpp::Swap<size, big_endian>::Valtype
  Page(Address address)
  {
    return (address & (~static_cast<Address>(0xFFF)));
  }

  // Update instruction (pointed by view) with selected bits (immed).
  // val = (val & ~dst_mask) | (immed << doffset)

  template<int valsize>
  static inline void
  update_view(unsigned char* view,
	      typename elfcpp::Swap<size, big_endian>::Valtype immed,
	      elfcpp::Elf_Xword doffset,
	      elfcpp::Elf_Xword dst_mask)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);

    // Clear immediate fields.
    val &= ~dst_mask;
    elfcpp::Swap<valsize, big_endian>::writeval(wv,
      static_cast<Valtype>(val | (immed << doffset)));
  }

  // Update two parts of an instruction (pointed by view) with selected
  // bits (immed1 and immed2).
  // val = (val & ~dst_mask) | (immed1 << doffset1) | (immed2 << doffset2)

  template<int valsize>
  static inline void
  update_view_two_parts(
    unsigned char* view,
    typename elfcpp::Swap<size, big_endian>::Valtype immed1,
    typename elfcpp::Swap<size, big_endian>::Valtype immed2,
    elfcpp::Elf_Xword doffset1,
    elfcpp::Elf_Xword doffset2,
    elfcpp::Elf_Xword dst_mask)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    val &= ~dst_mask;
    elfcpp::Swap<valsize, big_endian>::writeval(wv,
      static_cast<Valtype>(val | (immed1 << doffset1) |
			   (immed2 << doffset2)));
  }

  // Update adr or adrp instruction with [32:12] of X.
  // In adr and adrp: [30:29] immlo   [23:5] immhi

  static inline void
  update_adr(unsigned char* view,
	     typename elfcpp::Swap<size, big_endian>::Valtype x,
	     const AArch64_reloc_property* /* reloc_property */)
  {
    elfcpp::Elf_Xword dst_mask = (0x3 << 29) | (0x7ffff << 5);
    typename elfcpp::Swap<32, big_endian>::Valtype immed =
      (x >> 12) & 0x1fffff;
    This::template update_view_two_parts<32>(
      view,
      immed & 0x3,
      (immed & 0x1ffffc) >> 2,
      29,
      5,
      dst_mask);
  }

 public:

  // Do a simple rela relocation at unaligned addresses.

  template<int valsize>
  static inline typename This::Status
  rela_ua(unsigned char* view,
	  const Sized_relobj_file<size, big_endian>* object,
	  const Symbol_value<size>* psymval,
	  typename elfcpp::Swap<size, big_endian>::Valtype addend,
	  const AArch64_reloc_property* reloc_property)
  {
    typedef typename elfcpp::Swap_unaligned<valsize, big_endian>::Valtype
      Valtype;
    typename elfcpp::Elf_types<size>::Elf_Addr x =
	psymval->value(object, addend);
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view,
      static_cast<Valtype>(x));
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Do a simple pc-relative relocation at unaligned addresses.

  template<int valsize>
  static inline typename This::Status
  pcrela_ua(unsigned char* view,
	    const Sized_relobj_file<size, big_endian>* object,
	    const Symbol_value<size>* psymval,
	    typename elfcpp::Swap<size, big_endian>::Valtype addend,
	    Address address,
	    const AArch64_reloc_property* reloc_property)
  {
    typedef typename elfcpp::Swap_unaligned<valsize, big_endian>::Valtype
      Valtype;
    Address x =	psymval->value(object, addend) - address;
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view,
      static_cast<Valtype>(x));
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Do a simple rela relocation at aligned addresses.

  template<int valsize>
  static inline typename This::Status
  rela(
    unsigned char* view,
    const Sized_relobj_file<size, big_endian>* object,
    const Symbol_value<size>* psymval,
    typename elfcpp::Swap<size, big_endian>::Valtype addend,
    const AArch64_reloc_property* reloc_property)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype
      Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Address x =	psymval->value(object, addend);
    elfcpp::Swap<valsize, big_endian>::writeval(wv,
      static_cast<Valtype>(x));
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Do relocate. Update selected bits in text.
  // new_val = (val & ~dst_mask) | (immed << doffset)

  template<int valsize>
  static inline typename This::Status
  rela_general(unsigned char* view,
	       const Sized_relobj_file<size, big_endian>* object,
	       const Symbol_value<size>* psymval,
	       typename elfcpp::Swap<size, big_endian>::Valtype addend,
	       const AArch64_reloc_property* reloc_property)
  {
    // Calculate relocation.
    Address x =	psymval->value(object, addend);

    // Select bits from X.
    Address immed = reloc_property->select_x_value(x);

    // Update view.
    const AArch64_reloc_property::Reloc_inst inst =
      reloc_property->reloc_inst();
    // If it is a data relocation or instruction has 2 parts of immediate
    // fields, you should not call rela_general.
    gold_assert(aarch64_howto[inst].doffset2 == -1 &&
		aarch64_howto[inst].doffset != -1);
    This::template update_view<valsize>(view, immed,
					aarch64_howto[inst].doffset,
					aarch64_howto[inst].dst_mask);

    // Do check overflow or alignment if needed.
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Do relocate. Update selected bits in text.
  // new val = (val & ~dst_mask) | (immed << doffset)

  template<int valsize>
  static inline typename This::Status
  rela_general(
    unsigned char* view,
    typename elfcpp::Swap<size, big_endian>::Valtype s,
    typename elfcpp::Swap<size, big_endian>::Valtype addend,
    const AArch64_reloc_property* reloc_property)
  {
    // Calculate relocation.
    Address x = s + addend;

    // Select bits from X.
    Address immed = reloc_property->select_x_value(x);

    // Update view.
    const AArch64_reloc_property::Reloc_inst inst =
      reloc_property->reloc_inst();
    // If it is a data relocation or instruction has 2 parts of immediate
    // fields, you should not call rela_general.
    gold_assert(aarch64_howto[inst].doffset2 == -1 &&
		aarch64_howto[inst].doffset != -1);
    This::template update_view<valsize>(view, immed,
					aarch64_howto[inst].doffset,
					aarch64_howto[inst].dst_mask);

    // Do check overflow or alignment if needed.
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Do address relative relocate. Update selected bits in text.
  // new val = (val & ~dst_mask) | (immed << doffset)

  template<int valsize>
  static inline typename This::Status
  pcrela_general(
    unsigned char* view,
    const Sized_relobj_file<size, big_endian>* object,
    const Symbol_value<size>* psymval,
    typename elfcpp::Swap<size, big_endian>::Valtype addend,
    Address address,
    const AArch64_reloc_property* reloc_property)
  {
    // Calculate relocation.
    Address x =	psymval->value(object, addend) - address;

    // Select bits from X.
    Address immed = reloc_property->select_x_value(x);

    // Update view.
    const AArch64_reloc_property::Reloc_inst inst =
      reloc_property->reloc_inst();
    // If it is a data relocation or instruction has 2 parts of immediate
    // fields, you should not call pcrela_general.
    gold_assert(aarch64_howto[inst].doffset2 == -1 &&
		aarch64_howto[inst].doffset != -1);
    This::template update_view<valsize>(view, immed,
					aarch64_howto[inst].doffset,
					aarch64_howto[inst].dst_mask);

    // Do check overflow or alignment if needed.
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

  // Calculate PG(S+A) - PG(address), update adrp instruction.
  // R_AARCH64_ADR_PREL_PG_HI21

  static inline typename This::Status
  adrp(
    unsigned char* view,
    Address sa,
    Address address)
  {
    typename elfcpp::Swap<size, big_endian>::Valtype x =
      This::Page(sa) - This::Page(address);
    update_adr(view, x, NULL);
    return (size == 64 && Bits<32>::has_overflow(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // Calculate PG(S+A) - PG(address), update adrp instruction.
  // R_AARCH64_ADR_PREL_PG_HI21

  static inline typename This::Status
  adrp(unsigned char* view,
       const Sized_relobj_file<size, big_endian>* object,
       const Symbol_value<size>* psymval,
       Address addend,
       Address address,
       const AArch64_reloc_property* reloc_property)
  {
    Address sa = psymval->value(object, addend);
    typename elfcpp::Swap<size, big_endian>::Valtype x =
	This::Page(sa) - This::Page(address);
    update_adr(view, x, reloc_property);
    return (reloc_property->checkup_x_value(x)
	    ? This::STATUS_OKAY
	    : This::STATUS_OVERFLOW);
  }

};

// Return the number of entries in the PLT.

template<int size, bool big_endian>
unsigned int
Target_aarch64<size, big_endian>::plt_entry_count() const
{
  if (this->plt_ == NULL)
    return 0;
  return this->plt_->entry_count();
}

// Return the offset of the first non-reserved PLT entry.

template<int size, bool big_endian>
unsigned int
Target_aarch64<size, big_endian>::first_plt_entry_offset() const
{
  return this->plt_->first_plt_entry_offset();
}

// Return the size of each PLT entry.

template<int size, bool big_endian>
unsigned int
Target_aarch64<size, big_endian>::plt_entry_size() const
{
  return this->plt_->get_plt_entry_size();
}

// Optimize the TLS relocation type based on what we know about the
// symbol.  IS_FINAL is true if the final address of this symbol is
// known at link time.

template<int size, bool big_endian>
tls::Tls_optimization
Target_aarch64<size, big_endian>::optimize_tls_reloc(bool /* is_final */,
						     int /* r_type */)
{
  //TODO
  return tls::TLSOPT_NONE;
}

// Returns true if this relocation type could be that of a function pointer.

template<int size, bool big_endian>
inline bool
Target_aarch64<size, big_endian>::Scan::possible_function_pointer_reloc(
  unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_AARCH64_ABS64:
    //TODO
      {
	return true;
      }
    }
  return false;
}

// For safe ICF, scan a relocation for a local symbol to check if it
// corresponds to a function pointer being taken.  In that case mark
// the function whose pointer was taken as not foldable.

template<int size, bool big_endian>
inline bool
Target_aarch64<size, big_endian>::Scan::local_reloc_may_be_function_pointer(
  Symbol_table* ,
  Layout* ,
  Target_aarch64<size, big_endian>* ,
  Sized_relobj_file<size, big_endian>* ,
  unsigned int ,
  Output_section* ,
  const elfcpp::Rela<size, big_endian>& ,
  unsigned int r_type,
  const elfcpp::Sym<size, big_endian>&)
{
  // When building a shared library, do not fold any local symbols as it is
  // not possible to distinguish pointer taken versus a call by looking at
  // the relocation types.
  return (parameters->options().shared()
	  || possible_function_pointer_reloc(r_type));
}

// For safe ICF, scan a relocation for a global symbol to check if it
// corresponds to a function pointer being taken.  In that case mark
// the function whose pointer was taken as not foldable.

template<int size, bool big_endian>
inline bool
Target_aarch64<size, big_endian>::Scan::global_reloc_may_be_function_pointer(
  Symbol_table* ,
  Layout* ,
  Target_aarch64<size, big_endian>* ,
  Sized_relobj_file<size, big_endian>* ,
  unsigned int ,
  Output_section* ,
  const elfcpp::Rela<size, big_endian>& ,
  unsigned int r_type,
  Symbol* gsym)
{
  // When building a shared library, do not fold symbols whose visibility
  // is hidden, internal or protected.
  return ((parameters->options().shared()
	   && (gsym->visibility() == elfcpp::STV_INTERNAL
	       || gsym->visibility() == elfcpp::STV_PROTECTED
	       || gsym->visibility() == elfcpp::STV_HIDDEN))
	  || possible_function_pointer_reloc(r_type));
}

// Report an unsupported relocation against a local symbol.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::Scan::unsupported_reloc_local(
     Sized_relobj_file<size, big_endian>* object,
     unsigned int r_type)
{
  gold_error(_("%s: unsupported reloc %u against local symbol"),
	     object->name().c_str(), r_type);
}

// We are about to emit a dynamic relocation of type R_TYPE.  If the
// dynamic linker does not support it, issue an error.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::Scan::check_non_pic(Relobj* object,
						      unsigned int r_type)
{
  gold_assert(r_type != elfcpp::R_AARCH64_NONE);

  switch (r_type)
    {
    // These are the relocation types supported by glibc for AARCH64.
    case elfcpp::R_AARCH64_NONE:
    case elfcpp::R_AARCH64_COPY:
    case elfcpp::R_AARCH64_GLOB_DAT:
    case elfcpp::R_AARCH64_JUMP_SLOT:
    case elfcpp::R_AARCH64_RELATIVE:
    case elfcpp::R_AARCH64_TLS_DTPREL64:
    case elfcpp::R_AARCH64_TLS_DTPMOD64:
    case elfcpp::R_AARCH64_TLS_TPREL64:
    case elfcpp::R_AARCH64_TLSDESC:
    case elfcpp::R_AARCH64_IRELATIVE:
    case elfcpp::R_AARCH64_ABS32:
    case elfcpp::R_AARCH64_ABS64:
      return;

    default:
      break;
    }

  // This prevents us from issuing more than one error per reloc
  // section. But we can still wind up issuing more than one
  // error per object file.
  if (this->issued_non_pic_error_)
    return;
  gold_assert(parameters->options().output_is_position_independent());
  object->error(_("requires unsupported dynamic reloc; "
		  "recompile with -fPIC"));
  this->issued_non_pic_error_ = true;
  return;
}

// Scan a relocation for a local symbol.

template<int size, bool big_endian>
inline void
Target_aarch64<size, big_endian>::Scan::local(
    Symbol_table* /* symtab */,
    Layout* /* layout */,
    Target_aarch64<size, big_endian>* /* target */,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int /* data_shndx */,
    Output_section* /* output_section */,
    const elfcpp::Rela<size, big_endian>& /* reloc */,
    unsigned int r_type,
    const elfcpp::Sym<size, big_endian>& /* lsym */,
    bool is_discarded)
{
  if (is_discarded)
    return;

  switch (r_type)
    {
    case elfcpp::R_AARCH64_ABS64:
    case elfcpp::R_AARCH64_ABS32:
    case elfcpp::R_AARCH64_ABS16:
      // If building a shared library or pie, we need to mark this as a dynmic
      // reloction, so that the dynamic loader can relocate it.
      // Not supported yet.
      if (parameters->options().output_is_position_independent())
	{
	  gold_error(_("%s: unsupported ABS64 relocation type for pie or "
		       "shared library.\n"),
		     object->name().c_str());
	}
      break;

      // Relocations to generate 19, 21 and 33-bit PC-relative address
    case elfcpp::R_AARCH64_ADR_PREL_PG_HI21: // 275
    case elfcpp::R_AARCH64_LDST8_ABS_LO12_NC: // 278
    case elfcpp::R_AARCH64_LDST64_ABS_LO12_NC: // 286
    case elfcpp::R_AARCH64_ADD_ABS_LO12_NC: // 277
      break;

      // Control flow, pc-relative. We don't need to do anything for a relative
      // addressing relocation against a local symbol if it does not reference
      // the GOT.
    case elfcpp::R_AARCH64_CALL26: // 283
      break;

    default:
      unsupported_reloc_local(object, r_type);
    }
}


// Report an unsupported relocation against a global symbol.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::Scan::unsupported_reloc_global(
    Sized_relobj_file<size, big_endian>* object,
    unsigned int r_type,
    Symbol* gsym)
{
  gold_error(_("%s: unsupported reloc %u against global symbol %s"),
	     object->name().c_str(), r_type, gsym->demangled_name().c_str());
}

template<int size, bool big_endian>
inline void
Target_aarch64<size, big_endian>::Scan::global(
    Symbol_table* symtab,
    Layout* layout,
    Target_aarch64<size, big_endian>* target,
    Sized_relobj_file<size, big_endian>* /* object */,
    unsigned int /* data_shndx */,
    Output_section* /* output_section */,
    const elfcpp::Rela<size, big_endian>& /* reloc */,
    unsigned int r_type,
    Symbol* gsym)
{
  switch (r_type)
    {
    case elfcpp::R_AARCH64_ABS64:
      // This is used to fill the GOT absolute address.
      if (gsym->needs_plt_entry())
	{
	  target->make_plt_entry(symtab, layout, gsym);
	}
      break;

    case elfcpp::R_AARCH64_ADR_PREL_PG_HI21:
    case elfcpp::R_AARCH64_ADR_PREL_PG_HI21_NC:
    case elfcpp::R_AARCH64_ADD_ABS_LO12_NC:
      {
	// Do nothing here.
	break;
      }

    case elfcpp::R_AARCH64_ADR_GOT_PAGE:
    case elfcpp::R_AARCH64_LD64_GOT_LO12_NC:
      {
	// This pair of relocations is used to access a specific GOT entry.
	// Note a GOT entry is an *address* to a symbol.
	// The symbol requires a GOT entry
	Output_data_got_aarch64<size, big_endian>* got =
	  target->got_section(symtab, layout);
	if (gsym->final_value_is_known())
	  {
	    got->add_global(gsym, GOT_TYPE_STANDARD);
	  }
	else
	  {
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    if (gsym->is_from_dynobj()
		|| gsym->is_undefined()
		|| gsym->is_preemptible()
		|| (gsym->visibility() == elfcpp::STV_PROTECTED
		    && parameters->options().shared()))
	      got->add_global_with_rel(gsym, GOT_TYPE_STANDARD,
				       rela_dyn, elfcpp::R_AARCH64_GLOB_DAT);
	    else
	      {
		// Not implemented yet.
		gold_assert(false);
	      }
	  }
	break;
      }

    case elfcpp::R_AARCH64_JUMP26:
    case elfcpp::R_AARCH64_CALL26:
      {
	if (gsym->final_value_is_known())
	  break;

	if (gsym->is_defined() &&
	    !gsym->is_from_dynobj() &&
	    !gsym->is_preemptible())
	  break;

	// Make plt entry for function call.
	const AArch64_reloc_property* arp =
	    aarch64_reloc_property_table->get_reloc_property(r_type);
	gold_assert(arp != NULL);
	target->make_plt_entry(symtab, layout, gsym);
	break;
      }

    default:
      gold_error(_("%s: unsupported reloc type"),
		 aarch64_reloc_property_table->
		   reloc_name_in_error_message(r_type).c_str());
    }
  return;
}  // End of Scan::global

// Create the PLT section.
template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::make_plt_section(
  Symbol_table* symtab, Layout* layout)
{
  if (this->plt_ == NULL)
    {
      // Create the GOT section first.
      this->got_section(symtab, layout);

      this->plt_ = this->make_data_plt(layout, this->got_plt_);

      layout->add_output_section_data(".plt", elfcpp::SHT_PROGBITS,
				      (elfcpp::SHF_ALLOC
				       | elfcpp::SHF_EXECINSTR),
				      this->plt_, ORDER_PLT, false);

      // Make the sh_info field of .rela.plt point to .plt.
      Output_section* rela_plt_os = this->plt_->rela_plt()->output_section();
      rela_plt_os->set_info_section(this->plt_->output_section());
    }
}

// Create a PLT entry for a global symbol.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::make_plt_entry(
    Symbol_table* symtab,
    Layout* layout,
    Symbol* gsym)
{
  if (gsym->has_plt_offset())
    return;

  if (this->plt_ == NULL)
    this->make_plt_section(symtab, layout);

  this->plt_->add_entry(gsym);
}

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::gc_process_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols)
{
  if (sh_type == elfcpp::SHT_REL)
    {
      return;
    }

  gold::gc_process_relocs<
    size, big_endian,
    Target_aarch64<size, big_endian>,
    elfcpp::SHT_RELA,
    typename Target_aarch64<size, big_endian>::Scan,
    typename Target_aarch64<size, big_endian>::Relocatable_size_for_reloc>(
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

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::scan_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols)
{
  if (sh_type == elfcpp::SHT_REL)
    {
      gold_error(_("%s: unsupported REL reloc section"),
		 object->name().c_str());
      return;
    }
  gold::scan_relocs<size, big_endian, Target_aarch64, elfcpp::SHT_RELA, Scan>(
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

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::do_finalize_sections(
    Layout* layout,
    const Input_objects*,
    Symbol_table* symtab)
{
  const Reloc_section* rel_plt = (this->plt_ == NULL
				  ? NULL
				  : this->plt_->rela_plt());
  layout->add_target_dynamic_tags(false, this->got_plt_, rel_plt,
				  this->rela_dyn_, true, false);

  // Set the size of the _GLOBAL_OFFSET_TABLE_ symbol to the size of
  // the .got.plt section.
  Symbol* sym = this->global_offset_table_;
  if (sym != NULL)
    {
      uint64_t data_size = this->got_plt_->current_data_size();
      symtab->get_sized_symbol<size>(sym)->set_symsize(data_size);

      // If the .got section is more than 0x8000 bytes, we add
      // 0x8000 to the value of _GLOBAL_OFFSET_TABLE_, so that 16
      // bit relocations have a greater chance of working.
      if (data_size >= 0x8000)
	symtab->get_sized_symbol<size>(sym)->set_value(
	  symtab->get_sized_symbol<size>(sym)->value() + 0x8000);
    }

  if (parameters->doing_static_link()
      && (this->plt_ == NULL || !this->plt_->has_irelative_section()))
    {
      // If linking statically, make sure that the __rela_iplt symbols
      // were defined if necessary, even if we didn't create a PLT.
      static const Define_symbol_in_segment syms[] =
	{
	  {
	    "__rela_iplt_start",	// name
	    elfcpp::PT_LOAD,		// segment_type
	    elfcpp::PF_W,		// segment_flags_set
	    elfcpp::PF(0),		// segment_flags_clear
	    0,				// value
	    0,				// size
	    elfcpp::STT_NOTYPE,		// type
	    elfcpp::STB_GLOBAL,		// binding
	    elfcpp::STV_HIDDEN,		// visibility
	    0,				// nonvis
	    Symbol::SEGMENT_START,	// offset_from_base
	    true			// only_if_ref
	  },
	  {
	    "__rela_iplt_end",		// name
	    elfcpp::PT_LOAD,		// segment_type
	    elfcpp::PF_W,		// segment_flags_set
	    elfcpp::PF(0),		// segment_flags_clear
	    0,				// value
	    0,				// size
	    elfcpp::STT_NOTYPE,		// type
	    elfcpp::STB_GLOBAL,		// binding
	    elfcpp::STV_HIDDEN,		// visibility
	    0,				// nonvis
	    Symbol::SEGMENT_START,	// offset_from_base
	    true			// only_if_ref
	  }
	};

      symtab->define_symbols(layout, 2, syms,
			     layout->script_options()->saw_sections_clause());
    }

  return;
}

// Perform a relocation.

template<int size, bool big_endian>
inline bool
Target_aarch64<size, big_endian>::Relocate::relocate(
    const Relocate_info<size, big_endian>* relinfo,
    Target_aarch64<size, big_endian>* target,
    Output_section* ,
    size_t relnum,
    const elfcpp::Rela<size, big_endian>& rela,
    unsigned int r_type,
    const Sized_symbol<size>* gsym,
    const Symbol_value<size>* psymval,
    unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr address,
    section_size_type /* view_size */)
{
  if (view == NULL)
    return true;

  typedef AArch64_relocate_functions<size, big_endian> Reloc;

  const AArch64_reloc_property* reloc_property =
      aarch64_reloc_property_table->get_reloc_property(r_type);

  if (reloc_property == NULL)
    {
      std::string reloc_name =
	  aarch64_reloc_property_table->reloc_name_in_error_message(r_type);
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("cannot relocate %s in object file"),
			     reloc_name.c_str());
      return true;
    }

  const Sized_relobj_file<size, big_endian>* object = relinfo->object;

  // Pick the value to use for symbols defined in the PLT.
  Symbol_value<size> symval;
  if (gsym != NULL
      && gsym->use_plt_offset(reloc_property->reference_flags()))
    {
      symval.set_output_value(target->plt_address_for_global(gsym));
      psymval = &symval;
    }
  else if (gsym == NULL && psymval->is_ifunc_symbol())
    {
      unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
      if (object->local_has_plt_offset(r_sym))
	{
	  symval.set_output_value(target->plt_address_for_local(object, r_sym));
	  psymval = &symval;
	}
    }

  const elfcpp::Elf_Xword addend = rela.get_r_addend();

  // Get the GOT offset if needed.
  // For aarch64, the GOT pointer points to the start of the GOT section.
  bool have_got_offset = false;
  int got_offset = 0;
  int got_base = (target->got_ != NULL
		  ? (target->got_->current_data_size() >= 0x8000
		     ? 0x8000 : 0)
		  : 0);
  switch (r_type)
    {
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G0:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G0_NC:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G1:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G1_NC:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G2:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G2_NC:
    case elfcpp::R_AARCH64_MOVW_GOTOFF_G3:
    case elfcpp::R_AARCH64_GOTREL64:
    case elfcpp::R_AARCH64_GOTREL32:
    case elfcpp::R_AARCH64_GOT_LD_PREL19:
    case elfcpp::R_AARCH64_LD64_GOTOFF_LO15:
    case elfcpp::R_AARCH64_ADR_GOT_PAGE:
    case elfcpp::R_AARCH64_LD64_GOT_LO12_NC:
    case elfcpp::R_AARCH64_LD64_GOTPAGE_LO15:
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_STANDARD));
	  got_offset = gsym->got_offset(GOT_TYPE_STANDARD) - got_base;
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD));
	  got_offset = (object->local_got_offset(r_sym, GOT_TYPE_STANDARD)
			- got_base);
	}
      have_got_offset = true;
      break;
    default:
      break;
    }

  typename Reloc::Status reloc_status = Reloc::STATUS_OKAY;
  typename elfcpp::Elf_types<size>::Elf_Addr value;
  switch (r_type)
    {
    case elfcpp::R_AARCH64_NONE:
      break;

    case elfcpp::R_AARCH64_ABS64:
      reloc_status = Reloc::template rela_ua<64>(
	view, object, psymval, addend, reloc_property);
      break;

    case elfcpp::R_AARCH64_ABS32:
      reloc_status = Reloc::template rela_ua<32>(
	view, object, psymval, addend, reloc_property);
      break;

    case elfcpp::R_AARCH64_ABS16:
      reloc_status = Reloc::template rela_ua<16>(
	view, object, psymval, addend, reloc_property);
      break;

    case elfcpp::R_AARCH64_PREL64:
      reloc_status = Reloc::template pcrela_ua<64>(
	view, object, psymval, addend, address, reloc_property);

    case elfcpp::R_AARCH64_PREL32:
      reloc_status = Reloc::template pcrela_ua<32>(
	view, object, psymval, addend, address, reloc_property);

    case elfcpp::R_AARCH64_PREL16:
      reloc_status = Reloc::template pcrela_ua<16>(
	view, object, psymval, addend, address, reloc_property);

    case elfcpp::R_AARCH64_ADR_PREL_PG_HI21_NC:
    case elfcpp::R_AARCH64_ADR_PREL_PG_HI21:
      reloc_status = Reloc::adrp(view, object, psymval, addend, address,
				 reloc_property);
      break;

    case elfcpp::R_AARCH64_LDST8_ABS_LO12_NC:
    case elfcpp::R_AARCH64_LDST16_ABS_LO12_NC:
    case elfcpp::R_AARCH64_LDST32_ABS_LO12_NC:
    case elfcpp::R_AARCH64_LDST64_ABS_LO12_NC:
    case elfcpp::R_AARCH64_LDST128_ABS_LO12_NC:
    case elfcpp::R_AARCH64_ADD_ABS_LO12_NC:
      reloc_status = Reloc::template rela_general<32>(
	view, object, psymval, addend, reloc_property);
      break;

    case elfcpp::R_AARCH64_CALL26:
    case elfcpp::R_AARCH64_JUMP26:
      reloc_status = Reloc::template pcrela_general<32>(
	view, object, psymval, addend, address, reloc_property);
      break;

    case elfcpp::R_AARCH64_ADR_GOT_PAGE:
      gold_assert(have_got_offset);
      value = target->got_->address() + got_base + got_offset;
      reloc_status = Reloc::adrp(view, value + addend, address);
      break;

    case elfcpp::R_AARCH64_LD64_GOT_LO12_NC:
      gold_assert(have_got_offset);
      value = target->got_->address() + got_base + got_offset;
      reloc_status = Reloc::template rela_general<32>(
	view, value, addend, reloc_property);
      break;

    default:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("unsupported reloc aaa %u"),
			     r_type);
      break;
    }

  // Report any errors.
  switch (reloc_status)
    {
    case Reloc::STATUS_OKAY:
      break;
    case Reloc::STATUS_OVERFLOW:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("relocation overflow in %s"),
			     reloc_property->name().c_str());
      break;
    case Reloc::STATUS_BAD_RELOC:
      gold_error_at_location(
	  relinfo,
	  relnum,
	  rela.get_r_offset(),
	  _("unexpected opcode while processing relocation %s"),
	  reloc_property->name().c_str());
      break;
    default:
      gold_unreachable();
    }

  return true;
}

// Relocate section data.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::relocate_section(
    const Relocate_info<size, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr address,
    section_size_type view_size,
    const Reloc_symbol_changes* reloc_symbol_changes)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);
  typedef typename Target_aarch64<size, big_endian>::Relocate AArch64_relocate;
  gold::relocate_section<size, big_endian, Target_aarch64, elfcpp::SHT_RELA,
			 AArch64_relocate, gold::Default_comdat_behavior>(
    relinfo,
    this,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    view,
    address,
    view_size,
    reloc_symbol_changes);
}

// Return the size of a relocation while scanning during a relocatable
// link.

template<int size, bool big_endian>
unsigned int
Target_aarch64<size, big_endian>::Relocatable_size_for_reloc::
get_size_for_reloc(
    unsigned int ,
    Relobj* )
{
  // We will never support SHT_REL relocations.
  gold_unreachable();
  return 0;
}

// Scan the relocs during a relocatable link.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::scan_relocatable_relocs(
    Symbol_table* /* symtab */,
    Layout* /* layout */,
    Sized_relobj_file<size, big_endian>* /* object */,
    unsigned int /* data_shndx */,
    unsigned int sh_type,
    const unsigned char* /* prelocs */,
    size_t /* reloc_count */,
    Output_section* /* output_section */,
    bool /* needs_special_offset_handling */,
    size_t /* local_symbol_count */,
    const unsigned char* /* plocal_symbols */,
    Relocatable_relocs* /* rr */)
{
  //TODO
  gold_assert(sh_type == elfcpp::SHT_RELA);
}

// Relocate a section during a relocatable link.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::relocate_relocs(
    const Relocate_info<size, big_endian>* /* relinfo */,
    unsigned int sh_type,
    const unsigned char* /* prelocs */,
    size_t /* reloc_count */,
    Output_section* /* output_section */,
    typename elfcpp::Elf_types<size>::Elf_Off /* offset_in_output_section */,
    const Relocatable_relocs* /* rr */,
    unsigned char* /* view */,
    typename elfcpp::Elf_types<size>::Elf_Addr /* view_address */,
    section_size_type /* view_size */,
    unsigned char* /* reloc_view */,
    section_size_type /* reloc_view_size */)
{
  //TODO
  gold_assert(sh_type == elfcpp::SHT_RELA);
}

// The selector for aarch64 object files.

template<int size, bool big_endian>
class Target_selector_aarch64 : public Target_selector
{
 public:
  Target_selector_aarch64();

  virtual Target*
  do_instantiate_target()
  { return new Target_aarch64<size, big_endian>(); }
};

template<>
Target_selector_aarch64<32, true>::Target_selector_aarch64()
  : Target_selector(elfcpp::EM_AARCH64, 32, true,
		    "elf32-bigaarch64", "aarch64_elf32_be_vec")
{ }

template<>
Target_selector_aarch64<32, false>::Target_selector_aarch64()
  : Target_selector(elfcpp::EM_AARCH64, 32, false,
		    "elf32-littleaarch64", "aarch64_elf32_le_vec")
{ }

template<>
Target_selector_aarch64<64, true>::Target_selector_aarch64()
  : Target_selector(elfcpp::EM_AARCH64, 64, true,
		    "elf64-bigaarch64", "aarch64_elf64_be_vec")
{ }

template<>
Target_selector_aarch64<64, false>::Target_selector_aarch64()
  : Target_selector(elfcpp::EM_AARCH64, 64, false,
		    "elf64-littleaarch64", "aarch64_elf64_le_vec")
{ }

Target_selector_aarch64<32, true> target_selector_aarch64elf32b;
Target_selector_aarch64<32, false> target_selector_aarch64elf32;
Target_selector_aarch64<64, true> target_selector_aarch64elfb;
Target_selector_aarch64<64, false> target_selector_aarch64elf;

} // End anonymous namespace.
