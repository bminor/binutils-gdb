// powerpc.cc -- powerpc target support for gold.

// Copyright 2008, 2009, 2010, 2011, 2012 Free Software Foundation, Inc.
// Written by David S. Miller <davem@davemloft.net>
//        and David Edelsohn <edelsohn@gnu.org>

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

#include "elfcpp.h"
#include "parameters.h"
#include "reloc.h"
#include "powerpc.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "copy-relocs.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"
#include "tls.h"
#include "errors.h"
#include "gc.h"

namespace
{

using namespace gold;

template<int size, bool big_endian>
class Output_data_plt_powerpc;

template<int size, bool big_endian>
class Output_data_got_powerpc;

template<int size, bool big_endian>
class Output_data_glink;

template<int size, bool big_endian>
class Powerpc_relobj : public Sized_relobj_file<size, big_endian>
{
public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef typename elfcpp::Elf_types<size>::Elf_Off Offset;

  Powerpc_relobj(const std::string& name, Input_file* input_file, off_t offset,
		 const typename elfcpp::Ehdr<size, big_endian>& ehdr)
    : Sized_relobj_file<size, big_endian>(name, input_file, offset, ehdr),
      special_(0), opd_ent_shndx_(), opd_ent_off_()
  { }

  ~Powerpc_relobj()
  { }

  // The .got2 section shndx.
  unsigned int
  got2_shndx() const
  {
    if (size == 32)
      return this->special_;
    else
      return 0;
  }

  // The .opd section shndx.
  unsigned int
  opd_shndx() const
  {
    if (size == 32)
      return 0;
    else
      return this->special_;
  }

  // Init OPD entry arrays.
  void
  init_opd(size_t opd_size)
  {
    size_t count = this->opd_ent_ndx(opd_size);
    this->opd_ent_shndx_.resize(count);
    this->opd_ent_off_.reserve(count);
  }

  // Return section and offset of function entry for .opd + R_OFF.
  void
  get_opd_ent(Address r_off, unsigned int* shndx, Address* value)
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_shndx_.size());
    gold_assert(this->opd_ent_shndx_[ndx] != 0);
    *shndx = this->opd_ent_shndx_[ndx];
    *value = this->opd_ent_off_[ndx];
  }

  // Set section and offset of function entry for .opd + R_OFF.
  void
  set_opd_ent(Address r_off, unsigned int shndx, Address value)
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_shndx_.size());
    this->opd_ent_shndx_[ndx] = shndx;
    this->opd_ent_off_[ndx] = value;
  }

  // Examine .rela.opd to build info about function entry points.
  void
  scan_opd_relocs(size_t reloc_count,
		  const unsigned char* prelocs,
		  const unsigned char* plocal_syms);

  void
  do_read_relocs(Read_relocs_data*);

  bool
  do_find_special_sections(Read_symbols_data* sd);

  // Return offset in output GOT section that this object will use
  // as a TOC pointer.  Won't be just a constant with multi-toc support.
  Address
  toc_base_offset() const
  { return 0x8000; }

private:
  // Return index into opd_ent_shndx or opd_ent_off array for .opd entry
  // at OFF.  .opd entries are 24 bytes long, but they can be spaced
  // 16 bytes apart when the language doesn't use the last 8-byte
  // word, the environment pointer.  Thus dividing the entry section
  // offset by 16 will give an index into opd_ent_shndx_ and
  // opd_ent_off_ that works for either layout of .opd.  (It leaves
  // some elements of the vectors unused when .opd entries are spaced
  // 24 bytes apart, but we don't know the spacing until relocations
  // are processed, and in any case it is possible for an object to
  // have some entries spaced 16 bytes apart and others 24 bytes apart.)
  size_t
  opd_ent_ndx(size_t off) const
  { return off >> 4;}

  // For 32-bit the .got2 section shdnx, for 64-bit the .opd section shndx.
  unsigned int special_;
  // The first 8-byte word of an OPD entry gives the address of the
  // entry point of the function.  Relocatable object files have a
  // relocation on this word.  The following two vectors record the
  // section and offset specified by these relocations.
  std::vector<unsigned int> opd_ent_shndx_;
  std::vector<Offset> opd_ent_off_;
};

template<int size, bool big_endian>
class Target_powerpc : public Sized_target<size, big_endian>
{
 public:
  typedef
    Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Reloc_section;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef typename elfcpp::Elf_types<size>::Elf_Swxword Signed_address;
  static const Address invalid_address = static_cast<Address>(0) - 1;
  // Offset of tp and dtp pointers from start of TLS block.
  static const Address tp_offset = 0x7000;
  static const Address dtp_offset = 0x8000;

  Target_powerpc()
    : Sized_target<size, big_endian>(&powerpc_info),
      got_(NULL), plt_(NULL), glink_(NULL), rela_dyn_(NULL),
      copy_relocs_(elfcpp::R_POWERPC_COPY),
      dynbss_(NULL), tlsld_got_offset_(-1U)
  {
  }

  // Process the relocations to determine unreferenced sections for
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

  // Map input .toc section to output .got section.
  const char*
  do_output_section_name(const Relobj*, const char* name, size_t* plen) const
  {
    if (size == 64 && strcmp(name, ".toc") == 0)
      {
	*plen = 4;
	return ".got";
      }
    return NULL;
  }

  // Finalize the sections.
  void
  do_finalize_sections(Layout*, const Input_objects*, Symbol_table*);

  // Return the value to use for a dynamic which requires special
  // treatment.
  uint64_t
  do_dynsym_value(const Symbol*) const;

  // Relocate a section.
  void
  relocate_section(const Relocate_info<size, big_endian>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   Output_section* output_section,
		   bool needs_special_offset_handling,
		   unsigned char* view,
		   Address view_address,
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
  relocate_for_relocatable(const Relocate_info<size, big_endian>*,
			   unsigned int sh_type,
			   const unsigned char* prelocs,
			   size_t reloc_count,
			   Output_section* output_section,
			   off_t offset_in_output_section,
			   const Relocatable_relocs*,
			   unsigned char*,
			   Address view_address,
			   section_size_type,
			   unsigned char* reloc_view,
			   section_size_type reloc_view_size);

  // Return whether SYM is defined by the ABI.
  bool
  do_is_defined_by_abi(const Symbol* sym) const
  {
    return strcmp(sym->name(), "__tls_get_addr") == 0;
  }

  // Return the size of the GOT section.
  section_size_type
  got_size() const
  {
    gold_assert(this->got_ != NULL);
    return this->got_->data_size();
  }

  // Get the PLT section.
  const Output_data_plt_powerpc<size, big_endian>*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Get the .glink section.
  const Output_data_glink<size, big_endian>*
  glink_section() const
  {
    gold_assert(this->glink_ != NULL);
    return this->glink_;
  }

  // Get the GOT section.
  const Output_data_got_powerpc<size, big_endian>*
  got_section() const
  {
    gold_assert(this->got_ != NULL);
    return this->got_;
  }

  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		     const elfcpp::Ehdr<size, big_endian>&);

  // Return the number of entries in the GOT.
  unsigned int
  got_entry_count() const
  {
    if (this->got_ == NULL)
      return 0;
    return this->got_size() / (size / 8);
  }

  // Return the number of entries in the PLT.
  unsigned int
  plt_entry_count() const;

  // Return the offset of the first non-reserved PLT entry.
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
    local(Symbol_table* symtab, Layout* layout, Target_powerpc* target,
	  Sized_relobj_file<size, big_endian>* object,
	  unsigned int data_shndx,
	  Output_section* output_section,
	  const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	  const elfcpp::Sym<size, big_endian>& lsym);

    inline void
    global(Symbol_table* symtab, Layout* layout, Target_powerpc* target,
	   Sized_relobj_file<size, big_endian>* object,
	   unsigned int data_shndx,
	   Output_section* output_section,
	   const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	   Symbol* gsym);

    inline bool
    local_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					Target_powerpc* ,
					Sized_relobj_file<size, big_endian>* ,
					unsigned int ,
					Output_section* ,
					const elfcpp::Rela<size, big_endian>& ,
					unsigned int ,
					const elfcpp::Sym<size, big_endian>&)
    { return false; }

    inline bool
    global_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					 Target_powerpc* ,
					 Sized_relobj_file<size, big_endian>* ,
					 unsigned int ,
					 Output_section* ,
					 const elfcpp::Rela<size,
							    big_endian>& ,
					 unsigned int , Symbol*)
    { return false; }

  private:
    static void
    unsupported_reloc_local(Sized_relobj_file<size, big_endian>*,
			    unsigned int r_type);

    static void
    unsupported_reloc_global(Sized_relobj_file<size, big_endian>*,
			     unsigned int r_type, Symbol*);

    static void
    generate_tls_call(Symbol_table* symtab, Layout* layout,
		      Target_powerpc* target);

    void
    check_non_pic(Relobj*, unsigned int r_type);

    // Whether we have issued an error about a non-PIC compilation.
    bool issued_non_pic_error_;
  };

  // The class which implements relocation.
  class Relocate
  {
   public:
    // Use 'at' branch hints when true, 'y' when false.
    // FIXME maybe: set this with an option.
    static const bool is_isa_v2 = true;

    enum skip_tls
    {
      CALL_NOT_EXPECTED = 0,
      CALL_EXPECTED = 1,
      CALL_SKIP = 2
    };

    Relocate()
      : call_tls_get_addr_(CALL_NOT_EXPECTED)
    { }

    ~Relocate()
    {
      if (this->call_tls_get_addr_ != CALL_NOT_EXPECTED)
	{
	  // FIXME: This needs to specify the location somehow.
	  gold_error(_("missing expected __tls_get_addr call"));
	}
    }

    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool
    relocate(const Relocate_info<size, big_endian>*, Target_powerpc*,
	     Output_section*, size_t relnum,
	     const elfcpp::Rela<size, big_endian>&,
	     unsigned int r_type, const Sized_symbol<size>*,
	     const Symbol_value<size>*,
	     unsigned char*,
	     typename elfcpp::Elf_types<size>::Elf_Addr,
	     section_size_type);

    // This is set if we should skip the next reloc, which should be a
    // call to __tls_get_addr.
    enum skip_tls call_tls_get_addr_;
  };

  // A class which returns the size required for a relocation type,
  // used while scanning relocs during a relocatable link.
  class Relocatable_size_for_reloc
  {
   public:
    unsigned int
    get_size_for_reloc(unsigned int, Relobj*)
    {
      gold_unreachable();
      return 0;
    }
  };

  // Optimize the TLS relocation type based on what we know about the
  // symbol.  IS_FINAL is true if the final address of this symbol is
  // known at link time.

  tls::Tls_optimization
  optimize_tls_gd(bool is_final)
  {
    // If we are generating a shared library, then we can't do anything
    // in the linker.
    if (parameters->options().shared())
      return tls::TLSOPT_NONE;

    if (!is_final)
      return tls::TLSOPT_TO_IE;
    return tls::TLSOPT_TO_LE;
  }

  tls::Tls_optimization
  optimize_tls_ld()
  {
    if (parameters->options().shared())
      return tls::TLSOPT_NONE;

    return tls::TLSOPT_TO_LE;
  }

  tls::Tls_optimization
  optimize_tls_ie(bool is_final)
  {
    if (!is_final || parameters->options().shared())
      return tls::TLSOPT_NONE;

    return tls::TLSOPT_TO_LE;
  }

  // Get the GOT section, creating it if necessary.
  Output_data_got_powerpc<size, big_endian>*
  got_section(Symbol_table*, Layout*);

  // Create glink.
  void
  make_glink_section(Layout*);

  // Create the PLT section.
  void
  make_plt_section(Layout*);

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(Layout*, Symbol*,
		 const elfcpp::Rela<size, big_endian>&,
		 const Sized_relobj<size, big_endian>* object);

  // Create a GOT entry for local dynamic __tls_get_addr.
  unsigned int
  tlsld_got_offset(Symbol_table* symtab, Layout* layout,
		   Sized_relobj_file<size, big_endian>* object);

  unsigned int
  tlsld_got_offset() const
  {
    return this->tlsld_got_offset_;
  }

  // Get the dynamic reloc section, creating it if necessary.
  Reloc_section*
  rela_dyn_section(Layout*);

  // Copy a relocation against a global symbol.
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
  static Target::Target_info powerpc_info;

  // The types of GOT entries needed for this platform.
  // These values are exposed to the ABI in an incremental link.
  // Do not renumber existing values without changing the version
  // number of the .gnu_incremental_inputs section.
  enum Got_type
  {
    GOT_TYPE_STANDARD,
    GOT_TYPE_TLSGD,	// double entry for @got@tlsgd
    GOT_TYPE_DTPREL,	// entry for @got@dtprel
    GOT_TYPE_TPREL	// entry for @got@tprel
  };

  // The GOT output section.
  Output_data_got_powerpc<size, big_endian>* got_;
  // The PLT output section.
  Output_data_plt_powerpc<size, big_endian>* plt_;
  // The .glink output section.
  Output_data_glink<size, big_endian>* glink_;
  // The dynamic reloc output section.
  Reloc_section* rela_dyn_;
  // Relocs saved to avoid a COPY reloc.
  Copy_relocs<elfcpp::SHT_RELA, size, big_endian> copy_relocs_;
  // Space for variables copied with a COPY reloc.
  Output_data_space* dynbss_;
  // Offset of the GOT entry for local dynamic __tls_get_addr calls.
  unsigned int tlsld_got_offset_;
};

