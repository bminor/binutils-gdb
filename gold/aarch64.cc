// aarch64.cc -- aarch64 target support for gold.

// Copyright (C) 2014 Free Software Foundation, Inc.
// Written by Jing Yu <jingyu@google.com>.

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

// Output_data_got_aarch64 class.

template<int size, bool big_endian>
class Output_data_got_aarch64 : public Output_data_got<size, big_endian>
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Valtype;
  Output_data_got_aarch64(Symbol_table* symtab, Layout* layout)
    : Output_data_got<size, big_endian>(), layout_(layout)
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
  // A pointer to the Layout class, so that we can find the .dynamic
  // section when we write out the GOT section.
  Layout* layout_;
};

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

  // Return the number of entries in the PLT.
  unsigned int
  plt_entry_count() const;

  //Return the offset of the first non-reserved PLT entry.
  unsigned int
  first_plt_entry_offset() const;

  // Return the size of each PLT entry.
  unsigned int
  plt_entry_size() const;

 private:
  // The class which scans relocations.
  class Scan
  {
  public:
    Scan()
      : issued_non_pic_error_(false)
    { }

    static inline int
    get_reference_flags(unsigned int r_type);

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
      this->got_plt_->set_current_data_size
          (AARCH64_GOTPLT_RESERVE_COUNT * (size / 8));

      if (!is_got_plt_relro)
        {
          // Those bytes can go into the relro segment.
          layout->increase_relro
              (AARCH64_GOTPLT_RESERVE_COUNT * (size / 8));
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
  //TODO
}

// Set the final size.
template<int size, bool big_endian>
void
Output_data_plt_aarch64<size, big_endian>::set_final_data_size()
{
  this->set_data_size(this->first_plt_entry_offset()
                      + this->count * this->get_plt_entry_size());
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
    Address /* got_address */,
    Address /* plt_address */)
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
  // TODO
}

// Subsequent entries in the PLT for an executable.

template<int size, bool big_endian>
void
Output_data_plt_aarch64_standard<size, big_endian>::do_fill_plt_entry(
    unsigned char* pov,
    Address /* got_address*/,
    Address /* plt_address */,
    unsigned int /* got_offset */,
    unsigned int /* plt_offset */)
{
  memcpy(pov, this->plt_entry, this->plt_entry_size);
  //TODO
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
  // The base address of the .got section.
  typename elfcpp::Elf_types<size>::Elf_Addr got_base = this->got_->address();
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

      // Set the entry in the GOT.
      elfcpp::Swap<size, big_endian>::writeval(got_pov,
          plt_address + plt_offset);
    }

  gold_assert(static_cast<section_size_type>(pov - oview) == oview_size);
  gold_assert(static_cast<section_size_type>(got_pov - got_view) == got_size);

  of->write_output_view(offset, oview_size, oview);
  of->write_output_view(got_file_offset, got_size, got_view);
}

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

// Get the Reference_flags for a particular relocation.
template<int size, bool big_endian>
int
Target_aarch64<size, big_endian>::Scan::get_reference_flags(unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_AARCH64_NONE:
      // No symbol reference.
      return 0;
    //TODO
    default:
      // Not expected. We will give an error later.
    return 0;
    }
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
    Sized_relobj_file<size, big_endian>* /* object */,
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
    case elfcpp::R_AARCH64_NONE:
      break;

      //TODO
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
    Symbol_table* /* symtab */,
    Layout* /* layout */,
    Target_aarch64<size, big_endian>* /* target */,
    Sized_relobj_file<size, big_endian>* /* object */,
    unsigned int /* data_shndx */,
    Output_section* /* output_section */,
    const elfcpp::Rela<size, big_endian>& /* reloc */,
    unsigned int /* r_type */,
    Symbol* /* gsym */)
{
  //TODO
  return;
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

  gold::gc_process_relocs<size, big_endian,
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

  gold::scan_relocs<size, big_endian, Target_aarch64<size, big_endian>,
                    elfcpp::SHT_RELA,
                    typename Target_aarch64<size, big_endian>::Scan>(
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
    Layout* /* layout */,
    const Input_objects*,
    Symbol_table* /* symtab */)
{
  //TODO
  return;
}

// Perform a relocation.

template<int size, bool big_endian>
inline bool
Target_aarch64<size, big_endian>::Relocate::relocate(
    const Relocate_info<size, big_endian>* /* relinfo */,
    Target_aarch64<size, big_endian>* /* target */,
    Output_section* ,
    size_t /* relnum */,
    const elfcpp::Rela<size, big_endian>& /* rela */,
    unsigned int /* r_type */,
    const Sized_symbol<size>* /* gsym */,
    const Symbol_value<size>* /* psymval */,
    unsigned char* /* view */,
    typename elfcpp::Elf_types<size>::Elf_Addr /* address */,
    section_size_type /* view_size */)
{
  //TODO
  return true;
}

// Relocate section data.

template<int size, bool big_endian>
void
Target_aarch64<size, big_endian>::relocate_section(
    const Relocate_info<size, big_endian>* /* relinfo */,
    unsigned int sh_type,
    const unsigned char* /* prelocs */,
    size_t /* reloc_count */,
    Output_section* /* output_section */,
    bool /*needs_special_offset_handling */,
    unsigned char* /* view */,
    typename elfcpp::Elf_types<size>::Elf_Addr /* address */,
    section_size_type /* view_size */,
    const Reloc_symbol_changes* /* reloc_symbol_changes */)
{
  //TODO
  gold_assert(sh_type == elfcpp::SHT_RELA);
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
  Target_selector_aarch64()
    : Target_selector(elfcpp::EM_AARCH64, size, big_endian,
                      (size == 64
                       ? (big_endian ? "elf64-bigaarch64"
                                     : "elf64-littleaarch64")
                       : (big_endian ? "elf32-bigaarch64"
                                     : "elf32-littleaarch64")),
                      (size == 64
                       ? (big_endian ? "aarch64_elf64_be_vec"
                                     : "aarch64_elf64_le_vec")
                       : (big_endian ? "aarch64_elf32_be_vec"
                                     : "aarch64_elf32_le_vec")))
  { }

  virtual Target*
  do_instantiate_target()
  { return new Target_aarch64<size, big_endian>(); }
};

Target_selector_aarch64<32, true> target_selector_aarch64elf32b;
Target_selector_aarch64<32, false> target_selector_aarch64elf32;
Target_selector_aarch64<64, true> target_selector_aarch64elfb;
Target_selector_aarch64<64, false> target_selector_aarch64elf;


} // End anonymous namespace.