template<>
Target::Target_info Target_powerpc<32, true>::powerpc_info =
{
  32,			// size
  true,			// is_big_endian
  elfcpp::EM_PPC,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL			// attributes_vendor
};

template<>
Target::Target_info Target_powerpc<32, false>::powerpc_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_PPC,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL			// attributes_vendor
};

template<>
Target::Target_info Target_powerpc<64, true>::powerpc_info =
{
  64,			// size
  true,			// is_big_endian
  elfcpp::EM_PPC64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL			// attributes_vendor
};

template<>
Target::Target_info Target_powerpc<64, false>::powerpc_info =
{
  64,			// size
  false,		// is_big_endian
  elfcpp::EM_PPC64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL			// attributes_vendor
};

inline bool
is_branch_reloc(unsigned int r_type)
{
  return (r_type == elfcpp::R_POWERPC_REL24
	  || r_type == elfcpp::R_PPC_PLTREL24
	  || r_type == elfcpp::R_PPC_LOCAL24PC
	  || r_type == elfcpp::R_POWERPC_REL14
	  || r_type == elfcpp::R_POWERPC_REL14_BRTAKEN
	  || r_type == elfcpp::R_POWERPC_REL14_BRNTAKEN
	  || r_type == elfcpp::R_POWERPC_ADDR24
	  || r_type == elfcpp::R_POWERPC_ADDR14
	  || r_type == elfcpp::R_POWERPC_ADDR14_BRTAKEN
	  || r_type == elfcpp::R_POWERPC_ADDR14_BRNTAKEN);
}

// If INSN is an opcode that may be used with an @tls operand, return
// the transformed insn for TLS optimisation, otherwise return 0.  If
// REG is non-zero only match an insn with RB or RA equal to REG.
uint32_t
at_tls_transform(uint32_t insn, unsigned int reg)
{
  if ((insn & (0x3f << 26)) != 31 << 26)
    return 0;

  unsigned int rtra;
  if (reg == 0 || ((insn >> 11) & 0x1f) == reg)
    rtra = insn & ((1 << 26) - (1 << 16));
  else if (((insn >> 16) & 0x1f) == reg)
    rtra = (insn & (0x1f << 21)) | ((insn & (0x1f << 11)) << 5);
  else
    return 0;

  if ((insn & (0x3ff << 1)) == 266 << 1)
    // add -> addi
    insn = 14 << 26;
  else if ((insn & (0x1f << 1)) == 23 << 1
	   && ((insn & (0x1f << 6)) < 14 << 6
	       || ((insn & (0x1f << 6)) >= 16 << 6
		   && (insn & (0x1f << 6)) < 24 << 6)))
    // load and store indexed -> dform
    insn = (32 | ((insn >> 6) & 0x1f)) << 26;
  else if ((insn & (((0x1a << 5) | 0x1f) << 1)) == 21 << 1)
    // ldx, ldux, stdx, stdux -> ld, ldu, std, stdu
    insn = ((58 | ((insn >> 6) & 4)) << 26) | ((insn >> 6) & 1);
  else if ((insn & (((0x1f << 5) | 0x1f) << 1)) == 341 << 1)
    // lwax -> lwa
    insn = (58 << 26) | 2;
  else
    return 0;
  insn |= rtra;
  return insn;
}

// Modified version of symtab.h class Symbol member
// Given a direct absolute or pc-relative static relocation against
// the global symbol, this function returns whether a dynamic relocation
// is needed.

template<int size>
bool
needs_dynamic_reloc(const Symbol* gsym, int flags)
{
  // No dynamic relocations in a static link!
  if (parameters->doing_static_link())
    return false;

  // A reference to an undefined symbol from an executable should be
  // statically resolved to 0, and does not need a dynamic relocation.
  // This matches gnu ld behavior.
  if (gsym->is_undefined() && !parameters->options().shared())
    return false;

  // A reference to an absolute symbol does not need a dynamic relocation.
  if (gsym->is_absolute())
    return false;

  // An absolute reference within a position-independent output file
  // will need a dynamic relocation.
  if ((flags & Symbol::ABSOLUTE_REF)
      && parameters->options().output_is_position_independent())
    return true;

  // A function call that can branch to a local PLT entry does not need
  // a dynamic relocation.
  if ((flags & Symbol::FUNCTION_CALL) && gsym->has_plt_offset())
    return false;

  // A reference to any PLT entry in a non-position-independent executable
  // does not need a dynamic relocation.
  // Except due to having function descriptors on powerpc64 we don't define
  // functions to their plt code in an executable, so this doesn't apply.
  if (size == 32
      && !parameters->options().output_is_position_independent()
      && gsym->has_plt_offset())
    return false;

  // A reference to a symbol defined in a dynamic object or to a
  // symbol that is preemptible will need a dynamic relocation.
  if (gsym->is_from_dynobj()
      || gsym->is_undefined()
      || gsym->is_preemptible())
    return true;

  // For all other cases, return FALSE.
  return false;
}

// Modified version of symtab.h class Symbol member
// Whether we should use the PLT offset associated with a symbol for
// a relocation.  FLAGS is a set of Reference_flags.

template<int size>
bool
use_plt_offset(const Symbol* gsym, int flags)
{
  // If the symbol doesn't have a PLT offset, then naturally we
  // don't want to use it.
  if (!gsym->has_plt_offset())
    return false;

  // For a STT_GNU_IFUNC symbol we always have to use the PLT entry.
  if (gsym->type() == elfcpp::STT_GNU_IFUNC)
    return true;

  // If we are going to generate a dynamic relocation, then we will
  // wind up using that, so no need to use the PLT entry.
  if (needs_dynamic_reloc<size>(gsym, flags))
    return false;

  // If the symbol is from a dynamic object, we need to use the PLT
  // entry.
  if (gsym->is_from_dynobj())
    return true;

  // If we are generating a shared object, and gsym symbol is
  // undefined or preemptible, we need to use the PLT entry.
  if (parameters->options().shared()
      && (gsym->is_undefined() || gsym->is_preemptible()))
    return true;

  // If gsym is a call to a weak undefined symbol, we need to use
  // the PLT entry; the symbol may be defined by a library loaded
  // at runtime.
  if ((flags & Symbol::FUNCTION_CALL) && gsym->is_weak_undefined())
    return true;

  // Otherwise we can use the regular definition.
  return false;
}

template<int size, bool big_endian>
class Powerpc_relocate_functions
{
public:
  enum overflow_check
  {
    check_none,
    check_signed,
    check_bitfield
  };

  enum overflow_status
  {
    status_ok,
    status_overflow
  };

private:
  typedef Powerpc_relocate_functions<size, big_endian> This;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  template<int valsize>
  static inline bool
  has_overflow_signed(Address value)
  {
    // limit = 1 << (valsize - 1) without shift count exceeding size of type
    Address limit = static_cast<Address>(1) << ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) - 2 * ((valsize - 1) >> 1));
    return value + limit > (limit << 1) - 1;
  }

  template<int valsize>
  static inline bool
  has_overflow_bitfield(Address value)
  {
    Address limit = static_cast<Address>(1) << ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) - 2 * ((valsize - 1) >> 1));
    return value > (limit << 1) - 1 && value + limit > (limit << 1) - 1;
  }

  template<int valsize>
  static inline enum overflow_status
  overflowed(Address value, enum overflow_check overflow)
  {
    if (overflow == check_signed)
      {
	if (has_overflow_signed<valsize>(value))
	  return status_overflow;
      }
    else if (overflow == check_bitfield)
      {
	if (has_overflow_bitfield<valsize>(value))
	  return status_overflow;
      }
    return status_ok;
  }

  // Do a simple RELA relocation
  template<int valsize>
  static inline enum overflow_status
  rela(unsigned char* view, Address value, enum overflow_check overflow)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    elfcpp::Swap<valsize, big_endian>::writeval(wv, value);
    return overflowed<valsize>(value, overflow);
  }

  template<int valsize>
  static inline enum overflow_status
  rela(unsigned char* view,
       unsigned int right_shift,
       typename elfcpp::Valtype_base<valsize>::Valtype dst_mask,
       Address value,
       enum overflow_check overflow)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = value >> right_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;
    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
    return overflowed<valsize>(value >> right_shift, overflow);
  }

  // Do a simple RELA relocation, unaligned.
  template<int valsize>
  static inline enum overflow_status
  rela_ua(unsigned char* view, Address value, enum overflow_check overflow)
  {
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view, value);
    return overflowed<valsize>(value, overflow);
  }

  template<int valsize>
  static inline enum overflow_status
  rela_ua(unsigned char* view,
	  unsigned int right_shift,
	  typename elfcpp::Valtype_base<valsize>::Valtype dst_mask,
	  Address value,
	  enum overflow_check overflow)
  {
    typedef typename elfcpp::Swap_unaligned<valsize, big_endian>::Valtype
      Valtype;
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(view);
    Valtype reloc = value >> right_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view, val | reloc);
    return overflowed<valsize>(value >> right_shift, overflow);
  }

public:
  // R_PPC64_ADDR64: (Symbol + Addend)
  static inline void
  addr64(unsigned char* view, Address value)
  { This::template rela<64>(view, value, check_none); }

  // R_PPC64_UADDR64: (Symbol + Addend) unaligned
  static inline void
  addr64_u(unsigned char* view, Address value)
  { This::template rela_ua<64>(view, value, check_none); }

  // R_POWERPC_ADDR32: (Symbol + Addend)
  static inline enum overflow_status
  addr32(unsigned char* view, Address value, enum overflow_check overflow)
  { return This::template rela<32>(view, value, overflow); }

  // R_POWERPC_UADDR32: (Symbol + Addend) unaligned
  static inline enum overflow_status
  addr32_u(unsigned char* view, Address value, enum overflow_check overflow)
  { return This::template rela_ua<32>(view, value, overflow); }

  // R_POWERPC_ADDR24: (Symbol + Addend) & 0x3fffffc
  static inline enum overflow_status
  addr24(unsigned char* view, Address value, enum overflow_check overflow)
  {
    enum overflow_status stat
      = This::template rela<32>(view, 0, 0x03fffffc, value, overflow);
    if (overflow != check_none && (value & 3) != 0)
      stat = status_overflow;
    return stat;
  }

  // R_POWERPC_ADDR16: (Symbol + Addend) & 0xffff
  static inline enum overflow_status
  addr16(unsigned char* view, Address value, enum overflow_check overflow)
  { return This::template rela<16>(view, value, overflow); }

  // R_POWERPC_ADDR16: (Symbol + Addend) & 0xffff, unaligned
  static inline enum overflow_status
  addr16_u(unsigned char* view, Address value, enum overflow_check overflow)
  { return This::template rela_ua<16>(view, value, overflow); }

  // R_POWERPC_ADDR16_DS: (Symbol + Addend) & 0xfffc
  static inline enum overflow_status
  addr16_ds(unsigned char* view, Address value, enum overflow_check overflow)
  {
    enum overflow_status stat
      = This::template rela<16>(view, 0, 0xfffc, value, overflow);
    if (overflow != check_none && (value & 3) != 0)
      stat = status_overflow;
    return stat;
  }

  // R_POWERPC_ADDR16_HI: ((Symbol + Addend) >> 16) & 0xffff
  static inline void
  addr16_hi(unsigned char* view, Address value)
  { This::template rela<16>(view, 16, 0xffff, value, check_none); }

  // R_POWERPC_ADDR16_HA: ((Symbol + Addend + 0x8000) >> 16) & 0xffff
  static inline void
  addr16_ha(unsigned char* view, Address value)
  { This::addr16_hi(view, value + 0x8000); }

  // R_POWERPC_ADDR16_HIGHER: ((Symbol + Addend) >> 32) & 0xffff
  static inline void
  addr16_hi2(unsigned char* view, Address value)
  { This::template rela<16>(view, 32, 0xffff, value, check_none); }

  // R_POWERPC_ADDR16_HIGHERA: ((Symbol + Addend + 0x8000) >> 32) & 0xffff
  static inline void
  addr16_ha2(unsigned char* view, Address value)
  { This::addr16_hi2(view, value + 0x8000); }

  // R_POWERPC_ADDR16_HIGHEST: ((Symbol + Addend) >> 48) & 0xffff
  static inline void
  addr16_hi3(unsigned char* view, Address value)
  { This::template rela<16>(view, 48, 0xffff, value, check_none); }

  // R_POWERPC_ADDR16_HIGHESTA: ((Symbol + Addend + 0x8000) >> 48) & 0xffff
  static inline void
  addr16_ha3(unsigned char* view, Address value)
  { This::addr16_hi3(view, value + 0x8000); }

  // R_POWERPC_ADDR14: (Symbol + Addend) & 0xfffc
  static inline enum overflow_status
  addr14(unsigned char* view, Address value, enum overflow_check overflow)
  {
    enum overflow_status stat
      = This::template rela<32>(view, 0, 0xfffc, value, overflow);
    if (overflow != check_none && (value & 3) != 0)
      stat = status_overflow;
    return stat;
  }
};

// Stash away the index of .got2 or .opd in a relocatable object, if
// such a section exists.

template<int size, bool big_endian>
bool
Powerpc_relobj<size, big_endian>::do_find_special_sections(
    Read_symbols_data* sd)
{
  const unsigned char* const pshdrs = sd->section_headers->data();
  const unsigned char* namesu = sd->section_names->data();
  const char* names = reinterpret_cast<const char*>(namesu);
  section_size_type names_size = sd->section_names_size;
  const unsigned char* s;

  s = this->find_shdr(pshdrs, size == 32 ? ".got2" : ".opd",
		      names, names_size, NULL);
  if (s != NULL)
    {
      unsigned int ndx = (s - pshdrs) / elfcpp::Elf_sizes<size>::shdr_size;
      this->special_ = ndx;
    }
  return Sized_relobj_file<size, big_endian>::do_find_special_sections(sd);
}

// Examine .rela.opd to build info about function entry points.

template<int size, bool big_endian>
void
Powerpc_relobj<size, big_endian>::scan_opd_relocs(
    size_t reloc_count,
    const unsigned char* prelocs,
    const unsigned char* plocal_syms)
{
  if (size == 64)
    {
      typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc
	Reltype;
      const int reloc_size
	= Reloc_types<elfcpp::SHT_RELA, size, big_endian>::reloc_size;
      const int sym_size = elfcpp::Elf_sizes<size>::sym_size;

      for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
	{
	  Reltype reloc(prelocs);
	  typename elfcpp::Elf_types<size>::Elf_WXword r_info
	    = reloc.get_r_info();
	  unsigned int r_type = elfcpp::elf_r_type<size>(r_info);
	  if (r_type == elfcpp::R_PPC64_ADDR64)
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
	      typename elfcpp::Elf_types<size>::Elf_Addr value;
	      bool is_ordinary;
	      unsigned int shndx;
	      if (r_sym < this->local_symbol_count())
		{
		  typename elfcpp::Sym<size, big_endian>
		    lsym(plocal_syms + r_sym * sym_size);
		  shndx = lsym.get_st_shndx();
		  shndx = this->adjust_sym_shndx(r_sym, shndx, &is_ordinary);
		  value = lsym.get_st_value();
		}
	      else
		shndx = this->symbol_section_and_value(r_sym, &value,
						       &is_ordinary);
	      this->set_opd_ent(reloc.get_r_offset(), shndx,
				value + reloc.get_r_addend());
	    }
	}
    }
}

template<int size, bool big_endian>
void
Powerpc_relobj<size, big_endian>::do_read_relocs(Read_relocs_data* rd)
{
  Sized_relobj_file<size, big_endian>::do_read_relocs(rd);
  if (size == 64)
    {
      for (Read_relocs_data::Relocs_list::iterator p = rd->relocs.begin();
	   p != rd->relocs.end();
	   ++p)
	{
	  if (p->data_shndx == this->opd_shndx())
	    {
	      this->init_opd(this->section_size(this->opd_shndx()));
	      this->scan_opd_relocs(p->reloc_count, p->contents->data(),
				    rd->local_symbols->data());
	      break;
	    }
	}
    }
}

// Set up PowerPC target specific relobj.

template<int size, bool big_endian>
Object*
Target_powerpc<size, big_endian>::do_make_elf_object(
    const std::string& name,
    Input_file* input_file,
    off_t offset, const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  int et = ehdr.get_e_type();
  // ET_EXEC files are valid input for --just-symbols/-R,
  // and we treat them as relocatable objects.
  if (et == elfcpp::ET_REL
      || (et == elfcpp::ET_EXEC && input_file->just_symbols()))
    {
      Powerpc_relobj<size, big_endian>* obj =
	new Powerpc_relobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else if (et == elfcpp::ET_DYN)
    {
      Sized_dynobj<size, big_endian>* obj =
	new Sized_dynobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else
    {
      gold_error(_("%s: unsupported ELF file type %d"), name.c_str(), et);
      return NULL;
    }
}

template<int size, bool big_endian>
class Output_data_got_powerpc : public Output_data_got<size, big_endian>
{
public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Valtype;
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Rela_dyn;

  Output_data_got_powerpc(Symbol_table* symtab, Layout* layout)
    : Output_data_got<size, big_endian>(),
      symtab_(symtab), layout_(layout),
      header_ent_cnt_(size == 32 ? 3 : 1),
      header_index_(size == 32 ? 0x2000 : 0)
  {}

  class Got_entry;

  // Create a new GOT entry and return its offset.
  unsigned int
  add_got_entry(Got_entry got_entry)
  {
    this->reserve_ent();
    return Output_data_got<size, big_endian>::add_got_entry(got_entry);
  }

  // Create a pair of new GOT entries and return the offset of the first.
  unsigned int
  add_got_entry_pair(Got_entry got_entry_1, Got_entry got_entry_2)
  {
    this->reserve_ent(2);
    return Output_data_got<size, big_endian>::add_got_entry_pair(got_entry_1,
								 got_entry_2);
  }

  unsigned int
  add_constant_pair(Valtype c1, Valtype c2)
  {
    this->reserve_ent(2);
    unsigned int got_offset = this->add_constant(c1);
    this->add_constant(c2);
    return got_offset;
  }

  // Offset of _GLOBAL_OFFSET_TABLE_.
  unsigned int
  g_o_t() const
  {
    return this->got_offset(this->header_index_);
  }

  // Offset of base used to access the GOT/TOC.
  // The got/toc pointer reg will be set to this value.
  typename elfcpp::Elf_types<size>::Elf_Off
  got_base_offset(const Powerpc_relobj<size, big_endian>* object) const
  {
    if (size == 32)
      return this->g_o_t();
    else
      return (this->output_section()->address()
	      + object->toc_base_offset()
	      - this->address());
  }

  // Ensure our GOT has a header.
  void
  set_final_data_size()
  {
    if (this->header_ent_cnt_ != 0)
      this->make_header();
    Output_data_got<size, big_endian>::set_final_data_size();
  }

  // First word of GOT header needs some values that are not
  // handled by Output_data_got so poke them in here.
  // For 32-bit, address of .dynamic, for 64-bit, address of TOCbase.
  void
  do_write(Output_file* of)
  {
    this->replace_constant(this->header_index_,
			   (size == 32
			    ? this->layout_->dynamic_section()->address()
			    : this->output_section()->address() + 0x8000));

    Output_data_got<size, big_endian>::do_write(of);
  }

private:
  void
  reserve_ent(unsigned int cnt = 1)
  {
    if (this->header_ent_cnt_ == 0)
      return;
    if (this->num_entries() + cnt > this->header_index_)
      this->make_header();
  }

  void
  make_header()
  {
    this->header_ent_cnt_ = 0;
    this->header_index_ = this->num_entries();
    if (size == 32)
      {
	Output_data_got<size, big_endian>::add_constant(0);
	Output_data_got<size, big_endian>::add_constant(0);
	Output_data_got<size, big_endian>::add_constant(0);

	// Define _GLOBAL_OFFSET_TABLE_ at the header
	this->symtab_->define_in_output_data("_GLOBAL_OFFSET_TABLE_", NULL,
					     Symbol_table::PREDEFINED,
					     this, this->g_o_t(), 0,
					     elfcpp::STT_OBJECT,
					     elfcpp::STB_LOCAL,
					     elfcpp::STV_HIDDEN,
					     0, false, false);
      }
    else
      Output_data_got<size, big_endian>::add_constant(0);
  }

  // Stashed pointers.
  Symbol_table* symtab_;
  Layout* layout_;

  // GOT header size.
  unsigned int header_ent_cnt_;
  // GOT header index.
  unsigned int header_index_;
};

// Get the GOT section, creating it if necessary.

template<int size, bool big_endian>
Output_data_got_powerpc<size, big_endian>*
Target_powerpc<size, big_endian>::got_section(Symbol_table* symtab,
					      Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(symtab != NULL && layout != NULL);

      this->got_
	= new Output_data_got_powerpc<size, big_endian>(symtab, layout);

      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->got_, ORDER_DATA, false);
    }

  return this->got_;
}

// Get the dynamic reloc section, creating it if necessary.

template<int size, bool big_endian>
typename Target_powerpc<size, big_endian>::Reloc_section*
Target_powerpc<size, big_endian>::rela_dyn_section(Layout* layout)
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

template<int size, bool big_endian>
class Output_data_plt_powerpc : public Output_section_data_build
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true,
			    size, big_endian> Reloc_section;

  Output_data_plt_powerpc(Layout*, Target_powerpc<size, big_endian>*);

  // Add an entry to the PLT.
  void
  add_entry(Symbol*);

  // Return the .rela.plt section data.
  const Reloc_section*
  rel_plt() const
  {
    return this->rel_;
  }

  // Return the number of PLT entries.
  unsigned int
  entry_count() const
  {
    return ((this->current_data_size() - initial_plt_entry_size)
	    / plt_entry_size);
  }

  // Return the offset of the first non-reserved PLT entry.
  static unsigned int
  first_plt_entry_offset()
  { return initial_plt_entry_size; }

  // Return the size of a PLT entry.
  static unsigned int
  get_plt_entry_size()
  { return plt_entry_size; }

 protected:
  void
  do_adjust_output_section(Output_section* os)
  {
    os->set_entsize(0);
  }

  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** PLT")); }

 private:
  // The size of an entry in the PLT.
  static const int plt_entry_size = size == 32 ? 4 : 24;
  // The size of the first reserved entry.
  static const int initial_plt_entry_size = size == 32 ? 0 : 24;

  // Write out the PLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  // Allows access to .glink for do_write.
  Target_powerpc<size, big_endian>* targ_;
};

// Create the PLT section.

template<int size, bool big_endian>
Output_data_plt_powerpc<size, big_endian>::Output_data_plt_powerpc(
    Layout* layout,
    Target_powerpc<size, big_endian>* targ)
  : Output_section_data_build(size == 32 ? 4 : 8),
    targ_(targ)
{
  this->rel_ = new Reloc_section(false);
  layout->add_output_section_data(".rela.plt", elfcpp::SHT_RELA,
				  elfcpp::SHF_ALLOC, this->rel_,
				  ORDER_DYNAMIC_PLT_RELOCS, false);
}

// Add an entry to the PLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::add_entry(Symbol* gsym)
{
  if (!gsym->has_plt_offset())
    {
      off_t off = this->current_data_size();

      if (off == 0)
	off += initial_plt_entry_size;
      gsym->set_plt_offset(off);
      gsym->set_needs_dynsym_entry();
      this->rel_->add_global(gsym, elfcpp::R_POWERPC_JMP_SLOT, this, off, 0);
      off += plt_entry_size;
      this->set_current_data_size(off);
    }
}

static const uint32_t add_0_11_11	= 0x7c0b5a14;
static const uint32_t add_3_3_2		= 0x7c631214;
static const uint32_t add_3_3_13	= 0x7c636a14;
static const uint32_t add_11_0_11	= 0x7d605a14;
static const uint32_t add_12_2_11	= 0x7d825a14;
static const uint32_t addi_11_11	= 0x396b0000;
static const uint32_t addi_12_12	= 0x398c0000;
static const uint32_t addi_2_2		= 0x38420000;
static const uint32_t addi_3_2		= 0x38620000;
static const uint32_t addi_3_3		= 0x38630000;
static const uint32_t addis_0_2		= 0x3c020000;
static const uint32_t addis_0_13	= 0x3c0d0000;
static const uint32_t addis_11_11	= 0x3d6b0000;
static const uint32_t addis_11_30	= 0x3d7e0000;
static const uint32_t addis_12_12	= 0x3d8c0000;
static const uint32_t addis_12_2	= 0x3d820000;
static const uint32_t addis_3_2		= 0x3c620000;
static const uint32_t addis_3_13	= 0x3c6d0000;
static const uint32_t b			= 0x48000000;
static const uint32_t bcl_20_31		= 0x429f0005;
static const uint32_t bctr		= 0x4e800420;
static const uint32_t blrl		= 0x4e800021;
static const uint32_t cror_15_15_15	= 0x4def7b82;
static const uint32_t cror_31_31_31	= 0x4ffffb82;
static const uint32_t ld_11_12		= 0xe96c0000;
static const uint32_t ld_11_2		= 0xe9620000;
static const uint32_t ld_2_1		= 0xe8410000;
static const uint32_t ld_2_11		= 0xe84b0000;
static const uint32_t ld_2_12		= 0xe84c0000;
static const uint32_t ld_2_2		= 0xe8420000;
static const uint32_t li_0_0		= 0x38000000;
static const uint32_t lis_0_0		= 0x3c000000;
static const uint32_t lis_11		= 0x3d600000;
static const uint32_t lis_12		= 0x3d800000;
static const uint32_t lwz_0_12		= 0x800c0000;
static const uint32_t lwz_11_11		= 0x816b0000;
static const uint32_t lwz_11_30		= 0x817e0000;
static const uint32_t lwz_12_12		= 0x818c0000;
static const uint32_t lwzu_0_12		= 0x840c0000;
static const uint32_t mflr_0		= 0x7c0802a6;
static const uint32_t mflr_11		= 0x7d6802a6;
static const uint32_t mflr_12		= 0x7d8802a6;
static const uint32_t mtctr_0		= 0x7c0903a6;
static const uint32_t mtctr_11		= 0x7d6903a6;
static const uint32_t mtlr_0		= 0x7c0803a6;
static const uint32_t mtlr_12		= 0x7d8803a6;
static const uint32_t nop		= 0x60000000;
static const uint32_t ori_0_0_0		= 0x60000000;
static const uint32_t std_2_1		= 0xf8410000;
static const uint32_t sub_11_11_12	= 0x7d6c5850;

// Write out the PLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::do_write(Output_file* of)
{
  if (size == 32)
    {
      const off_t offset = this->offset();
      const section_size_type oview_size
	= convert_to_section_size_type(this->data_size());
      unsigned char* const oview = of->get_output_view(offset, oview_size);
      unsigned char* pov = oview;
      unsigned char* endpov = oview + oview_size;

      // The address the .glink branch table
      const Output_data_glink<size, big_endian>* glink
	= this->targ_->glink_section();
      elfcpp::Elf_types<32>::Elf_Addr branch_tab
	= glink->address() + glink->pltresolve();

      while (pov < endpov)
	{
	  elfcpp::Swap<32, big_endian>::writeval(pov, branch_tab);
	  pov += 4;
	  branch_tab += 4;
	}

      of->write_output_view(offset, oview_size, oview);
    }
}

// Create the PLT section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_plt_section(Layout* layout)
{
  if (this->plt_ == NULL)
    {
      if (this->glink_ == NULL)
	make_glink_section(layout);

      // Ensure that .rela.dyn always appears before .rela.plt  This is
      // necessary due to how, on PowerPC and some other targets, .rela.dyn
      // needs to include .rela.plt in it's range.
      this->rela_dyn_section(layout);

      this->plt_ = new Output_data_plt_powerpc<size, big_endian>(layout, this);
      layout->add_output_section_data(".plt",
				      (size == 32
				       ? elfcpp::SHT_PROGBITS
				       : elfcpp::SHT_NOBITS),
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->plt_,
				      (size == 32
				       ? ORDER_SMALL_DATA
				       : ORDER_SMALL_BSS),
				      false);
    }
}

// A class to handle .glink.

template<int size, bool big_endian>
class Output_data_glink : public Output_section_data
{
 public:
  static const int pltresolve_size = 16*4;

  Output_data_glink(Target_powerpc<size, big_endian>*);

  // Add an entry
  void
  add_entry(const Symbol*, const elfcpp::Rela<size, big_endian>&,
	    const Sized_relobj<size, big_endian>*);

  unsigned int
  find_entry(const Symbol*, const elfcpp::Rela<size, big_endian>&,
	     const Sized_relobj<size, big_endian>*) const;

  unsigned int
  glink_entry_size() const
  {
    if (size == 32)
      return 4 * 4;
    else
      // FIXME: We should be using multiple glink sections for
      // stubs to support > 33M applications.
      return 8 * 4;
  }

  off_t
  pltresolve() const
  {
    return this->pltresolve_;
  }

 protected:
  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** glink")); }

 private:
  void
  set_final_data_size();

  // Write out .glink
  void
  do_write(Output_file*);

  class Glink_sym_ent
  {
  public:
    Glink_sym_ent(const Symbol* sym,
		  const elfcpp::Rela<size, big_endian>& reloc,
		  const Sized_relobj<size, big_endian>* object)
      : sym_(sym), addend_(0), object_(0)
    {
      if (size != 32)
	this->addend_ = reloc.get_r_addend();
      else if (parameters->options().output_is_position_independent()
	       && (elfcpp::elf_r_type<size>(reloc.get_r_info())
		   == elfcpp::R_PPC_PLTREL24))
	{
	  this->addend_ = reloc.get_r_addend();
	  if (this->addend_ != 0)
	    this->object_ = object;
	}
    }

    bool operator==(const Glink_sym_ent& that) const
    {
      return (this->sym_ == that.sym_
	      && this->object_ == that.object_
	      && this->addend_ == that.addend_);
    }

    const Symbol* sym_;
    unsigned int addend_;
    const Sized_relobj<size, big_endian>* object_;
  };

  class Glink_sym_ent_hash
  {
  public:
    size_t operator()(const Glink_sym_ent& ent) const
    {
      return (reinterpret_cast<uintptr_t>(ent.sym_)
	      ^ reinterpret_cast<uintptr_t>(ent.object_)
	      ^ ent.addend_);
    }
  };

  // Map sym/object/addend to index.
  typedef Unordered_map<Glink_sym_ent, unsigned int,
			Glink_sym_ent_hash> Glink_entries;
  Glink_entries glink_entries_;

  // Offset of pltresolve stub (actually, branch table for 32-bit)
  off_t pltresolve_;

  // Allows access to .got and .plt for do_write.
  Target_powerpc<size, big_endian>* targ_;
};

// Create the glink section.

template<int size, bool big_endian>
Output_data_glink<size, big_endian>::Output_data_glink(
    Target_powerpc<size, big_endian>* targ)
  : Output_section_data(16),
    pltresolve_(0), targ_(targ)
{
}

// Add an entry to glink, if we do not already have one for this
// sym/object/addend combo.

template<int size, bool big_endian>
void
Output_data_glink<size, big_endian>::add_entry(
    const Symbol* gsym,
    const elfcpp::Rela<size, big_endian>& reloc,
    const Sized_relobj<size, big_endian>* object)
{
  Glink_sym_ent ent(gsym, reloc, object);
  unsigned int indx = this->glink_entries_.size();
  this->glink_entries_.insert(std::make_pair(ent, indx));
}

template<int size, bool big_endian>
unsigned int
Output_data_glink<size, big_endian>::find_entry(
    const Symbol* gsym,
    const elfcpp::Rela<size, big_endian>& reloc,
    const Sized_relobj<size, big_endian>* object) const
{
  Glink_sym_ent ent(gsym, reloc, object);
  typename Glink_entries::const_iterator p = this->glink_entries_.find(ent);
  gold_assert(p != this->glink_entries_.end());
  return p->second;
}

template<int size, bool big_endian>
void
Output_data_glink<size, big_endian>::set_final_data_size()
{
  unsigned int count = this->glink_entries_.size();
  off_t total = count;

  if (count != 0)
    {
      if (size == 32)
	{
	  total *= 16;
	  this->pltresolve_ = total;

	  // space for branch table
	  total += 4 * (count - 1);

	  total += -total & 15;
	  total += this->pltresolve_size;
	}
      else
	{
	  total *= 32;
	  this->pltresolve_ = total;
	  total += this->pltresolve_size;

	  // space for branch table
	  total += 8 * count;
	  if (count > 0x8000)
	    total += 4 * (count - 0x8000);
	}
    }

  this->set_data_size(total);
}

static inline uint32_t
l(uint32_t a)
{
  return a & 0xffff;
}

static inline uint32_t
hi(uint32_t a)
{
  return l(a >> 16);
}

static inline uint32_t
ha(uint32_t a)
{
  return hi(a + 0x8000);
}

template<bool big_endian>
static inline void
write_insn(unsigned char* p, uint32_t v)
{
  elfcpp::Swap<32, big_endian>::writeval(p, v);
}

// Write out .glink.

template<int size, bool big_endian>
void
Output_data_glink<size, big_endian>::do_write(Output_file* of)
{
  const off_t off = this->offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size());
  unsigned char* const oview = of->get_output_view(off, oview_size);
  unsigned char* p;

  // The base address of the .plt section.
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  Address plt_base = this->targ_->plt_section()->address();

  const Output_data_got_powerpc<size, big_endian>* got
    = this->targ_->got_section();

  if (size == 64)
    {
      Address got_os_addr = got->output_section()->address();

      // Write out call stubs.
      typename Glink_entries::const_iterator g;
      for (g = this->glink_entries_.begin();
	   g != this->glink_entries_.end();
	   ++g)
	{
	  Address plt_addr = plt_base + g->first.sym_->plt_offset();
	  const Powerpc_relobj<size, big_endian>* ppcobj = static_cast
	    <const Powerpc_relobj<size, big_endian>*>(g->first.object_);
	  Address got_addr = got_os_addr + ppcobj->toc_base_offset();
	  Address pltoff = plt_addr - got_addr;

	  if (pltoff + 0x80008000 > 0xffffffff || (pltoff & 7) != 0)
	    gold_error(_("%s: linkage table error against `%s'"),
		       g->first.object_->name().c_str(),
		       g->first.sym_->demangled_name().c_str());

	  p = oview + g->second * this->glink_entry_size();
	  if (ha(pltoff) != 0)
	    {
	      write_insn<big_endian>(p, addis_12_2 + ha(pltoff)),	p += 4;
	      write_insn<big_endian>(p, std_2_1 + 40),			p += 4;
	      write_insn<big_endian>(p, ld_11_12 + l(pltoff)),		p += 4;
	      if (ha(pltoff + 16) != ha(pltoff))
		{
		  write_insn<big_endian>(p, addi_12_12 + l(pltoff)),	p += 4;
		  pltoff = 0;
		}
	      write_insn<big_endian>(p, mtctr_11),			p += 4;
	      write_insn<big_endian>(p, ld_2_12 + l(pltoff + 8)),	p += 4;
	      write_insn<big_endian>(p, ld_11_12 + l(pltoff + 16)),	p += 4;
	      write_insn<big_endian>(p, bctr),				p += 4;
	    }
	  else
	    {
	      write_insn<big_endian>(p, std_2_1 + 40),			p += 4;
	      write_insn<big_endian>(p, ld_11_2 + l(pltoff)),		p += 4;
	      if (ha(pltoff + 16) != ha(pltoff))
		{
		  write_insn<big_endian>(p, addi_2_2 + l(pltoff)),	p += 4;
		  pltoff = 0;
		}
	      write_insn<big_endian>(p, mtctr_11),			p += 4;
	      write_insn<big_endian>(p, ld_11_2 + l(pltoff + 16)),	p += 4;
	      write_insn<big_endian>(p, ld_2_2 + l(pltoff + 8)),	p += 4;
	      write_insn<big_endian>(p, bctr),				p += 4;
	    }
	}

      // Write pltresolve stub.
      p = oview + this->pltresolve_;
      Address after_bcl = this->address() + this->pltresolve_ + 16;
      Address pltoff = plt_base - after_bcl;

      elfcpp::Swap<64, big_endian>::writeval(p, pltoff),	p += 8;

      write_insn<big_endian>(p, mflr_12),			p += 4;
      write_insn<big_endian>(p, bcl_20_31),			p += 4;
      write_insn<big_endian>(p, mflr_11),			p += 4;
      write_insn<big_endian>(p, ld_2_11 + l(-16)),		p += 4;
      write_insn<big_endian>(p, mtlr_12),			p += 4;
      write_insn<big_endian>(p, add_12_2_11),			p += 4;
      write_insn<big_endian>(p, ld_11_12 + 0),			p += 4;
      write_insn<big_endian>(p, ld_2_12 + 8),			p += 4;
      write_insn<big_endian>(p, mtctr_11),			p += 4;
      write_insn<big_endian>(p, ld_11_12 + 16),			p += 4;
      write_insn<big_endian>(p, bctr),				p += 4;
      while (p < oview + this->pltresolve_ + this->pltresolve_size)
	write_insn<big_endian>(p, nop), p += 4;

      // Write lazy link call stubs.
      uint32_t indx = 0;
      while (p < oview + oview_size)
	{
	  if (indx < 0x8000)
	    {
	      write_insn<big_endian>(p, li_0_0 + indx),			p += 4;
	    }
	  else
	    {
	      write_insn<big_endian>(p, lis_0_0 + hi(indx)),		p += 4;
	      write_insn<big_endian>(p, ori_0_0_0 + l(indx)),		p += 4;
	    }
	  uint32_t branch_off = this->pltresolve_ + 8 - (p - oview);
	  write_insn<big_endian>(p, b + (branch_off & 0x3fffffc)),	p += 4;
	  indx++;
	}
    }
  else
    {
      // The address of _GLOBAL_OFFSET_TABLE_.
      Address g_o_t = got->address() + got->g_o_t();

      // Write out call stubs.
      typename Glink_entries::const_iterator g;
      for (g = this->glink_entries_.begin();
	   g != this->glink_entries_.end();
	   ++g)
	{
	  Address plt_addr = plt_base + g->first.sym_->plt_offset();
	  Address got_addr;
	  const Address invalid_address = static_cast<Address>(-1);

	  p = oview + g->second * this->glink_entry_size();
	  if (parameters->options().output_is_position_independent())
	    {
	      const Powerpc_relobj<size, big_endian>* object = static_cast
		<const Powerpc_relobj<size, big_endian>*>(g->first.object_);
	      if (object != NULL)
		{
		  unsigned int got2 = object->got2_shndx();
		  got_addr = g->first.object_->get_output_section_offset(got2);
		  gold_assert(got_addr != invalid_address);
		  got_addr += (g->first.object_->output_section(got2)->address()
			       + g->first.addend_);
		}
	      else
		got_addr = g_o_t;

	      Address pltoff = plt_addr - got_addr;
	      if (ha(pltoff) == 0)
		{
		  write_insn<big_endian>(p +  0, lwz_11_30 + l(pltoff));
		  write_insn<big_endian>(p +  4, mtctr_11);
		  write_insn<big_endian>(p +  8, bctr);
		}
	      else
		{
		  write_insn<big_endian>(p +  0, addis_11_30 + ha(pltoff));
		  write_insn<big_endian>(p +  4, lwz_11_11 + l(pltoff));
		  write_insn<big_endian>(p +  8, mtctr_11);
		  write_insn<big_endian>(p + 12, bctr);
		}
	    }
	  else
	    {
	      write_insn<big_endian>(p +  0, lis_11 + ha(plt_addr));
	      write_insn<big_endian>(p +  4, lwz_11_11 + l(plt_addr));
	      write_insn<big_endian>(p +  8, mtctr_11);
	      write_insn<big_endian>(p + 12, bctr);
	    }
	}

      // Write out pltresolve branch table.
      p = oview + this->pltresolve_;
      unsigned int the_end = oview_size - this->pltresolve_size;
      unsigned char* end_p = oview + the_end;
      while (p < end_p - 8 * 4)
	write_insn<big_endian>(p, b + end_p - p), p += 4;
      while (p < end_p)
	write_insn<big_endian>(p, nop), p += 4;

      // Write out pltresolve call stub.
      if (parameters->options().output_is_position_independent())
	{
	  Address res0_off = this->pltresolve_;
	  Address after_bcl_off = the_end + 12;
	  Address bcl_res0 = after_bcl_off - res0_off;

	  write_insn<big_endian>(p +  0, addis_11_11 + ha(bcl_res0));
	  write_insn<big_endian>(p +  4, mflr_0);
	  write_insn<big_endian>(p +  8, bcl_20_31);
	  write_insn<big_endian>(p + 12, addi_11_11 + l(bcl_res0));
	  write_insn<big_endian>(p + 16, mflr_12);
	  write_insn<big_endian>(p + 20, mtlr_0);
	  write_insn<big_endian>(p + 24, sub_11_11_12);

	  Address got_bcl = g_o_t + 4 - (after_bcl_off + this->address());

	  write_insn<big_endian>(p + 28, addis_12_12 + ha(got_bcl));
	  if (ha(got_bcl) == ha(got_bcl + 4))
	    {
	      write_insn<big_endian>(p + 32, lwz_0_12 + l(got_bcl));
	      write_insn<big_endian>(p + 36, lwz_12_12 + l(got_bcl + 4));
	    }
	  else
	    {
	      write_insn<big_endian>(p + 32, lwzu_0_12 + l(got_bcl));
	      write_insn<big_endian>(p + 36, lwz_12_12 + 4);
	    }
	  write_insn<big_endian>(p + 40, mtctr_0);
	  write_insn<big_endian>(p + 44, add_0_11_11);
	  write_insn<big_endian>(p + 48, add_11_0_11);
	  write_insn<big_endian>(p + 52, bctr);
	  write_insn<big_endian>(p + 56, nop);
	  write_insn<big_endian>(p + 60, nop);
	}
      else
	{
	  Address res0 = this->pltresolve_ + this->address();

	  write_insn<big_endian>(p + 0, lis_12 + ha(g_o_t + 4));
	  write_insn<big_endian>(p + 4, addis_11_11 + ha(-res0));
	  if (ha(g_o_t + 4) == ha(g_o_t + 8))
	    write_insn<big_endian>(p + 8, lwz_0_12 + l(g_o_t + 4));
	  else
	    write_insn<big_endian>(p + 8, lwzu_0_12 + l(g_o_t + 4));
	  write_insn<big_endian>(p + 12, addi_11_11 + l(-res0));
	  write_insn<big_endian>(p + 16, mtctr_0);
	  write_insn<big_endian>(p + 20, add_0_11_11);
	  if (ha(g_o_t + 4) == ha(g_o_t + 8))
	    write_insn<big_endian>(p + 24, lwz_12_12 + l(g_o_t + 8));
	  else
	    write_insn<big_endian>(p + 24, lwz_12_12 + 4);
	  write_insn<big_endian>(p + 28, add_11_0_11);
	  write_insn<big_endian>(p + 32, bctr);
	  write_insn<big_endian>(p + 36, nop);
	  write_insn<big_endian>(p + 40, nop);
	  write_insn<big_endian>(p + 44, nop);
	  write_insn<big_endian>(p + 48, nop);
	  write_insn<big_endian>(p + 52, nop);
	  write_insn<big_endian>(p + 56, nop);
	  write_insn<big_endian>(p + 60, nop);
	}
      p += 64;
    }

  of->write_output_view(off, oview_size, oview);
}

// Create the glink section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_glink_section(Layout* layout)
{
  if (this->glink_ == NULL)
    {
      this->glink_ = new Output_data_glink<size, big_endian>(this);
      layout->add_output_section_data(".text", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_EXECINSTR,
				      this->glink_, ORDER_TEXT, false);
    }
}

// Create a PLT entry for a global symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_plt_entry(
    Layout* layout,
    Symbol* gsym,
    const elfcpp::Rela<size, big_endian>& reloc,
    const Sized_relobj<size, big_endian>* object)
{
  if (this->plt_ == NULL)
    this->make_plt_section(layout);

  this->plt_->add_entry(gsym);

  this->glink_->add_entry(gsym, reloc, object);
}

// Return the number of entries in the PLT.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::plt_entry_count() const
{
  if (this->plt_ == NULL)
    return 0;
  return this->plt_->entry_count();
}

// Return the offset of the first non-reserved PLT entry.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::first_plt_entry_offset() const
{
  return Output_data_plt_powerpc<size, big_endian>::first_plt_entry_offset();
}

// Return the size of each PLT entry.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::plt_entry_size() const
{
  return Output_data_plt_powerpc<size, big_endian>::get_plt_entry_size();
}

// Create a GOT entry for local dynamic __tls_get_addr calls.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::tlsld_got_offset(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object)
{
  if (this->tlsld_got_offset_ == -1U)
    {
      gold_assert(symtab != NULL && layout != NULL && object != NULL);
      Reloc_section* rela_dyn = this->rela_dyn_section(layout);
      Output_data_got_powerpc<size, big_endian>* got
	= this->got_section(symtab, layout);
      unsigned int got_offset = got->add_constant_pair(0, 0);
      rela_dyn->add_local(object, 0, elfcpp::R_POWERPC_DTPMOD, got,
			  got_offset, 0);
      this->tlsld_got_offset_ = got_offset;
    }
  return this->tlsld_got_offset_;
}

// Get the Reference_flags for a particular relocation.

template<int size, bool big_endian>
int
Target_powerpc<size, big_endian>::Scan::get_reference_flags(unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC64_TOC:
      // No symbol reference.
      return 0;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
      return Symbol::ABSOLUTE_REF;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      return Symbol::FUNCTION_CALL | Symbol::ABSOLUTE_REF;

    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
      return Symbol::RELATIVE_REF;

    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      return Symbol::FUNCTION_CALL | Symbol::RELATIVE_REF;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // Absolute in GOT.
      return Symbol::ABSOLUTE_REF;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_TLS:
      return Symbol::TLS_REF;

    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_DTPMOD:
    default:
      // Not expected.  We will give an error later.
      return 0;
    }
}

// Report an unsupported relocation against a local symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Scan::unsupported_reloc_local(
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
Target_powerpc<size, big_endian>::Scan::check_non_pic(Relobj* object,
						      unsigned int r_type)
{
  gold_assert(r_type != elfcpp::R_POWERPC_NONE);

  // These are the relocation types supported by glibc for both 32-bit
  // and 64-bit powerpc.
  switch (r_type)
    {
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_DTPMOD:
    case elfcpp::R_POWERPC_DTPREL:
    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_REL24:
      return;

    default:
      break;
    }

  if (size == 64)
    {
      switch (r_type)
	{
	  // These are the relocation types supported only on 64-bit.
	case elfcpp::R_PPC64_ADDR64:
	case elfcpp::R_PPC64_TPREL16_LO_DS:
	case elfcpp::R_PPC64_TPREL16_DS:
	case elfcpp::R_POWERPC_TPREL16:
	case elfcpp::R_POWERPC_TPREL16_LO:
	case elfcpp::R_POWERPC_TPREL16_HI:
	case elfcpp::R_POWERPC_TPREL16_HA:
	case elfcpp::R_PPC64_TPREL16_HIGHER:
	case elfcpp::R_PPC64_TPREL16_HIGHEST:
	case elfcpp::R_PPC64_TPREL16_HIGHERA:
	case elfcpp::R_PPC64_TPREL16_HIGHESTA:
	case elfcpp::R_PPC64_ADDR16_LO_DS:
	case elfcpp::R_POWERPC_ADDR16_LO:
	case elfcpp::R_POWERPC_ADDR16_HI:
	case elfcpp::R_POWERPC_ADDR16_HA:
	case elfcpp::R_POWERPC_ADDR30:
	case elfcpp::R_PPC64_UADDR64:
	case elfcpp::R_POWERPC_UADDR32:
	case elfcpp::R_POWERPC_ADDR16:
	case elfcpp::R_POWERPC_UADDR16:
	case elfcpp::R_PPC64_ADDR16_DS:
	case elfcpp::R_PPC64_ADDR16_HIGHER:
	case elfcpp::R_PPC64_ADDR16_HIGHEST:
	case elfcpp::R_PPC64_ADDR16_HIGHERA:
	case elfcpp::R_PPC64_ADDR16_HIGHESTA:
	case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
	case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
	case elfcpp::R_POWERPC_REL32:
	case elfcpp::R_PPC64_REL64:
	  return;

	default:
	  break;
	}
    }
  else
    {
      switch (r_type)
	{
	  // These are the relocation types supported only on 32-bit.

	default:
	  break;
	}
    }

  // This prevents us from issuing more than one error per reloc
  // section.  But we can still wind up issuing more than one
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
Target_powerpc<size, big_endian>::Scan::local(
    Symbol_table* symtab,
    Layout* layout,
    Target_powerpc<size, big_endian>* target,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    Output_section* output_section,
    const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type,
    const elfcpp::Sym<size, big_endian>& lsym)
{
  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(object);

  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC64_TOCSAVE:
    case elfcpp::R_PPC_EMB_MRKREF:
      break;

    case elfcpp::R_PPC64_TOC:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	if (parameters->options().output_is_position_independent())
	  {
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    rela_dyn->add_output_section_relative(got->output_section(),
						  elfcpp::R_POWERPC_RELATIVE,
						  output_section,
						  object, data_shndx,
						  reloc.get_r_offset(),
						  ppc_object->toc_base_offset());
	  }
      }
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      // If building a shared library (or a position-independent
      // executable), we need to create a dynamic relocation for
      // this location.
      if (parameters->options().output_is_position_independent())
	{
	  Reloc_section* rela_dyn = target->rela_dyn_section(layout);

	  if ((size == 32 && r_type == elfcpp::R_POWERPC_ADDR32)
	      || (size == 64 && r_type == elfcpp::R_PPC64_ADDR64))
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	      rela_dyn->add_local_relative(object, r_sym,
					   elfcpp::R_POWERPC_RELATIVE,
					   output_section, data_shndx,
					   reloc.get_r_offset(),
					   reloc.get_r_addend(), false);
	    }
	  else
	    {
	      check_non_pic(object, r_type);
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	      rela_dyn->add_local(object, r_sym, r_type, output_section,
				  data_shndx, reloc.get_r_offset(),
				  reloc.get_r_addend());
	    }
	}
      break;

    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
    case elfcpp::R_PPC64_TLSGD:
    case elfcpp::R_PPC64_TLSLD:
      break;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
      {
	// The symbol requires a GOT entry.
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());

	// If we are generating a shared object, we need to add a
	// dynamic relocation for this symbol's GOT entry.
	if (parameters->options().output_is_position_independent())
	  {
	    if (!object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD))
	      {
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		unsigned int off;

		off = got->add_constant(0);
		object->set_local_got_offset(r_sym, GOT_TYPE_STANDARD, off);
		rela_dyn->add_local_relative(object, r_sym,
					     elfcpp::R_POWERPC_RELATIVE,
					     got, off, 0, false);
	      }
	  }
	else
	  got->add_local(object, r_sym, GOT_TYPE_STANDARD);
      }
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_gd(true);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	    unsigned int shndx = lsym.get_st_shndx();
	    bool is_ordinary;
	    shndx = object->adjust_sym_shndx(r_sym, shndx, &is_ordinary);
	    gold_assert(is_ordinary);
	    got->add_local_pair_with_rel(object, r_sym,
					 shndx,
					 GOT_TYPE_TLSGD,
					 target->rela_dyn_section(layout),
					 elfcpp::R_POWERPC_DTPMOD,
					 elfcpp::R_POWERPC_DTPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ld();
	if (tls_type == tls::TLSOPT_NONE)
	  target->tlsld_got_offset(symtab, layout, object);
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	got->add_local_with_rel(object, r_sym, GOT_TYPE_DTPREL,
				target->rela_dyn_section(layout),
				elfcpp::R_POWERPC_DTPREL);
      }
      break;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ie(true);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	    got->add_local_with_rel(object, r_sym, GOT_TYPE_TPREL,
				    target->rela_dyn_section(layout),
				    elfcpp::R_POWERPC_TPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    default:
      unsupported_reloc_local(object, r_type);
      break;
    }
}

// Report an unsupported relocation against a global symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Scan::unsupported_reloc_global(
    Sized_relobj_file<size, big_endian>* object,
    unsigned int r_type,
    Symbol* gsym)
{
  gold_error(_("%s: unsupported reloc %u against global symbol %s"),
	     object->name().c_str(), r_type, gsym->demangled_name().c_str());
}

// Scan a relocation for a global symbol.

template<int size, bool big_endian>
inline void
Target_powerpc<size, big_endian>::Scan::global(
    Symbol_table* symtab,
    Layout* layout,
    Target_powerpc<size, big_endian>* target,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    Output_section* output_section,
    const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type,
    Symbol* gsym)
{
  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(object);

  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_PPC_EMB_MRKREF:
      break;

    case elfcpp::R_PPC64_TOC:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	if (parameters->options().output_is_position_independent())
	  {
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    Powerpc_relobj<size, big_endian>* symobj = ppc_object;
	    if (data_shndx != ppc_object->opd_shndx())
	      symobj = static_cast
		<Powerpc_relobj<size, big_endian>*>(gsym->object());
	    rela_dyn->add_output_section_relative(got->output_section(),
						  elfcpp::R_POWERPC_RELATIVE,
						  output_section,
						  object, data_shndx,
						  reloc.get_r_offset(),
						  symobj->toc_base_offset());
	  }
      }
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      {
	// Make a PLT entry if necessary.
	if (gsym->needs_plt_entry())
	  {
	    target->make_plt_entry(layout, gsym, reloc, 0);
	    // Since this is not a PC-relative relocation, we may be
	    // taking the address of a function. In that case we need to
	    // set the entry in the dynamic symbol table to the address of
	    // the PLT entry.
	    if (size == 32
		&& gsym->is_from_dynobj() && !parameters->options().shared())
	      gsym->set_needs_dynsym_value();
	  }
	// Make a dynamic relocation if necessary.
	if (needs_dynamic_reloc<size>(gsym, Scan::get_reference_flags(r_type)))
	  {
	    if (gsym->may_need_copy_reloc())
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym, reloc);
	      }
	    else if (((size == 32 && r_type == elfcpp::R_POWERPC_ADDR32)
		      || (size == 64 && r_type == elfcpp::R_PPC64_ADDR64))
		     && (gsym->can_use_relative_reloc(false)
			 || data_shndx == ppc_object->opd_shndx()))
	      {
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		rela_dyn->add_global_relative(gsym, elfcpp::R_POWERPC_RELATIVE,
					      output_section, object,
					      data_shndx, reloc.get_r_offset(),
					      reloc.get_r_addend(), false);
	      }
	    else
	      {
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		check_non_pic(object, r_type);
		rela_dyn->add_global(gsym, r_type, output_section,
				     object, data_shndx,
				     reloc.get_r_offset(),
				     reloc.get_r_addend());
	      }
	  }
      }
      break;

    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_POWERPC_REL24:
      {
	if (gsym->needs_plt_entry()
	    || (!gsym->final_value_is_known()
		 && !(gsym->is_defined()
		      && !gsym->is_from_dynobj()
		      && !gsym->is_preemptible())))
	  target->make_plt_entry(layout, gsym, reloc, object);
	// Make a dynamic relocation if necessary.
	if (needs_dynamic_reloc<size>(gsym, Scan::get_reference_flags(r_type)))
	  {
	    if (gsym->may_need_copy_reloc())
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym,
				   reloc);
	      }
	    else
	      {
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		check_non_pic(object, r_type);
		rela_dyn->add_global(gsym, r_type, output_section, object,
				     data_shndx, reloc.get_r_offset(),
				     reloc.get_r_addend());
	      }
	  }
      }
      break;

    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
    case elfcpp::R_PPC64_TLSGD:
    case elfcpp::R_PPC64_TLSLD:
      break;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
      {
	// The symbol requires a GOT entry.
	Output_data_got_powerpc<size, big_endian>* got;

	got = target->got_section(symtab, layout);
	if (gsym->final_value_is_known())
	  got->add_global(gsym, GOT_TYPE_STANDARD);
	else
	  {
	    // If this symbol is not fully resolved, we need to add a
	    // dynamic relocation for it.
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    if (gsym->is_from_dynobj()
		|| gsym->is_undefined()
		|| gsym->is_preemptible())
	      got->add_global_with_rel(gsym, GOT_TYPE_STANDARD, rela_dyn,
				       elfcpp::R_POWERPC_GLOB_DAT);
	    else if (!gsym->has_got_offset(GOT_TYPE_STANDARD))
	      {
		unsigned int off = got->add_constant(0);

		gsym->set_got_offset(GOT_TYPE_STANDARD, off);
		rela_dyn->add_global_relative(gsym, elfcpp::R_POWERPC_RELATIVE,
					      got, off, 0, false);
	      }
	  }
      }
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
      {
	const bool final = gsym->final_value_is_known();
	const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    got->add_global_pair_with_rel(gsym, GOT_TYPE_TLSGD,
					  target->rela_dyn_section(layout),
					  elfcpp::R_POWERPC_DTPMOD,
					  elfcpp::R_POWERPC_DTPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_IE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    got->add_global_with_rel(gsym, GOT_TYPE_TPREL,
				     target->rela_dyn_section(layout),
				     elfcpp::R_POWERPC_TPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ld();
	if (tls_type == tls::TLSOPT_NONE)
	  target->tlsld_got_offset(symtab, layout, object);
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	got->add_global_with_rel(gsym, GOT_TYPE_DTPREL,
				 target->rela_dyn_section(layout),
				 elfcpp::R_POWERPC_DTPREL);
      }
      break;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
      {
	const bool final = gsym->final_value_is_known();
	const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    got->add_global_with_rel(gsym, GOT_TYPE_TPREL,
				     target->rela_dyn_section(layout),
				     elfcpp::R_POWERPC_TPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    default:
      unsupported_reloc_global(object, r_type, gsym);
      break;
    }
}

// Process relocations for gc.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::gc_process_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols)
{
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Scan Scan;

  gold::gc_process_relocs<size, big_endian, Powerpc, elfcpp::SHT_RELA, Scan,
			  typename Target_powerpc::Relocatable_size_for_reloc>(
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
Target_powerpc<size, big_endian>::scan_relocs(
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
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Scan Scan;

  if (sh_type == elfcpp::SHT_REL)
    {
      gold_error(_("%s: unsupported REL reloc section"),
		 object->name().c_str());
      return;
    }

  if (size == 32)
    {
      static Output_data_space* sdata;

      // Define _SDA_BASE_ at the start of the .sdata section.
      if (sdata == NULL)
	{
	  // layout->find_output_section(".sdata") == NULL
	  sdata = new Output_data_space(4, "** sdata");
	  Output_section* os
	    = layout->add_output_section_data(".sdata", 0,
					      elfcpp::SHF_ALLOC
					      | elfcpp::SHF_WRITE,
					      sdata, ORDER_SMALL_DATA, false);
	  symtab->define_in_output_data("_SDA_BASE_", NULL,
					Symbol_table::PREDEFINED,
					os, 32768, 0, elfcpp::STT_OBJECT,
					elfcpp::STB_LOCAL, elfcpp::STV_HIDDEN,
					0, false, false);
	}
    }

  gold::scan_relocs<size, big_endian, Powerpc, elfcpp::SHT_RELA, Scan>(
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
Target_powerpc<size, big_endian>::do_finalize_sections(
    Layout* layout,
    const Input_objects*,
    Symbol_table*)
{
  // Fill in some more dynamic tags.
  const Reloc_section* rel_plt = (this->plt_ == NULL
				  ? NULL
				  : this->plt_->rel_plt());
  layout->add_target_dynamic_tags(false, this->plt_, rel_plt,
				  this->rela_dyn_, true, size == 32);

  Output_data_dynamic* odyn = layout->dynamic_data();
  if (size == 32)
    {
      if (this->got_ != NULL)
	{
	  this->got_->finalize_data_size();
	  odyn->add_section_plus_offset(elfcpp::DT_PPC_GOT,
					this->got_, this->got_->g_o_t());
	}
    }
  else
    {
      if (this->glink_ != NULL)
	{
	  this->glink_->finalize_data_size();
	  odyn->add_section_plus_offset(elfcpp::DT_PPC64_GLINK,
					this->glink_,
					(this->glink_->pltresolve()
					 + this->glink_->pltresolve_size - 32));
	}
    }

  // Emit any relocs we saved in an attempt to avoid generating COPY
  // relocs.
  if (this->copy_relocs_.any_saved_relocs())
    this->copy_relocs_.emit(this->rela_dyn_section(layout));
}

// Perform a relocation.

template<int size, bool big_endian>
inline bool
Target_powerpc<size, big_endian>::Relocate::relocate(
    const Relocate_info<size, big_endian>* relinfo,
    Target_powerpc* target,
    Output_section* os,
    size_t relnum,
    const elfcpp::Rela<size, big_endian>& rela,
    unsigned int r_type,
    const Sized_symbol<size>* gsym,
    const Symbol_value<size>* psymval,
    unsigned char* view,
    Address address,
    section_size_type view_size)
{

  bool is_tls_call = ((r_type == elfcpp::R_POWERPC_REL24
		       || r_type == elfcpp::R_PPC_PLTREL24)
		      && gsym != NULL
		      && strcmp(gsym->name(), "__tls_get_addr") == 0);
  enum skip_tls last_tls = this->call_tls_get_addr_;
  this->call_tls_get_addr_ = CALL_NOT_EXPECTED;
  if (is_tls_call)
    {
      if (last_tls == CALL_NOT_EXPECTED)
	gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			       _("__tls_get_addr call lacks marker reloc"));
      else if (last_tls == CALL_SKIP)
	return false;
    }
  else if (last_tls != CALL_NOT_EXPECTED)
    gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			   _("missing expected __tls_get_addr call"));

  typedef Powerpc_relocate_functions<size, big_endian> Reloc;
  typedef typename elfcpp::Swap<32, big_endian>::Valtype Insn;
  const Powerpc_relobj<size, big_endian>* const object
    = static_cast<const Powerpc_relobj<size, big_endian>*>(relinfo->object);
  Address value = 0;
  bool has_plt_value = false;
  if (gsym != NULL
      && use_plt_offset<size>(gsym, Scan::get_reference_flags(r_type)))
    {
      const Output_data_glink<size, big_endian>* glink
	= target->glink_section();
      unsigned int glink_index = glink->find_entry(gsym, rela, object);
      value = glink->address() + glink_index * glink->glink_entry_size();
      has_plt_value = true;
    }

  if (r_type == elfcpp::R_POWERPC_GOT16
      || r_type == elfcpp::R_POWERPC_GOT16_LO
      || r_type == elfcpp::R_POWERPC_GOT16_HI
      || r_type == elfcpp::R_POWERPC_GOT16_HA
      || r_type == elfcpp::R_PPC64_GOT16_DS
      || r_type == elfcpp::R_PPC64_GOT16_LO_DS)
    {
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_STANDARD));
	  value = gsym->got_offset(GOT_TYPE_STANDARD);
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD));
	  value = object->local_got_offset(r_sym, GOT_TYPE_STANDARD);
	}
      value -= target->got_section()->got_base_offset(object);
    }
  else if (r_type == elfcpp::R_PPC64_TOC)
    {
      value = (target->got_section()->output_section()->address()
	       + object->toc_base_offset());
    }
  else if (gsym != NULL
	   && (r_type == elfcpp::R_POWERPC_REL24
	       || r_type == elfcpp::R_PPC_PLTREL24)
	   && has_plt_value)
    {
      if (size == 64)
	{
	  typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
	  Valtype* wv = reinterpret_cast<Valtype*>(view);
	  bool can_plt_call = false;
	  if (rela.get_r_offset() + 8 <= view_size)
	    {
	      Valtype insn2 = elfcpp::Swap<32, big_endian>::readval(wv + 1);
	      if (insn2 == nop
		  || insn2 == cror_15_15_15 || insn2 == cror_31_31_31)
		{
		  elfcpp::Swap<32, big_endian>::writeval(wv + 1, ld_2_1 + 40);
		  can_plt_call = true;
		}
	    }
	  if (!can_plt_call)
	    gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
				   _("call lacks nop, can't restore toc"));
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HA)
    {
      // First instruction of a global dynamic sequence, arg setup insn.
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
      enum Got_type got_type = GOT_TYPE_STANDARD;
      if (tls_type == tls::TLSOPT_NONE)
	got_type = GOT_TYPE_TLSGD;
      else if (tls_type == tls::TLSOPT_TO_IE)
	got_type = GOT_TYPE_TPREL;
      if (got_type != GOT_TYPE_STANDARD)
	{
	  if (gsym != NULL)
	    {
	      gold_assert(gsym->has_got_offset(got_type));
	      value = gsym->got_offset(got_type);
	    }
	  else
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	      gold_assert(object->local_has_got_offset(r_sym, got_type));
	      value = object->local_got_offset(r_sym, got_type);
	    }
	  value -= target->got_section()->got_base_offset(object);
	}
      if (tls_type == tls::TLSOPT_TO_IE)
	{
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      insn &= (1 << 26) - (1 << 16); // extract rt,ra from addi
	      if (size == 32)
		insn |= 32 << 26; // lwz
	      else
		insn |= 58 << 26; // ld
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	    }
	  r_type += (elfcpp::R_POWERPC_GOT_TPREL16
		     - elfcpp::R_POWERPC_GOT_TLSGD16);
	}
      else if (tls_type == tls::TLSOPT_TO_LE)
	{
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = addis_3_13;
	      if (size == 32)
		insn = addis_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HA)
    {
      // First instruction of a local dynamic sequence, arg setup insn.
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type == tls::TLSOPT_NONE)
	{
	  value = target->tlsld_got_offset();
	  value -= target->got_section()->got_base_offset(object);
	}
      else
	{
	  gold_assert(tls_type == tls::TLSOPT_TO_LE);
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = addis_3_13;
	      if (size == 32)
		insn = addis_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = relinfo->layout->tls_segment()->vaddr() + dtp_offset;
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_DTPREL16
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_HA)
    {
      // Accesses relative to a local dynamic sequence address,
      // no optimisation here.
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_DTPREL));
	  value = gsym->got_offset(GOT_TYPE_DTPREL);
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_DTPREL));
	  value = object->local_got_offset(r_sym, GOT_TYPE_DTPREL);
	}
      value -= target->got_section()->got_base_offset(object);
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HA)
    {
      // First instruction of initial exec sequence.
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
      if (tls_type == tls::TLSOPT_NONE)
	{
	  if (gsym != NULL)
	    {
	      gold_assert(gsym->has_got_offset(GOT_TYPE_TPREL));
	      value = gsym->got_offset(GOT_TYPE_TPREL);
	    }
	  else
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	      gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_TPREL));
	      value = object->local_got_offset(r_sym, GOT_TYPE_TPREL);
	    }
	  value -= target->got_section()->got_base_offset(object);
	}
      else
	{
	  gold_assert(tls_type == tls::TLSOPT_TO_LE);
	  if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
	      || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      insn &= (1 << 26) - (1 << 21); // extract rt from ld
	      if (size == 32)
		insn |= addis_0_2;
	      else
		insn |= addis_0_13;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSGD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSGD))
    {
      // Second instruction of a global dynamic sequence,
      // the __tls_get_addr call
      this->call_tls_get_addr_ = CALL_EXPECTED;
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
      if (tls_type != tls::TLSOPT_NONE)
	{
	  if (tls_type == tls::TLSOPT_TO_IE)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view);
	      Insn insn = add_3_3_13;
	      if (size == 32)
		insn = add_3_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view);
	      Insn insn = addi_3_3;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_LO;
	      view += 2 * big_endian;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  this->call_tls_get_addr_ = CALL_SKIP;
	}
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSLD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSLD))
    {
      // Second instruction of a local dynamic sequence,
      // the __tls_get_addr call
      this->call_tls_get_addr_ = CALL_EXPECTED;
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type == tls::TLSOPT_TO_LE)
	{
	  Insn* iview = reinterpret_cast<Insn*>(view);
	  Insn insn = addi_3_3;
	  elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	  this->call_tls_get_addr_ = CALL_SKIP;
	  r_type = elfcpp::R_POWERPC_TPREL16_LO;
	  view += 2 * big_endian;
	  value = relinfo->layout->tls_segment()->vaddr() + dtp_offset;
	}
    }
  else if (r_type == elfcpp::R_POWERPC_TLS)
    {
      // Second instruction of an initial exec sequence
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
      if (tls_type == tls::TLSOPT_TO_LE)
	{
	  Insn* iview = reinterpret_cast<Insn*>(view);
	  Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	  unsigned int reg = size == 32 ? 2 : 13;
	  insn = at_tls_transform(insn, reg);
	  gold_assert(insn != 0);
	  elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	  r_type = elfcpp::R_POWERPC_TPREL16_LO;
	  view += 2 * big_endian;
	  value = psymval->value(object, rela.get_r_addend());
	}
    }
  else
    {
      Address addend = 0;
      if (r_type != elfcpp::R_PPC_PLTREL24)
	addend = rela.get_r_addend();
      if (size == 64 || !has_plt_value)
	value = psymval->value(object, addend);
      if (size == 64 && is_branch_reloc(r_type))
	{
	  // If the symbol is defined in an opd section, ie. is a function
	  // descriptor, use the function descriptor code entry address
	  Powerpc_relobj<size, big_endian>* symobj = const_cast
	    <Powerpc_relobj<size, big_endian>*>(object);
	  if (gsym != NULL)
	    symobj = static_cast
	      <Powerpc_relobj<size, big_endian>*>(gsym->object());
	  unsigned int shndx = symobj->opd_shndx();
	  Address opd_addr = symobj->get_output_section_offset(shndx);
	  gold_assert(opd_addr != invalid_address);
	  opd_addr += symobj->output_section(shndx)->address();
	  if (value >= opd_addr
	      && value < opd_addr + symobj->section_size(shndx))
	    {
	      Address sec_off;
	      symobj->get_opd_ent(value - opd_addr, &shndx, &sec_off);
	      Address sec_addr = symobj->get_output_section_offset(shndx);
	      gold_assert(sec_addr != invalid_address);
	      sec_addr += symobj->output_section(shndx)->address();
	      value = sec_addr + sec_off;
	    }
	}
    }

  switch (r_type)
    {
    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      value -= address;
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // Subtract the TOC base address.
      value -= (target->got_section()->output_section()->address()
		+ object->toc_base_offset());
      break;

    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
      if (os != NULL)
	value -= os->address();
      break;

    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
      if (size != 64)
	// R_PPC_TLSGD and R_PPC_TLSLD
	break;
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
      // tls symbol values are relative to tls_segment()->vaddr()
      value -= tp_offset;
      break;

    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
      if (size != 64)
	// R_PPC_EMB_NADDR32, R_PPC_EMB_NADDR16, R_PPC_EMB_NADDR16_LO
	// R_PPC_EMB_NADDR16_HI, R_PPC_EMB_NADDR16_HA, R_PPC_EMB_SDAI16
	break;
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL:
      // tls symbol values are relative to tls_segment()->vaddr()
      value -= dtp_offset;
      break;

    default:
      break;
    }

  Insn branch_bit = 0;
  switch (r_type)
    {
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
      branch_bit = 1 << 21;
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      {
	Insn* iview = reinterpret_cast<Insn*>(view);
	Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	insn &= ~(1 << 21);
	insn |= branch_bit;
	if (this->is_isa_v2)
	  {
	    // Set 'a' bit.  This is 0b00010 in BO field for branch
	    // on CR(BI) insns (BO == 001at or 011at), and 0b01000
	    // for branch on CTR insns (BO == 1a00t or 1a01t).
	    if ((insn & (0x14 << 21)) == (0x04 << 21))
	      insn |= 0x02 << 21;
	    else if ((insn & (0x14 << 21)) == (0x10 << 21))
	      insn |= 0x08 << 21;
	    else
	      break;
	  }
	else
	  {
	    // Invert 'y' bit if not the default.
	    if (static_cast<Signed_address>(value) < 0)
	      insn ^= 1 << 21;
	  }
	elfcpp::Swap<32, big_endian>::writeval(iview, insn);
      }
      break;

    default:
      break;
    }

  enum Reloc::overflow_check overflow = Reloc::check_none;
  switch (r_type)
    {
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
      if (size == 64)
	overflow = Reloc::check_bitfield;
      break;

    case elfcpp::R_POWERPC_REL32:
      if (size == 64)
	overflow = Reloc::check_signed;
      break;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      overflow = Reloc::check_bitfield;
      break;

    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16:
      overflow = Reloc::check_signed;
      break;
    }

  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_TLS:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC_EMB_MRKREF:
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_PPC64_TOC:
      Reloc::addr64(view, value);
      break;

    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_POWERPC_DTPREL:
      if (size == 64)
	Reloc::addr64(view, value);
      else
	Reloc::addr32(view, value, overflow);
      break;

    case elfcpp::R_PPC64_UADDR64:
      Reloc::addr64_u(view, value);
      break;

    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_REL32:
      Reloc::addr32(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_UADDR32:
      Reloc::addr32_u(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
      Reloc::addr24(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
      if (size == 64)
	{
	  Reloc::addr16_ds(view, value, overflow);
	  break;
	}
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
      Reloc::addr16(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_UADDR16:
      Reloc::addr16_u(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
      Reloc::addr16_hi(view, value);
      break;

    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      Reloc::addr16_ha(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHER:
      if (size == 32)
	// R_PPC_EMB_NADDR16_LO
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
      Reloc::addr16_hi2(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
      if (size == 32)
	// R_PPC_EMB_NADDR16_HI
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
      Reloc::addr16_ha2(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
      if (size == 32)
	// R_PPC_EMB_NADDR16_HA
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
      Reloc::addr16_hi3(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
      if (size == 32)
	// R_PPC_EMB_SDAI16
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
      Reloc::addr16_ha3(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
      if (size == 32)
	// R_PPC_EMB_NADDR32, R_PPC_EMB_NADDR16
	goto unsupp;
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
      if (size == 32)
	// R_PPC_TLSGD, R_PPC_TLSLD
	break;
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
      Reloc::addr16_ds(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      Reloc::addr14(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_DTPMOD:
    case elfcpp::R_PPC64_JMP_IREL:
    case elfcpp::R_POWERPC_IRELATIVE:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("unexpected reloc %u in object file"),
			     r_type);
      break;

    case elfcpp::R_PPC_EMB_SDA21:
      if (size == 32)
	goto unsupp;
      else
	{
	  // R_PPC64_TOCSAVE.  For the time being this can be ignored.
	}
      break;

    case elfcpp::R_PPC_EMB_SDA2I16:
    case elfcpp::R_PPC_EMB_SDA2REL:
      if (size == 32)
	goto unsupp;
      // R_PPC64_TLSGD, R_PPC64_TLSLD
      break;

    case elfcpp::R_POWERPC_PLT32:
    case elfcpp::R_POWERPC_PLTREL32:
    case elfcpp::R_POWERPC_PLT16_LO:
    case elfcpp::R_POWERPC_PLT16_HI:
    case elfcpp::R_POWERPC_PLT16_HA:
    case elfcpp::R_PPC_SDAREL16:
    case elfcpp::R_POWERPC_ADDR30:
    case elfcpp::R_PPC64_PLT64:
    case elfcpp::R_PPC64_PLTREL64:
    case elfcpp::R_PPC64_PLTGOT16:
    case elfcpp::R_PPC64_PLTGOT16_LO:
    case elfcpp::R_PPC64_PLTGOT16_HI:
    case elfcpp::R_PPC64_PLTGOT16_HA:
    case elfcpp::R_PPC64_PLT16_LO_DS:
    case elfcpp::R_PPC64_PLTGOT16_DS:
    case elfcpp::R_PPC64_PLTGOT16_LO_DS:
    case elfcpp::R_PPC_EMB_RELSEC16:
    case elfcpp::R_PPC_EMB_RELST_LO:
    case elfcpp::R_PPC_EMB_RELST_HI:
    case elfcpp::R_PPC_EMB_RELST_HA:
    case elfcpp::R_PPC_EMB_BIT_FLD:
    case elfcpp::R_PPC_EMB_RELSDA:
    case elfcpp::R_PPC_TOC16:
    default:
    unsupp:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("unsupported reloc %u"),
			     r_type);
      break;
    }

  return true;
}

// Relocate section data.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::relocate_section(
    const Relocate_info<size, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    unsigned char* view,
    Address address,
    section_size_type view_size,
    const Reloc_symbol_changes* reloc_symbol_changes)
{
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Relocate Powerpc_relocate;

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_section<size, big_endian, Powerpc, elfcpp::SHT_RELA,
			 Powerpc_relocate>(
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

class Powerpc_scan_relocatable_reloc
{
public:
  // Return the strategy to use for a local symbol which is not a
  // section symbol, given the relocation type.
  inline Relocatable_relocs::Reloc_strategy
  local_non_section_strategy(unsigned int r_type, Relobj*, unsigned int r_sym)
  {
    if (r_type == 0 && r_sym == 0)
      return Relocatable_relocs::RELOC_DISCARD;
    return Relocatable_relocs::RELOC_COPY;
  }

  // Return the strategy to use for a local symbol which is a section
  // symbol, given the relocation type.
  inline Relocatable_relocs::Reloc_strategy
  local_section_strategy(unsigned int, Relobj*)
  {
    return Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA;
  }

  // Return the strategy to use for a global symbol, given the
  // relocation type, the object, and the symbol index.
  inline Relocatable_relocs::Reloc_strategy
  global_strategy(unsigned int r_type, Relobj*, unsigned int)
  {
    if (r_type == elfcpp::R_PPC_PLTREL24)
      return Relocatable_relocs::RELOC_SPECIAL;
    return Relocatable_relocs::RELOC_COPY;
  }
};

// Scan the relocs during a relocatable link.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::scan_relocatable_relocs(
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
    const unsigned char* plocal_symbols,
    Relocatable_relocs* rr)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::scan_relocatable_relocs<size, big_endian, elfcpp::SHT_RELA,
				Powerpc_scan_relocatable_reloc>(
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
// This is a modified version of the function by the same name in
// target-reloc.h.  Using relocate_special_relocatable for
// R_PPC_PLTREL24 would require duplication of the entire body of the
// loop, so we may as well duplicate the whole thing.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::relocate_for_relocatable(
    const Relocate_info<size, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    off_t offset_in_output_section,
    const Relocatable_relocs* rr,
    unsigned char*,
    Address view_address,
    section_size_type,
    unsigned char* reloc_view,
    section_size_type reloc_view_size)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);

  typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc
    Reltype;
  typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc_write
    Reltype_write;
  const int reloc_size
    = Reloc_types<elfcpp::SHT_RELA, size, big_endian>::reloc_size;

  Powerpc_relobj<size, big_endian>* const object
    = static_cast<Powerpc_relobj<size, big_endian>*>(relinfo->object);
  const unsigned int local_count = object->local_symbol_count();
  unsigned int got2_shndx = object->got2_shndx();
  Address got2_addend = 0;
  if (got2_shndx != 0)
    {
      got2_addend = object->get_output_section_offset(got2_shndx);
      gold_assert(got2_addend != invalid_address);
    }

  unsigned char* pwrite = reloc_view;

  for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
    {
      Relocatable_relocs::Reloc_strategy strategy = rr->strategy(i);
      if (strategy == Relocatable_relocs::RELOC_DISCARD)
	continue;

      Reltype reloc(prelocs);
      Reltype_write reloc_write(pwrite);

      typename elfcpp::Elf_types<size>::Elf_WXword r_info = reloc.get_r_info();
      const unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
      const unsigned int r_type = elfcpp::elf_r_type<size>(r_info);

      // Get the new symbol index.

      unsigned int new_symndx;
      if (r_sym < local_count)
	{
	  switch (strategy)
	    {
	    case Relocatable_relocs::RELOC_COPY:
	    case Relocatable_relocs::RELOC_SPECIAL:
	      if (r_sym == 0)
		new_symndx = 0;
	      else
		{
		  new_symndx = object->symtab_index(r_sym);
		  gold_assert(new_symndx != -1U);
		}
	      break;

	    case Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA:
	      {
		// We are adjusting a section symbol.  We need to find
		// the symbol table index of the section symbol for
		// the output section corresponding to input section
		// in which this symbol is defined.
		gold_assert(r_sym < local_count);
		bool is_ordinary;
		unsigned int shndx =
		  object->local_symbol_input_shndx(r_sym, &is_ordinary);
		gold_assert(is_ordinary);
		Output_section* os = object->output_section(shndx);
		gold_assert(os != NULL);
		gold_assert(os->needs_symtab_index());
		new_symndx = os->symtab_index();
	      }
	      break;

	    default:
	      gold_unreachable();
	    }
	}
      else
	{
	  const Symbol* gsym = object->global_symbol(r_sym);
	  gold_assert(gsym != NULL);
	  if (gsym->is_forwarder())
	    gsym = relinfo->symtab->resolve_forwards(gsym);

	  gold_assert(gsym->has_symtab_index());
	  new_symndx = gsym->symtab_index();
	}

      // Get the new offset--the location in the output section where
      // this relocation should be applied.

      Address offset = reloc.get_r_offset();
      Address new_offset;
      if (static_cast<Address>(offset_in_output_section) != invalid_address)
	new_offset = offset + offset_in_output_section;
      else
	{
	  section_offset_type sot_offset =
	    convert_types<section_offset_type, Address>(offset);
	  section_offset_type new_sot_offset =
	    output_section->output_offset(object, relinfo->data_shndx,
					  sot_offset);
	  gold_assert(new_sot_offset != -1);
	  new_offset = new_sot_offset;
	}

      // In an object file, r_offset is an offset within the section.
      // In an executable or dynamic object, generated by
      // --emit-relocs, r_offset is an absolute address.
      // FIXME: Arrange to call this function for --emit-relocs too,
      // so that we can make emitted relocs match edited TLS code.
      if (0 && !parameters->options().relocatable())
	{
	  new_offset += view_address;
	  if (static_cast<Address>(offset_in_output_section) != invalid_address)
	    new_offset -= offset_in_output_section;
	}

      reloc_write.put_r_offset(new_offset);
      reloc_write.put_r_info(elfcpp::elf_r_info<size>(new_symndx, r_type));

      // Handle the reloc addend based on the strategy.
      typename elfcpp::Elf_types<size>::Elf_Swxword addend;
      addend = Reloc_types<elfcpp::SHT_RELA, size, big_endian>::
	get_reloc_addend(&reloc);

      if (strategy == Relocatable_relocs::RELOC_COPY)
	;
      else if (strategy == Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA)
	{
	  const Symbol_value<size>* psymval = object->local_symbol(r_sym);
	  addend = psymval->value(object, addend);
	}
      else if (strategy == Relocatable_relocs::RELOC_SPECIAL)
	{
	  if (addend >= 32768)
	    addend += got2_addend;
	}
      else
	gold_unreachable();

      Reloc_types<elfcpp::SHT_RELA, size, big_endian>::
	set_reloc_addend(&reloc_write, addend);

      pwrite += reloc_size;
    }

  gold_assert(static_cast<section_size_type>(pwrite - reloc_view)
	      == reloc_view_size);
}

// Return the value to use for a dynamic which requires special
// treatment.  This is how we support equality comparisons of function
// pointers across shared library boundaries, as described in the
// processor specific ABI supplement.

template<int size, bool big_endian>
uint64_t
Target_powerpc<size, big_endian>::do_dynsym_value(const Symbol* gsym) const
{
  if (size == 32)
    {
      gold_assert(gsym->is_from_dynobj() && gsym->has_plt_offset());
      return this->plt_section()->address() + gsym->plt_offset();
    }
  else
    gold_unreachable();
}

// The selector for powerpc object files.

template<int size, bool big_endian>
class Target_selector_powerpc : public Target_selector
{
public:
  Target_selector_powerpc()
    : Target_selector(elfcpp::EM_NONE, size, big_endian,
		      (size == 64
		       ? (big_endian ? "elf64-powerpc" : "elf64-powerpcle")
		       : (big_endian ? "elf32-powerpc" : "elf32-powerpcle")),
		      (size == 64
		       ? (big_endian ? "elf64ppc" : "elf64lppc")
		       : (big_endian ? "elf32ppc" : "elf32lppc")))
  { }

  virtual Target*
  do_recognize(Input_file*, off_t, int machine, int, int)
  {
    switch (size)
      {
      case 64:
	if (machine != elfcpp::EM_PPC64)
	  return NULL;
	break;

      case 32:
	if (machine != elfcpp::EM_PPC)
	  return NULL;
	break;

      default:
	return NULL;
      }

    return this->instantiate_target();
  }

  virtual Target*
  do_instantiate_target()
  { return new Target_powerpc<size, big_endian>(); }
};

Target_selector_powerpc<32, true> target_selector_ppc32;
Target_selector_powerpc<32, false> target_selector_ppc32le;
Target_selector_powerpc<64, true> target_selector_ppc64;
Target_selector_powerpc<64, false> target_selector_ppc64le;

} // End anonymous namespace.
