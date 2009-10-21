// arm.cc -- arm target support for gold.

// Copyright 2009 Free Software Foundation, Inc.
// Written by Doug Kwan <dougkwan@google.com> based on the i386 code
// by Ian Lance Taylor <iant@google.com>.
// This file also contains borrowed and adapted code from
// bfd/elf32-arm.c.

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
#include "gc.h"

namespace
{

using namespace gold;

template<bool big_endian>
class Output_data_plt_arm;

template<bool big_endian>
class Target_arm;

// For convenience.
typedef elfcpp::Elf_types<32>::Elf_Addr Arm_address;

// Maximum branch offsets for ARM, THUMB and THUMB2.
const int32_t ARM_MAX_FWD_BRANCH_OFFSET = ((((1 << 23) - 1) << 2) + 8);
const int32_t ARM_MAX_BWD_BRANCH_OFFSET = ((-((1 << 23) << 2)) + 8);
const int32_t THM_MAX_FWD_BRANCH_OFFSET = ((1 << 22) -2 + 4);
const int32_t THM_MAX_BWD_BRANCH_OFFSET = (-(1 << 22) + 4);
const int32_t THM2_MAX_FWD_BRANCH_OFFSET = (((1 << 24) - 2) + 4);
const int32_t THM2_MAX_BWD_BRANCH_OFFSET = (-(1 << 24) + 4);

// The arm target class.
//
// This is a very simple port of gold for ARM-EABI.  It is intended for
// supporting Android only for the time being.  Only these relocation types
// are supported.
//
// R_ARM_NONE
// R_ARM_ABS32
// R_ARM_ABS32_NOI
// R_ARM_ABS16
// R_ARM_ABS12
// R_ARM_ABS8
// R_ARM_THM_ABS5
// R_ARM_BASE_ABS
// R_ARM_REL32
// R_ARM_THM_CALL
// R_ARM_COPY
// R_ARM_GLOB_DAT
// R_ARM_BASE_PREL
// R_ARM_JUMP_SLOT
// R_ARM_RELATIVE
// R_ARM_GOTOFF32
// R_ARM_GOT_BREL
// R_ARM_GOT_PREL
// R_ARM_PLT32
// R_ARM_CALL
// R_ARM_JUMP24
// R_ARM_TARGET1
// R_ARM_PREL31
// R_ARM_ABS8
// R_ARM_MOVW_ABS_NC
// R_ARM_MOVT_ABS
// R_ARM_THM_MOVW_ABS_NC
// R_ARM_THM_MOVT_ABS
// R_ARM_MOVW_PREL_NC
// R_ARM_MOVT_PREL
// R_ARM_THM_MOVW_PREL_NC
// R_ARM_THM_MOVT_PREL
// 
// TODOs:
// - Generate various branch stubs.
// - Support interworking.
// - Define section symbols __exidx_start and __exidx_stop.
// - Support more relocation types as needed. 
// - Make PLTs more flexible for different architecture features like
//   Thumb-2 and BE8.
// There are probably a lot more.

// Instruction template class.  This class is similar to the insn_sequence
// struct in bfd/elf32-arm.c.

class Insn_template
{
 public:
  // Types of instruction templates.
  enum Type
    {
      THUMB16_TYPE = 1,
      THUMB32_TYPE,
      ARM_TYPE,
      DATA_TYPE
    };

  // Factory methods to create instrunction templates in different formats.

  static const Insn_template
  thumb16_insn(uint32_t data)
  { return Insn_template(data, THUMB16_TYPE, elfcpp::R_ARM_NONE, 0); } 

  // A bit of a hack.  A Thumb conditional branch, in which the proper
  // condition is inserted when we build the stub.
  static const Insn_template
  thumb16_bcond_insn(uint32_t data)
  { return Insn_template(data, THUMB16_TYPE, elfcpp::R_ARM_NONE, 1); } 

  static const Insn_template
  thumb32_insn(uint32_t data)
  { return Insn_template(data, THUMB32_TYPE, elfcpp::R_ARM_NONE, 0); } 

  static const Insn_template
  thumb32_b_insn(uint32_t data, int reloc_addend)
  {
    return Insn_template(data, THUMB32_TYPE, elfcpp::R_ARM_THM_JUMP24,
			 reloc_addend);
  } 

  static const Insn_template
  arm_insn(uint32_t data)
  { return Insn_template(data, ARM_TYPE, elfcpp::R_ARM_NONE, 0); }

  static const Insn_template
  arm_rel_insn(unsigned data, int reloc_addend)
  { return Insn_template(data, ARM_TYPE, elfcpp::R_ARM_JUMP24, reloc_addend); }

  static const Insn_template
  data_word(unsigned data, unsigned int r_type, int reloc_addend)
  { return Insn_template(data, DATA_TYPE, r_type, reloc_addend); } 

  // Accessors.  This class is used for read-only objects so no modifiers
  // are provided.

  uint32_t
  data() const
  { return this->data_; }

  // Return the instruction sequence type of this.
  Type
  type() const
  { return this->type_; }

  // Return the ARM relocation type of this.
  unsigned int
  r_type() const
  { return this->r_type_; }

  int32_t
  reloc_addend() const
  { return this->reloc_addend_; }

  // Return size of instrunction template in bytes.
  size_t
  size() const;

  // Return byte-alignment of instrunction template.
  unsigned
  alignment() const;

 private:
  // We make the constructor private to ensure that only the factory
  // methods are used.
  inline
  Insn_template(unsigned data, Type type, unsigned int r_type, int reloc_addend)
    : data_(data), type_(type), r_type_(r_type), reloc_addend_(reloc_addend)
  { }

  // Instruction specific data.  This is used to store information like
  // some of the instruction bits.
  uint32_t data_;
  // Instruction template type.
  Type type_;
  // Relocation type if there is a relocation or R_ARM_NONE otherwise.
  unsigned int r_type_;
  // Relocation addend.
  int32_t reloc_addend_;
};

// Macro for generating code to stub types. One entry per long/short
// branch stub

#define DEF_STUBS \
  DEF_STUB(long_branch_any_any) \
  DEF_STUB(long_branch_v4t_arm_thumb) \
  DEF_STUB(long_branch_thumb_only) \
  DEF_STUB(long_branch_v4t_thumb_thumb) \
  DEF_STUB(long_branch_v4t_thumb_arm) \
  DEF_STUB(short_branch_v4t_thumb_arm) \
  DEF_STUB(long_branch_any_arm_pic) \
  DEF_STUB(long_branch_any_thumb_pic) \
  DEF_STUB(long_branch_v4t_thumb_thumb_pic) \
  DEF_STUB(long_branch_v4t_arm_thumb_pic) \
  DEF_STUB(long_branch_v4t_thumb_arm_pic) \
  DEF_STUB(long_branch_thumb_only_pic) \
  DEF_STUB(a8_veneer_b_cond) \
  DEF_STUB(a8_veneer_b) \
  DEF_STUB(a8_veneer_bl) \
  DEF_STUB(a8_veneer_blx)

// Stub types.

#define DEF_STUB(x) arm_stub_##x,
typedef enum
  {
    arm_stub_none,
    DEF_STUBS

    // First reloc stub type.
    arm_stub_reloc_first = arm_stub_long_branch_any_any,
    // Last  reloc stub type.
    arm_stub_reloc_last = arm_stub_long_branch_thumb_only_pic,

    // First Cortex-A8 stub type.
    arm_stub_cortex_a8_first = arm_stub_a8_veneer_b_cond,
    // Last Cortex-A8 stub type.
    arm_stub_cortex_a8_last = arm_stub_a8_veneer_blx,
    
    // Last stub type.
    arm_stub_type_last = arm_stub_a8_veneer_blx
  } Stub_type;
#undef DEF_STUB

// Stub template class.  Templates are meant to be read-only objects.
// A stub template for a stub type contains all read-only attributes
// common to all stubs of the same type.

class Stub_template
{
 public:
  Stub_template(Stub_type, const Insn_template*, size_t);

  ~Stub_template()
  { }

  // Return stub type.
  Stub_type
  type() const
  { return this->type_; }

  // Return an array of instruction templates.
  const Insn_template*
  insns() const
  { return this->insns_; }

  // Return size of template in number of instructions.
  size_t
  insn_count() const
  { return this->insn_count_; }

  // Return size of template in bytes.
  size_t
  size() const
  { return this->size_; }

  // Return alignment of the stub template.
  unsigned
  alignment() const
  { return this->alignment_; }
  
  // Return whether entry point is in thumb mode.
  bool
  entry_in_thumb_mode() const
  { return this->entry_in_thumb_mode_; }

  // Return number of relocations in this template.
  size_t
  reloc_count() const
  { return this->relocs_.size(); }

  // Return index of the I-th instruction with relocation.
  size_t
  reloc_insn_index(size_t i) const
  {
    gold_assert(i < this->relocs_.size());
    return this->relocs_[i].first;
  }

  // Return the offset of the I-th instruction with relocation from the
  // beginning of the stub.
  section_size_type
  reloc_offset(size_t i) const
  {
    gold_assert(i < this->relocs_.size());
    return this->relocs_[i].second;
  }

 private:
  // This contains information about an instruction template with a relocation
  // and its offset from start of stub.
  typedef std::pair<size_t, section_size_type> Reloc;

  // A Stub_template may not be copied.  We want to share templates as much
  // as possible.
  Stub_template(const Stub_template&);
  Stub_template& operator=(const Stub_template&);
  
  // Stub type.
  Stub_type type_;
  // Points to an array of Insn_templates.
  const Insn_template* insns_;
  // Number of Insn_templates in insns_[].
  size_t insn_count_;
  // Size of templated instructions in bytes.
  size_t size_;
  // Alignment of templated instructions.
  unsigned alignment_;
  // Flag to indicate if entry is in thumb mode.
  bool entry_in_thumb_mode_;
  // A table of reloc instruction indices and offsets.  We can find these by
  // looking at the instruction templates but we pre-compute and then stash
  // them here for speed. 
  std::vector<Reloc> relocs_;
};

//
// A class for code stubs.  This is a base class for different type of
// stubs used in the ARM target.
//

class Stub
{
 private:
  static const section_offset_type invalid_offset =
    static_cast<section_offset_type>(-1);

 public:
  Stub(const Stub_template* stub_template)
    : stub_template_(stub_template), offset_(invalid_offset)
  { }

  virtual
   ~Stub()
  { }

  // Return the stub template.
  const Stub_template*
  stub_template() const
  { return this->stub_template_; }

  // Return offset of code stub from beginning of its containing stub table.
  section_offset_type
  offset() const
  {
    gold_assert(this->offset_ != invalid_offset);
    return this->offset_;
  }

  // Set offset of code stub from beginning of its containing stub table.
  void
  set_offset(section_offset_type offset)
  { this->offset_ = offset; }
  
  // Return the relocation target address of the i-th relocation in the
  // stub.  This must be defined in a child class.
  Arm_address
  reloc_target(size_t i)
  { return this->do_reloc_target(i); }

  // Write a stub at output VIEW.  BIG_ENDIAN select how a stub is written.
  void
  write(unsigned char* view, section_size_type view_size, bool big_endian)
  { this->do_write(view, view_size, big_endian); }

 protected:
  // This must be defined in the child class.
  virtual Arm_address
  do_reloc_target(size_t) = 0;

  // This must be defined in the child class.
  virtual void
  do_write(unsigned char*, section_size_type, bool) = 0;
  
 private:
  // Its template.
  const Stub_template* stub_template_;
  // Offset within the section of containing this stub.
  section_offset_type offset_;
};

// Reloc stub class.  These are stubs we use to fix up relocation because
// of limited branch ranges.

class Reloc_stub : public Stub
{
 public:
  static const unsigned int invalid_index = static_cast<unsigned int>(-1);
  // We assume we never jump to this address.
  static const Arm_address invalid_address = static_cast<Arm_address>(-1);

  // Return destination address.
  Arm_address
  destination_address() const
  {
    gold_assert(this->destination_address_ != this->invalid_address);
    return this->destination_address_;
  }

  // Set destination address.
  void
  set_destination_address(Arm_address address)
  {
    gold_assert(address != this->invalid_address);
    this->destination_address_ = address;
  }

  // Reset destination address.
  void
  reset_destination_address()
  { this->destination_address_ = this->invalid_address; }

  // Determine stub type for a branch of a relocation of R_TYPE going
  // from BRANCH_ADDRESS to BRANCH_TARGET.  If TARGET_IS_THUMB is set,
  // the branch target is a thumb instruction.  TARGET is used for look
  // up ARM-specific linker settings.
  static Stub_type
  stub_type_for_reloc(unsigned int r_type, Arm_address branch_address,
		      Arm_address branch_target, bool target_is_thumb);

  // Reloc_stub key.  A key is logically a triplet of a stub type, a symbol
  // and an addend.  Since we treat global and local symbol differently, we
  // use a Symbol object for a global symbol and a object-index pair for
  // a local symbol.
  class Key
  {
   public:
    // If SYMBOL is not null, this is a global symbol, we ignore RELOBJ and
    // R_SYM.  Otherwise, this is a local symbol and RELOBJ must non-NULL
    // and R_SYM must not be invalid_index.
    Key(Stub_type stub_type, const Symbol* symbol, const Relobj* relobj,
	unsigned int r_sym, int32_t addend)
      : stub_type_(stub_type), addend_(addend)
    {
      if (symbol != NULL)
	{
	  this->r_sym_ = Reloc_stub::invalid_index;
	  this->u_.symbol = symbol;
	}
      else
	{
	  gold_assert(relobj != NULL && r_sym != invalid_index);
	  this->r_sym_ = r_sym;
	  this->u_.relobj = relobj;
	}
    }

    ~Key()
    { }

    // Accessors: Keys are meant to be read-only object so no modifiers are
    // provided.

    // Return stub type.
    Stub_type
    stub_type() const
    { return this->stub_type_; }

    // Return the local symbol index or invalid_index.
    unsigned int
    r_sym() const
    { return this->r_sym_; }

    // Return the symbol if there is one.
    const Symbol*
    symbol() const
    { return this->r_sym_ == invalid_index ? this->u_.symbol : NULL; }

    // Return the relobj if there is one.
    const Relobj*
    relobj() const
    { return this->r_sym_ != invalid_index ? this->u_.relobj : NULL; }

    // Whether this equals to another key k.
    bool
    eq(const Key& k) const 
    {
      return ((this->stub_type_ == k.stub_type_)
	      && (this->r_sym_ == k.r_sym_)
	      && ((this->r_sym_ != Reloc_stub::invalid_index)
		  ? (this->u_.relobj == k.u_.relobj)
		  : (this->u_.symbol == k.u_.symbol))
	      && (this->addend_ == k.addend_));
    }

    // Return a hash value.
    size_t
    hash_value() const
    {
      return (this->stub_type_
	      ^ this->r_sym_
	      ^ gold::string_hash<char>(
		    (this->r_sym_ != Reloc_stub::invalid_index)
		    ? this->u_.relobj->name().c_str()
		    : this->u_.symbol->name())
	      ^ this->addend_);
    }

    // Functors for STL associative containers.
    struct hash
    {
      size_t
      operator()(const Key& k) const
      { return k.hash_value(); }
    };

    struct equal_to
    {
      bool
      operator()(const Key& k1, const Key& k2) const
      { return k1.eq(k2); }
    };

    // Name of key.  This is mainly for debugging.
    std::string
    name() const;

   private:
    // Stub type.
    Stub_type stub_type_;
    // If this is a local symbol, this is the index in the defining object.
    // Otherwise, it is invalid_index for a global symbol.
    unsigned int r_sym_;
    // If r_sym_ is invalid index.  This points to a global symbol.
    // Otherwise, this points a relobj.  We used the unsized and target
    // independent Symbol and Relobj classes instead of Arm_symbol and  
    // Arm_relobj.  This is done to avoid making the stub class a template
    // as most of the stub machinery is endianity-neutral.  However, it
    // may require a bit of casting done by users of this class.
    union
    {
      const Symbol* symbol;
      const Relobj* relobj;
    } u_;
    // Addend associated with a reloc.
    int32_t addend_;
  };

 protected:
  // Reloc_stubs are created via a stub factory.  So these are protected.
  Reloc_stub(const Stub_template* stub_template)
    : Stub(stub_template), destination_address_(invalid_address)
  { }

  ~Reloc_stub()
  { }

  friend class Stub_factory;

 private:
  // Return the relocation target address of the i-th relocation in the
  // stub.
  Arm_address
  do_reloc_target(size_t i)
  {
    // All reloc stub have only one relocation.
    gold_assert(i == 0);
    return this->destination_address_;
  }

  // A template to implement do_write below.
  template<bool big_endian>
  void inline
  do_fixed_endian_write(unsigned char*, section_size_type);

  // Write a stub.
  void
  do_write(unsigned char* view, section_size_type view_size, bool big_endian);

  // Address of destination.
  Arm_address destination_address_;
};

// Stub factory class.

class Stub_factory
{
 public:
  // Return the unique instance of this class.
  static const Stub_factory&
  get_instance()
  {
    static Stub_factory singleton;
    return singleton;
  }

  // Make a relocation stub.
  Reloc_stub*
  make_reloc_stub(Stub_type stub_type) const
  {
    gold_assert(stub_type >= arm_stub_reloc_first
		&& stub_type <= arm_stub_reloc_last);
    return new Reloc_stub(this->stub_templates_[stub_type]);
  }

 private:
  // Constructor and destructor are protected since we only return a single
  // instance created in Stub_factory::get_instance().
  
  Stub_factory();

  // A Stub_factory may not be copied since it is a singleton.
  Stub_factory(const Stub_factory&);
  Stub_factory& operator=(Stub_factory&);
  
  // Stub templates.  These are initialized in the constructor.
  const Stub_template* stub_templates_[arm_stub_type_last+1];
};

// Utilities for manipulating integers of up to 32-bits

namespace utils
{
  // Sign extend an n-bit unsigned integer stored in an uint32_t into
  // an int32_t.  NO_BITS must be between 1 to 32.
  template<int no_bits>
  static inline int32_t
  sign_extend(uint32_t bits)
  {
    gold_assert(no_bits >= 0 && no_bits <= 32);
    if (no_bits == 32)
      return static_cast<int32_t>(bits);
    uint32_t mask = (~((uint32_t) 0)) >> (32 - no_bits);
    bits &= mask;
    uint32_t top_bit = 1U << (no_bits - 1);
    int32_t as_signed = static_cast<int32_t>(bits);
    return (bits & top_bit) ? as_signed + (-top_bit * 2) : as_signed;
  }

  // Detects overflow of an NO_BITS integer stored in a uint32_t.
  template<int no_bits>
  static inline bool
  has_overflow(uint32_t bits)
  {
    gold_assert(no_bits >= 0 && no_bits <= 32);
    if (no_bits == 32)
      return false;
    int32_t max = (1 << (no_bits - 1)) - 1;
    int32_t min = -(1 << (no_bits - 1));
    int32_t as_signed = static_cast<int32_t>(bits);
    return as_signed > max || as_signed < min;
  }

  // Detects overflow of an NO_BITS integer stored in a uint32_t when it
  // fits in the given number of bits as either a signed or unsigned value.
  // For example, has_signed_unsigned_overflow<8> would check
  // -128 <= bits <= 255
  template<int no_bits>
  static inline bool
  has_signed_unsigned_overflow(uint32_t bits)
  {
    gold_assert(no_bits >= 2 && no_bits <= 32);
    if (no_bits == 32)
      return false;
    int32_t max = static_cast<int32_t>((1U << no_bits) - 1);
    int32_t min = -(1 << (no_bits - 1));
    int32_t as_signed = static_cast<int32_t>(bits);
    return as_signed > max || as_signed < min;
  }

  // Select bits from A and B using bits in MASK.  For each n in [0..31],
  // the n-th bit in the result is chosen from the n-th bits of A and B.
  // A zero selects A and a one selects B.
  static inline uint32_t
  bit_select(uint32_t a, uint32_t b, uint32_t mask)
  { return (a & ~mask) | (b & mask); }
};

template<bool big_endian>
class Target_arm : public Sized_target<32, big_endian>
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_REL, true, 32, big_endian>
    Reloc_section;

  Target_arm()
    : Sized_target<32, big_endian>(&arm_info),
      got_(NULL), plt_(NULL), got_plt_(NULL), rel_dyn_(NULL),
      copy_relocs_(elfcpp::R_ARM_COPY), dynbss_(NULL),
      may_use_blx_(true), should_force_pic_veneer_(false)
  { }

  // Whether we can use BLX.
  bool
  may_use_blx() const
  { return this->may_use_blx_; }

  // Set use-BLX flag.
  void
  set_may_use_blx(bool value)
  { this->may_use_blx_ = value; }
  
  // Whether we force PCI branch veneers.
  bool
  should_force_pic_veneer() const
  { return this->should_force_pic_veneer_; }

  // Set PIC veneer flag.
  void
  set_should_force_pic_veneer(bool value)
  { this->should_force_pic_veneer_ = value; }
  
  // Whether we use THUMB-2 instructions.
  bool
  using_thumb2() const
  {
    // FIXME:  This should not hard-coded.
    return false;
  }

  // Whether we use THUMB/THUMB-2 instructions only.
  bool
  using_thumb_only() const
  {
    // FIXME:  This should not hard-coded.
    return false;
  }

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
		   section_size_type view_size,
		   const Reloc_symbol_changes*);

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

  // Get the default ARM target.
  static const Target_arm<big_endian>&
  default_target()
  {
    gold_assert(parameters->target().machine_code() == elfcpp::EM_ARM
		&& parameters->target().is_big_endian() == big_endian);
    return static_cast<const Target_arm<big_endian>&>(parameters->target());
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

    void
    check_non_pic(Relobj*, unsigned int r_type);

    // Almost identical to Symbol::needs_plt_entry except that it also
    // handles STT_ARM_TFUNC.
    static bool
    symbol_needs_plt_entry(const Symbol* sym)
    {
      // An undefined symbol from an executable does not need a PLT entry.
      if (sym->is_undefined() && !parameters->options().shared())
	return false;

      return (!parameters->doing_static_link()
	      && (sym->type() == elfcpp::STT_FUNC
		  || sym->type() == elfcpp::STT_ARM_TFUNC)
	      && (sym->is_from_dynobj()
		  || sym->is_undefined()
		  || sym->is_preemptible()));
    }

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

    // Return whether the static relocation needs to be applied.
    inline bool
    should_apply_static_reloc(const Sized_symbol<32>* gsym,
			      int ref_flags,
			      bool is_32bit,
			      Output_section* output_section);

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

    // Return whether we want to pass flag NON_PIC_REF for this
    // reloc.
    static inline bool
    reloc_is_non_pic (unsigned int r_type)
    {
      switch (r_type)
	{
	case elfcpp::R_ARM_REL32:
	case elfcpp::R_ARM_THM_CALL:
	case elfcpp::R_ARM_CALL:
	case elfcpp::R_ARM_JUMP24:
	case elfcpp::R_ARM_PREL31:
	case elfcpp::R_ARM_THM_ABS5:
	case elfcpp::R_ARM_ABS8:
	case elfcpp::R_ARM_ABS12:
	case elfcpp::R_ARM_ABS16:
	case elfcpp::R_ARM_BASE_ABS:
	  return true;
	default:
	  return false;
	}
    }
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
    return (gsym->type() != elfcpp::STT_ARM_TFUNC
	    && gsym->may_need_copy_reloc());
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
  // Whether we can use BLX.
  bool may_use_blx_;
  // Whether we force PIC branch veneers.
  bool should_force_pic_veneer_;
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
  0x1000,		// common_pagesize (overridable by -z common-page-size)
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0			// large_common_section_flags
};

// Arm relocate functions class
//

template<bool big_endian>
class Arm_relocate_functions : public Relocate_functions<32, big_endian>
{
 public:
  typedef enum
  {
    STATUS_OKAY,	// No error during relocation.
    STATUS_OVERFLOW,	// Relocation oveflow.
    STATUS_BAD_RELOC	// Relocation cannot be applied.
  } Status;

 private:
  typedef Relocate_functions<32, big_endian> Base;
  typedef Arm_relocate_functions<big_endian> This;

  // Get an symbol value of *PSYMVAL with an ADDEND.  This is a wrapper
  // to Symbol_value::value().  If HAS_THUMB_BIT is true, that LSB is used
  // to distinguish ARM and THUMB functions and it is treated specially.
  static inline Symbol_value<32>::Value
  arm_symbol_value (const Sized_relobj<32, big_endian> *object,
		    const Symbol_value<32>* psymval,
		    Symbol_value<32>::Value addend,
		    bool has_thumb_bit)
  {
    typedef Symbol_value<32>::Value Valtype;

    if (has_thumb_bit)
      {
	Valtype raw = psymval->value(object, 0);
	Valtype thumb_bit = raw & 1;
	return ((raw & ~((Valtype) 1)) + addend) | thumb_bit;
      }
    else
      return psymval->value(object, addend);
  }

  // Encoding of imm16 argument for movt and movw ARM instructions
  // from ARM ARM:
  //     
  //     imm16 := imm4 | imm12
  //
  //  f e d c b a 9 8 7 6 5 4 3 2 1 0 f e d c b a 9 8 7 6 5 4 3 2 1 0 
  // +-------+---------------+-------+-------+-----------------------+
  // |       |               |imm4   |       |imm12                  |
  // +-------+---------------+-------+-------+-----------------------+

  // Extract the relocation addend from VAL based on the ARM
  // instruction encoding described above.
  static inline typename elfcpp::Swap<32, big_endian>::Valtype
  extract_arm_movw_movt_addend(
      typename elfcpp::Swap<32, big_endian>::Valtype val)
  {
    // According to the Elf ABI for ARM Architecture the immediate
    // field is sign-extended to form the addend.
    return utils::sign_extend<16>(((val >> 4) & 0xf000) | (val & 0xfff));
  }

  // Insert X into VAL based on the ARM instruction encoding described
  // above.
  static inline typename elfcpp::Swap<32, big_endian>::Valtype
  insert_val_arm_movw_movt(
      typename elfcpp::Swap<32, big_endian>::Valtype val,
      typename elfcpp::Swap<32, big_endian>::Valtype x)
  {
    val &= 0xfff0f000;
    val |= x & 0x0fff;
    val |= (x & 0xf000) << 4;
    return val;
  }

  // Encoding of imm16 argument for movt and movw Thumb2 instructions
  // from ARM ARM:
  //     
  //     imm16 := imm4 | i | imm3 | imm8
  //
  //  f e d c b a 9 8 7 6 5 4 3 2 1 0  f e d c b a 9 8 7 6 5 4 3 2 1 0 
  // +---------+-+-----------+-------++-+-----+-------+---------------+
  // |         |i|           |imm4   || |imm3 |       |imm8           |
  // +---------+-+-----------+-------++-+-----+-------+---------------+

  // Extract the relocation addend from VAL based on the Thumb2
  // instruction encoding described above.
  static inline typename elfcpp::Swap<32, big_endian>::Valtype
  extract_thumb_movw_movt_addend(
      typename elfcpp::Swap<32, big_endian>::Valtype val)
  {
    // According to the Elf ABI for ARM Architecture the immediate
    // field is sign-extended to form the addend.
    return utils::sign_extend<16>(((val >> 4) & 0xf000)
				  | ((val >> 15) & 0x0800)
				  | ((val >> 4) & 0x0700)
				  | (val & 0x00ff));
  }

  // Insert X into VAL based on the Thumb2 instruction encoding
  // described above.
  static inline typename elfcpp::Swap<32, big_endian>::Valtype
  insert_val_thumb_movw_movt(
      typename elfcpp::Swap<32, big_endian>::Valtype val,
      typename elfcpp::Swap<32, big_endian>::Valtype x)
  {
    val &= 0xfbf08f00;
    val |= (x & 0xf000) << 4;
    val |= (x & 0x0800) << 15;
    val |= (x & 0x0700) << 4;
    val |= (x & 0x00ff);
    return val;
  }

  // FIXME: This probably only works for Android on ARM v5te. We should
  // following GNU ld for the general case.
  template<unsigned r_type>
  static inline typename This::Status
  arm_branch_common(unsigned char *view,
		    const Sized_relobj<32, big_endian>* object,
		    const Symbol_value<32>* psymval,
		    elfcpp::Elf_types<32>::Elf_Addr address,
		    bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
     
    bool insn_is_b = (((val >> 28) & 0xf) <= 0xe)
		      && ((val & 0x0f000000UL) == 0x0a000000UL);
    bool insn_is_uncond_bl = (val & 0xff000000UL) == 0xeb000000UL;
    bool insn_is_cond_bl = (((val >> 28) & 0xf) < 0xe)
			    && ((val & 0x0f000000UL) == 0x0b000000UL);
    bool insn_is_blx = (val & 0xfe000000UL) == 0xfa000000UL;
    bool insn_is_any_branch = (val & 0x0e000000UL) == 0x0a000000UL;

    if (r_type == elfcpp::R_ARM_CALL)
      {
	if (!insn_is_uncond_bl && !insn_is_blx)
	  return This::STATUS_BAD_RELOC;
      }
    else if (r_type == elfcpp::R_ARM_JUMP24)
      {
	if (!insn_is_b && !insn_is_cond_bl)
	  return This::STATUS_BAD_RELOC;
      }
    else if (r_type == elfcpp::R_ARM_PLT32)
      {
	if (!insn_is_any_branch)
	  return This::STATUS_BAD_RELOC;
      }
    else
      gold_unreachable();

    Valtype addend = utils::sign_extend<26>(val << 2);
    Valtype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit)
		 - address);

    // If target has thumb bit set, we need to either turn the BL
    // into a BLX (for ARMv5 or above) or generate a stub.
    if (x & 1)
      {
	// Turn BL to BLX.
	if (insn_is_uncond_bl)
	  val = (val & 0xffffff) | 0xfa000000 | ((x & 2) << 23);
	else
	  return This::STATUS_BAD_RELOC;
      }
    else
      gold_assert(!insn_is_blx);

    val = utils::bit_select(val, (x >> 2), 0xffffffUL);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return (utils::has_overflow<26>(x)
	    ? This::STATUS_OVERFLOW : This::STATUS_OKAY);
  }

 public:

  // R_ARM_ABS8: S + A
  static inline typename This::Status
  abs8(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<8, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<8, big_endian>::readval(wv);
    Reltype addend = utils::sign_extend<8>(val);
    Reltype x = This::arm_symbol_value(object, psymval, addend, false);
    val = utils::bit_select(val, x, 0xffU);
    elfcpp::Swap<8, big_endian>::writeval(wv, val);
    return (utils::has_signed_unsigned_overflow<8>(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // R_ARM_THM_ABS5: S + A
  static inline typename This::Status
  thm_abs5(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<16, big_endian>::readval(wv);
    Reltype addend = (val & 0x7e0U) >> 6;
    Reltype x = This::arm_symbol_value(object, psymval, addend, false);
    val = utils::bit_select(val, x << 6, 0x7e0U);
    elfcpp::Swap<16, big_endian>::writeval(wv, val);
    return (utils::has_overflow<5>(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // R_ARM_ABS12: S + A
  static inline typename This::Status
  abs12(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Reltype addend = val & 0x0fffU;
    Reltype x = This::arm_symbol_value(object, psymval, addend, false);
    val = utils::bit_select(val, x, 0x0fffU);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return (utils::has_overflow<12>(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // R_ARM_ABS16: S + A
  static inline typename This::Status
  abs16(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<16, big_endian>::readval(wv);
    Reltype addend = utils::sign_extend<16>(val);
    Reltype x = This::arm_symbol_value(object, psymval, addend, false);
    val = utils::bit_select(val, x, 0xffffU);
    elfcpp::Swap<16, big_endian>::writeval(wv, val);
    return (utils::has_signed_unsigned_overflow<16>(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // R_ARM_ABS32: (S + A) | T
  static inline typename This::Status
  abs32(unsigned char *view,
	const Sized_relobj<32, big_endian>* object,
	const Symbol_value<32>* psymval,
	bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype addend = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype x = This::arm_symbol_value(object, psymval, addend, has_thumb_bit);
    elfcpp::Swap<32, big_endian>::writeval(wv, x);
    return This::STATUS_OKAY;
  }

  // R_ARM_REL32: (S + A) | T - P
  static inline typename This::Status
  rel32(unsigned char *view,
	const Sized_relobj<32, big_endian>* object,
	const Symbol_value<32>* psymval,
	elfcpp::Elf_types<32>::Elf_Addr address,
	bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype addend = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit) 
		 - address);
    elfcpp::Swap<32, big_endian>::writeval(wv, x);
    return This::STATUS_OKAY;
  }

  // R_ARM_THM_CALL: (S + A) | T - P
  static inline typename This::Status
  thm_call(unsigned char *view,
	   const Sized_relobj<32, big_endian>* object,
	   const Symbol_value<32>* psymval,
	   elfcpp::Elf_types<32>::Elf_Addr address,
	   bool has_thumb_bit)
  {
    // A thumb call consists of two instructions.
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype hi = elfcpp::Swap<16, big_endian>::readval(wv);
    Valtype lo = elfcpp::Swap<16, big_endian>::readval(wv + 1);
    // Must be a BL instruction. lo == 11111xxxxxxxxxxx.
    gold_assert((lo & 0xf800) == 0xf800);
    Reltype addend = utils::sign_extend<23>(((hi & 0x7ff) << 12)
					   | ((lo & 0x7ff) << 1));
    Reltype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit)
		 - address);

    // If target has no thumb bit set, we need to either turn the BL
    // into a BLX (for ARMv5 or above) or generate a stub.
    if ((x & 1) == 0)
      {
	// This only works for ARMv5 and above with interworking enabled.
	lo &= 0xefff;
      }
    hi = utils::bit_select(hi, (x >> 12), 0x7ffU);
    lo = utils::bit_select(lo, (x >> 1), 0x7ffU);
    elfcpp::Swap<16, big_endian>::writeval(wv, hi);
    elfcpp::Swap<16, big_endian>::writeval(wv + 1, lo);
    return (utils::has_overflow<23>(x)
	    ? This::STATUS_OVERFLOW
	    : This::STATUS_OKAY);
  }

  // R_ARM_BASE_PREL: B(S) + A - P
  static inline typename This::Status
  base_prel(unsigned char* view,
	    elfcpp::Elf_types<32>::Elf_Addr origin,
	    elfcpp::Elf_types<32>::Elf_Addr address)
  {
    Base::rel32(view, origin - address);
    return STATUS_OKAY;
  }

  // R_ARM_BASE_ABS: B(S) + A
  static inline typename This::Status
  base_abs(unsigned char* view,
	    elfcpp::Elf_types<32>::Elf_Addr origin)
  {
    Base::rel32(view, origin);
    return STATUS_OKAY;
  }

  // R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
  static inline typename This::Status
  got_brel(unsigned char* view,
	   typename elfcpp::Swap<32, big_endian>::Valtype got_offset)
  {
    Base::rel32(view, got_offset);
    return This::STATUS_OKAY;
  }

  // R_ARM_GOT_PREL: GOT(S) + A – P
  static inline typename This::Status
  got_prel(unsigned char* view,
	   typename elfcpp::Swap<32, big_endian>::Valtype got_offset,
	   elfcpp::Elf_types<32>::Elf_Addr address)
  {
    Base::rel32(view, got_offset - address);
    return This::STATUS_OKAY;
  }

  // R_ARM_PLT32: (S + A) | T - P
  static inline typename This::Status
  plt32(unsigned char *view,
	const Sized_relobj<32, big_endian>* object,
	const Symbol_value<32>* psymval,
	elfcpp::Elf_types<32>::Elf_Addr address,
	bool has_thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_PLT32>(view, object, psymval,
						  address, has_thumb_bit);
  }

  // R_ARM_CALL: (S + A) | T - P
  static inline typename This::Status
  call(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval,
       elfcpp::Elf_types<32>::Elf_Addr address,
       bool has_thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_CALL>(view, object, psymval,
						 address, has_thumb_bit);
  }

  // R_ARM_JUMP24: (S + A) | T - P
  static inline typename This::Status
  jump24(unsigned char *view,
	 const Sized_relobj<32, big_endian>* object,
	 const Symbol_value<32>* psymval,
	 elfcpp::Elf_types<32>::Elf_Addr address,
	 bool has_thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_JUMP24>(view, object, psymval,
						   address, has_thumb_bit);
  }

  // R_ARM_PREL: (S + A) | T - P
  static inline typename This::Status
  prel31(unsigned char *view,
	 const Sized_relobj<32, big_endian>* object,
	 const Symbol_value<32>* psymval,
	 elfcpp::Elf_types<32>::Elf_Addr address,
	 bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = utils::sign_extend<31>(val);
    Valtype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit)
		 - address);
    val = utils::bit_select(val, x, 0x7fffffffU);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return (utils::has_overflow<31>(x) ?
	    This::STATUS_OVERFLOW : This::STATUS_OKAY);
  }

  // R_ARM_MOVW_ABS_NC: (S + A) | T
  static inline typename This::Status 
  movw_abs_nc(unsigned char *view,
	      const Sized_relobj<32, big_endian>* object,
	      const Symbol_value<32>* psymval,
	      bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend =  This::extract_arm_movw_movt_addend(val);
    Valtype x = This::arm_symbol_value(object, psymval, addend, has_thumb_bit);
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  // R_ARM_MOVT_ABS: S + A
  static inline typename This::Status
  movt_abs(unsigned char *view,
	   const Sized_relobj<32, big_endian>* object,
           const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = This::extract_arm_movw_movt_addend(val);
    Valtype x = This::arm_symbol_value(object, psymval, addend, 0) >> 16;
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  //  R_ARM_THM_MOVW_ABS_NC: S + A | T
  static inline typename This::Status 
  thm_movw_abs_nc(unsigned char *view,
	          const Sized_relobj<32, big_endian>* object,
	          const Symbol_value<32>* psymval,
	          bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = ((elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		   | elfcpp::Swap<16, big_endian>::readval(wv + 1));
    Reltype addend = extract_thumb_movw_movt_addend(val);
    Reltype x = This::arm_symbol_value(object, psymval, addend, has_thumb_bit);
    val = This::insert_val_thumb_movw_movt(val, x);
    elfcpp::Swap<16, big_endian>::writeval(wv, val >> 16);
    elfcpp::Swap<16, big_endian>::writeval(wv + 1, val & 0xffff);
    return This::STATUS_OKAY;
  }

  //  R_ARM_THM_MOVT_ABS: S + A
  static inline typename This::Status 
  thm_movt_abs(unsigned char *view,
	       const Sized_relobj<32, big_endian>* object,
	       const Symbol_value<32>* psymval)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = ((elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		   | elfcpp::Swap<16, big_endian>::readval(wv + 1));
    Reltype addend = This::extract_thumb_movw_movt_addend(val);
    Reltype x = This::arm_symbol_value(object, psymval, addend, 0) >> 16;
    val = This::insert_val_thumb_movw_movt(val, x);
    elfcpp::Swap<16, big_endian>::writeval(wv, val >> 16);
    elfcpp::Swap<16, big_endian>::writeval(wv + 1, val & 0xffff);
    return This::STATUS_OKAY;
  }

  // R_ARM_MOVW_PREL_NC: (S + A) | T - P
  static inline typename This::Status
  movw_prel_nc(unsigned char *view,
	       const Sized_relobj<32, big_endian>* object,
	       const Symbol_value<32>* psymval,
	       elfcpp::Elf_types<32>::Elf_Addr address,
	       bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = This::extract_arm_movw_movt_addend(val);
    Valtype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit)
                 - address);
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  // R_ARM_MOVT_PREL: S + A - P
  static inline typename This::Status
  movt_prel(unsigned char *view,
	    const Sized_relobj<32, big_endian>* object,
	    const Symbol_value<32>* psymval,
            elfcpp::Elf_types<32>::Elf_Addr address)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = This::extract_arm_movw_movt_addend(val);
    Valtype x = (This::arm_symbol_value(object, psymval, addend, 0)
                 - address) >> 16;
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  // R_ARM_THM_MOVW_PREL_NC: (S + A) | T - P
  static inline typename This::Status
  thm_movw_prel_nc(unsigned char *view,
	           const Sized_relobj<32, big_endian>* object,
	           const Symbol_value<32>* psymval,
	           elfcpp::Elf_types<32>::Elf_Addr address,
	           bool has_thumb_bit)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = (elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		  | elfcpp::Swap<16, big_endian>::readval(wv + 1);
    Reltype addend = This::extract_thumb_movw_movt_addend(val);
    Reltype x = (This::arm_symbol_value(object, psymval, addend, has_thumb_bit)
                 - address);
    val = This::insert_val_thumb_movw_movt(val, x);
    elfcpp::Swap<16, big_endian>::writeval(wv, val >> 16);
    elfcpp::Swap<16, big_endian>::writeval(wv + 1, val & 0xffff);
    return This::STATUS_OKAY;
  }

  // R_ARM_THM_MOVT_PREL: S + A - P
  static inline typename This::Status
  thm_movt_prel(unsigned char *view,
	        const Sized_relobj<32, big_endian>* object,
	        const Symbol_value<32>* psymval,
	        elfcpp::Elf_types<32>::Elf_Addr address)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = (elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		  | elfcpp::Swap<16, big_endian>::readval(wv + 1);
    Reltype addend = This::extract_thumb_movw_movt_addend(val);
    Reltype x = (This::arm_symbol_value(object, psymval, addend, 0)
                 - address) >> 16;
    val = This::insert_val_thumb_movw_movt(val, x);
    elfcpp::Swap<16, big_endian>::writeval(wv, val >> 16);
    elfcpp::Swap<16, big_endian>::writeval(wv + 1, val & 0xffff);
    return This::STATUS_OKAY;
  }
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

// Insn_template methods.

// Return byte size of an instruction template.

size_t
Insn_template::size() const
{
  switch (this->type())
    {
    case THUMB16_TYPE:
      return 2;
    case ARM_TYPE:
    case THUMB32_TYPE:
    case DATA_TYPE:
      return 4;
    default:
      gold_unreachable();
    }
}

// Return alignment of an instruction template.

unsigned
Insn_template::alignment() const
{
  switch (this->type())
    {
    case THUMB16_TYPE:
    case THUMB32_TYPE:
      return 2;
    case ARM_TYPE:
    case DATA_TYPE:
      return 4;
    default:
      gold_unreachable();
    }
}

// Stub_template methods.

Stub_template::Stub_template(
    Stub_type type, const Insn_template* insns,
     size_t insn_count)
  : type_(type), insns_(insns), insn_count_(insn_count), alignment_(1),
    entry_in_thumb_mode_(false), relocs_()
{
  off_t offset = 0;

  // Compute byte size and alignment of stub template.
  for (size_t i = 0; i < insn_count; i++)
    {
      unsigned insn_alignment = insns[i].alignment();
      size_t insn_size = insns[i].size();
      gold_assert((offset & (insn_alignment - 1)) == 0);
      this->alignment_ = std::max(this->alignment_, insn_alignment);
      switch (insns[i].type())
	{
	case Insn_template::THUMB16_TYPE:
	  if (i == 0)
	    this->entry_in_thumb_mode_ = true;
	  break;

	case Insn_template::THUMB32_TYPE:
          if (insns[i].r_type() != elfcpp::R_ARM_NONE)
	    this->relocs_.push_back(Reloc(i, offset));
	  if (i == 0)
	    this->entry_in_thumb_mode_ = true;
          break;

	case Insn_template::ARM_TYPE:
	  // Handle cases where the target is encoded within the
	  // instruction.
	  if (insns[i].r_type() == elfcpp::R_ARM_JUMP24)
	    this->relocs_.push_back(Reloc(i, offset));
	  break;

	case Insn_template::DATA_TYPE:
	  // Entry point cannot be data.
	  gold_assert(i != 0);
	  this->relocs_.push_back(Reloc(i, offset));
	  break;

	default:
	  gold_unreachable();
	}
      offset += insn_size; 
    }
  this->size_ = offset;
}

// Reloc_stub::Key methods.

// Dump a Key as a string for debugging.

std::string
Reloc_stub::Key::name() const
{
  if (this->r_sym_ == invalid_index)
    {
      // Global symbol key name
      // <stub-type>:<symbol name>:<addend>.
      const std::string sym_name = this->u_.symbol->name();
      // We need to print two hex number and two colons.  So just add 100 bytes
      // to the symbol name size.
      size_t len = sym_name.size() + 100;
      char* buffer = new char[len];
      int c = snprintf(buffer, len, "%d:%s:%x", this->stub_type_,
		       sym_name.c_str(), this->addend_);
      gold_assert(c > 0 && c < static_cast<int>(len));
      delete[] buffer;
      return std::string(buffer);
    }
  else
    {
      // local symbol key name
      // <stub-type>:<object>:<r_sym>:<addend>.
      const size_t len = 200;
      char buffer[len];
      int c = snprintf(buffer, len, "%d:%p:%u:%x", this->stub_type_,
		       this->u_.relobj, this->r_sym_, this->addend_);
      gold_assert(c > 0 && c < static_cast<int>(len));
      return std::string(buffer);
    }
}

// Reloc_stub methods.

// Determine the type of stub needed, if any, for a relocation of R_TYPE at
// LOCATION to DESTINATION.
// This code is based on the arm_type_of_stub function in
// bfd/elf32-arm.c.  We have changed the interface a liitle to keep the Stub
// class simple.

Stub_type
Reloc_stub::stub_type_for_reloc(
   unsigned int r_type,
   Arm_address location,
   Arm_address destination,
   bool target_is_thumb)
{
  Stub_type stub_type = arm_stub_none;

  // This is a bit ugly but we want to avoid using a templated class for
  // big and little endianities.
  bool may_use_blx;
  bool should_force_pic_veneer;
  bool thumb2;
  bool thumb_only;
  if (parameters->target().is_big_endian())
    {
      const Target_arm<true>& big_endian_target =
	Target_arm<true>::default_target();
      may_use_blx = big_endian_target.may_use_blx();
      should_force_pic_veneer = big_endian_target.should_force_pic_veneer();
      thumb2 = big_endian_target.using_thumb2();
      thumb_only = big_endian_target.using_thumb_only();
    }
  else
    {
      const Target_arm<false>& little_endian_target =
	Target_arm<false>::default_target();
      may_use_blx = little_endian_target.may_use_blx();
      should_force_pic_veneer = little_endian_target.should_force_pic_veneer();
      thumb2 = little_endian_target.using_thumb2();
      thumb_only = little_endian_target.using_thumb_only();
    }

  int64_t branch_offset = (int64_t)destination - location;

  if (r_type == elfcpp::R_ARM_THM_CALL || r_type == elfcpp::R_ARM_THM_JUMP24)
    {
      // Handle cases where:
      // - this call goes too far (different Thumb/Thumb2 max
      //   distance)
      // - it's a Thumb->Arm call and blx is not available, or it's a
      //   Thumb->Arm branch (not bl). A stub is needed in this case.
      if ((!thumb2
	    && (branch_offset > THM_MAX_FWD_BRANCH_OFFSET
		|| (branch_offset < THM_MAX_BWD_BRANCH_OFFSET)))
	  || (thumb2
	      && (branch_offset > THM2_MAX_FWD_BRANCH_OFFSET
		  || (branch_offset < THM2_MAX_BWD_BRANCH_OFFSET)))
	  || ((!target_is_thumb)
	      && (((r_type == elfcpp::R_ARM_THM_CALL) && !may_use_blx)
		  || (r_type == elfcpp::R_ARM_THM_JUMP24))))
	{
	  if (target_is_thumb)
	    {
	      // Thumb to thumb.
	      if (!thumb_only)
		{
		  stub_type = (parameters->options().shared() | should_force_pic_veneer)
		    // PIC stubs.
		    ? ((may_use_blx
			&& (r_type == elfcpp::R_ARM_THM_CALL))
		       // V5T and above. Stub starts with ARM code, so
		       // we must be able to switch mode before
		       // reaching it, which is only possible for 'bl'
		       // (ie R_ARM_THM_CALL relocation).
		       ? arm_stub_long_branch_any_thumb_pic
		       // On V4T, use Thumb code only.
		       : arm_stub_long_branch_v4t_thumb_thumb_pic)

		    // non-PIC stubs.
		    : ((may_use_blx
			&& (r_type == elfcpp::R_ARM_THM_CALL))
		       ? arm_stub_long_branch_any_any // V5T and above.
		       : arm_stub_long_branch_v4t_thumb_thumb);	// V4T.
		}
	      else
		{
		  stub_type = (parameters->options().shared() | should_force_pic_veneer)
		    ? arm_stub_long_branch_thumb_only_pic	// PIC stub.
		    : arm_stub_long_branch_thumb_only;	// non-PIC stub.
		}
	    }
	  else
	    {
	      // Thumb to arm.
	     
	      // FIXME: We should check that the input section is from an
	      // object that has interwork enabled.

	      stub_type = (parameters->options().shared()
			   || should_force_pic_veneer)
		// PIC stubs.
		? ((may_use_blx
		    && (r_type == elfcpp::R_ARM_THM_CALL))
		   ? arm_stub_long_branch_any_arm_pic	// V5T and above.
		   : arm_stub_long_branch_v4t_thumb_arm_pic)	// V4T.

		// non-PIC stubs.
		: ((may_use_blx
		    && (r_type == elfcpp::R_ARM_THM_CALL))
		   ? arm_stub_long_branch_any_any	// V5T and above.
		   : arm_stub_long_branch_v4t_thumb_arm);	// V4T.

	      // Handle v4t short branches.
	      if ((stub_type == arm_stub_long_branch_v4t_thumb_arm)
		  && (branch_offset <= THM_MAX_FWD_BRANCH_OFFSET)
		  && (branch_offset >= THM_MAX_BWD_BRANCH_OFFSET))
		stub_type = arm_stub_short_branch_v4t_thumb_arm;
	    }
	}
    }
  else if (r_type == elfcpp::R_ARM_CALL
	   || r_type == elfcpp::R_ARM_JUMP24
	   || r_type == elfcpp::R_ARM_PLT32)
    {
      if (target_is_thumb)
	{
	  // Arm to thumb.

	  // FIXME: We should check that the input section is from an
	  // object that has interwork enabled.

	  // We have an extra 2-bytes reach because of
	  // the mode change (bit 24 (H) of BLX encoding).
	  if (branch_offset > (ARM_MAX_FWD_BRANCH_OFFSET + 2)
	      || (branch_offset < ARM_MAX_BWD_BRANCH_OFFSET)
	      || ((r_type == elfcpp::R_ARM_CALL) && !may_use_blx)
	      || (r_type == elfcpp::R_ARM_JUMP24)
	      || (r_type == elfcpp::R_ARM_PLT32))
	    {
	      stub_type = (parameters->options().shared()
			   || should_force_pic_veneer)
		// PIC stubs.
		? (may_use_blx
		   ? arm_stub_long_branch_any_thumb_pic// V5T and above.
		   : arm_stub_long_branch_v4t_arm_thumb_pic)	// V4T stub.

		// non-PIC stubs.
		: (may_use_blx
		   ? arm_stub_long_branch_any_any	// V5T and above.
		   : arm_stub_long_branch_v4t_arm_thumb);	// V4T.
	    }
	}
      else
	{
	  // Arm to arm.
	  if (branch_offset > ARM_MAX_FWD_BRANCH_OFFSET
	      || (branch_offset < ARM_MAX_BWD_BRANCH_OFFSET))
	    {
	      stub_type = (parameters->options().shared()
			   || should_force_pic_veneer)
		? arm_stub_long_branch_any_arm_pic	// PIC stubs.
		: arm_stub_long_branch_any_any;		/// non-PIC.
	    }
	}
    }

  return stub_type;
}

// Template to implement do_write for a specific target endianity.

template<bool big_endian>
void inline
Reloc_stub::do_fixed_endian_write(unsigned char* view,
				  section_size_type view_size)
{
  const Stub_template* stub_template = this->stub_template();
  const Insn_template* insns = stub_template->insns();

  // FIXME:  We do not handle BE8 encoding yet.
  unsigned char* pov = view;
  for (size_t i = 0; i < stub_template->insn_count(); i++)
    {
      switch (insns[i].type())
	{
	case Insn_template::THUMB16_TYPE:
	  // Non-zero reloc addends are only used in Cortex-A8 stubs. 
	  gold_assert(insns[i].reloc_addend() == 0);
	  elfcpp::Swap<16, big_endian>::writeval(pov, insns[i].data() & 0xffff);
	  break;
	case Insn_template::THUMB32_TYPE:
	  {
	    uint32_t hi = (insns[i].data() >> 16) & 0xffff;
	    uint32_t lo = insns[i].data() & 0xffff;
	    elfcpp::Swap<16, big_endian>::writeval(pov, hi);
	    elfcpp::Swap<16, big_endian>::writeval(pov + 2, lo);
	  }
          break;
	case Insn_template::ARM_TYPE:
	case Insn_template::DATA_TYPE:
	  elfcpp::Swap<32, big_endian>::writeval(pov, insns[i].data());
	  break;
	default:
	  gold_unreachable();
	}
      pov += insns[i].size();
    }
  gold_assert(static_cast<section_size_type>(pov - view) == view_size);
} 

// Write a reloc stub to VIEW with endianity specified by BIG_ENDIAN.

void
Reloc_stub::do_write(unsigned char* view, section_size_type view_size,
		     bool big_endian)
{
  if (big_endian)
    this->do_fixed_endian_write<true>(view, view_size);
  else
    this->do_fixed_endian_write<false>(view, view_size);
}

// Stub_factory methods.

Stub_factory::Stub_factory()
{
  // The instruction template sequences are declared as static
  // objects and initialized first time the constructor runs.
 
  // Arm/Thumb -> Arm/Thumb long branch stub. On V5T and above, use blx
  // to reach the stub if necessary.
  static const Insn_template elf32_arm_stub_long_branch_any_any[] =
    {
      Insn_template::arm_insn(0xe51ff004),	// ldr   pc, [pc, #-4]
      Insn_template::data_word(0, elfcpp::R_ARM_ABS32, 0),
  						// dcd   R_ARM_ABS32(X)
    };
  
  // V4T Arm -> Thumb long branch stub. Used on V4T where blx is not
  // available.
  static const Insn_template elf32_arm_stub_long_branch_v4t_arm_thumb[] =
    {
      Insn_template::arm_insn(0xe59fc000),	// ldr   ip, [pc, #0]
      Insn_template::arm_insn(0xe12fff1c),	// bx    ip
      Insn_template::data_word(0, elfcpp::R_ARM_ABS32, 0),
  						// dcd   R_ARM_ABS32(X)
    };
  
  // Thumb -> Thumb long branch stub. Used on M-profile architectures.
  static const Insn_template elf32_arm_stub_long_branch_thumb_only[] =
    {
      Insn_template::thumb16_insn(0xb401),	// push {r0}
      Insn_template::thumb16_insn(0x4802),	// ldr  r0, [pc, #8]
      Insn_template::thumb16_insn(0x4684),	// mov  ip, r0
      Insn_template::thumb16_insn(0xbc01),	// pop  {r0}
      Insn_template::thumb16_insn(0x4760),	// bx   ip
      Insn_template::thumb16_insn(0xbf00),	// nop
      Insn_template::data_word(0, elfcpp::R_ARM_ABS32, 0),
  						// dcd  R_ARM_ABS32(X)
    };
  
  // V4T Thumb -> Thumb long branch stub. Using the stack is not
  // allowed.
  static const Insn_template elf32_arm_stub_long_branch_v4t_thumb_thumb[] =
    {
      Insn_template::thumb16_insn(0x4778),	// bx   pc
      Insn_template::thumb16_insn(0x46c0),	// nop
      Insn_template::arm_insn(0xe59fc000),	// ldr  ip, [pc, #0]
      Insn_template::arm_insn(0xe12fff1c),	// bx   ip
      Insn_template::data_word(0, elfcpp::R_ARM_ABS32, 0),
  						// dcd  R_ARM_ABS32(X)
    };
  
  // V4T Thumb -> ARM long branch stub. Used on V4T where blx is not
  // available.
  static const Insn_template elf32_arm_stub_long_branch_v4t_thumb_arm[] =
    {
      Insn_template::thumb16_insn(0x4778),	// bx   pc
      Insn_template::thumb16_insn(0x46c0),	// nop
      Insn_template::arm_insn(0xe51ff004),	// ldr   pc, [pc, #-4]
      Insn_template::data_word(0, elfcpp::R_ARM_ABS32, 0),
  						// dcd   R_ARM_ABS32(X)
    };
  
  // V4T Thumb -> ARM short branch stub. Shorter variant of the above
  // one, when the destination is close enough.
  static const Insn_template elf32_arm_stub_short_branch_v4t_thumb_arm[] =
    {
      Insn_template::thumb16_insn(0x4778),		// bx   pc
      Insn_template::thumb16_insn(0x46c0),		// nop
      Insn_template::arm_rel_insn(0xea000000, -8),	// b    (X-8)
    };
  
  // ARM/Thumb -> ARM long branch stub, PIC.  On V5T and above, use
  // blx to reach the stub if necessary.
  static const Insn_template elf32_arm_stub_long_branch_any_arm_pic[] =
    {
      Insn_template::arm_insn(0xe59fc000),	// ldr   r12, [pc]
      Insn_template::arm_insn(0xe08ff00c),	// add   pc, pc, ip
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, -4),
  						// dcd   R_ARM_REL32(X-4)
    };
  
  // ARM/Thumb -> Thumb long branch stub, PIC.  On V5T and above, use
  // blx to reach the stub if necessary.  We can not add into pc;
  // it is not guaranteed to mode switch (different in ARMv6 and
  // ARMv7).
  static const Insn_template elf32_arm_stub_long_branch_any_thumb_pic[] =
    {
      Insn_template::arm_insn(0xe59fc004),	// ldr   r12, [pc, #4]
      Insn_template::arm_insn(0xe08fc00c),	// add   ip, pc, ip
      Insn_template::arm_insn(0xe12fff1c),	// bx    ip
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, 0),
  						// dcd   R_ARM_REL32(X)
    };
  
  // V4T ARM -> ARM long branch stub, PIC.
  static const Insn_template elf32_arm_stub_long_branch_v4t_arm_thumb_pic[] =
    {
      Insn_template::arm_insn(0xe59fc004),	// ldr   ip, [pc, #4]
      Insn_template::arm_insn(0xe08fc00c),	// add   ip, pc, ip
      Insn_template::arm_insn(0xe12fff1c),	// bx    ip
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, 0),
  						// dcd   R_ARM_REL32(X)
    };
  
  // V4T Thumb -> ARM long branch stub, PIC.
  static const Insn_template elf32_arm_stub_long_branch_v4t_thumb_arm_pic[] =
    {
      Insn_template::thumb16_insn(0x4778),	// bx   pc
      Insn_template::thumb16_insn(0x46c0),	// nop
      Insn_template::arm_insn(0xe59fc000),	// ldr  ip, [pc, #0]
      Insn_template::arm_insn(0xe08cf00f),	// add  pc, ip, pc
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, -4),
  						// dcd  R_ARM_REL32(X)
    };
  
  // Thumb -> Thumb long branch stub, PIC. Used on M-profile
  // architectures.
  static const Insn_template elf32_arm_stub_long_branch_thumb_only_pic[] =
    {
      Insn_template::thumb16_insn(0xb401),	// push {r0}
      Insn_template::thumb16_insn(0x4802),	// ldr  r0, [pc, #8]
      Insn_template::thumb16_insn(0x46fc),	// mov  ip, pc
      Insn_template::thumb16_insn(0x4484),	// add  ip, r0
      Insn_template::thumb16_insn(0xbc01),	// pop  {r0}
      Insn_template::thumb16_insn(0x4760),	// bx   ip
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, 4),
  						// dcd  R_ARM_REL32(X)
    };
  
  // V4T Thumb -> Thumb long branch stub, PIC. Using the stack is not
  // allowed.
  static const Insn_template elf32_arm_stub_long_branch_v4t_thumb_thumb_pic[] =
    {
      Insn_template::thumb16_insn(0x4778),	// bx   pc
      Insn_template::thumb16_insn(0x46c0),	// nop
      Insn_template::arm_insn(0xe59fc004),	// ldr  ip, [pc, #4]
      Insn_template::arm_insn(0xe08fc00c),	// add   ip, pc, ip
      Insn_template::arm_insn(0xe12fff1c),	// bx   ip
      Insn_template::data_word(0, elfcpp::R_ARM_REL32, 0),
  						// dcd  R_ARM_REL32(X)
    };
  
  // Cortex-A8 erratum-workaround stubs.
  
  // Stub used for conditional branches (which may be beyond +/-1MB away,
  // so we can't use a conditional branch to reach this stub).
  
  // original code:
  //
  // 	b<cond> X
  // after:
  //
  static const Insn_template elf32_arm_stub_a8_veneer_b_cond[] =
    {
      Insn_template::thumb16_bcond_insn(0xd001),	//	b<cond>.n true
      Insn_template::thumb32_b_insn(0xf000b800, -4),	//	b.w after
      Insn_template::thumb32_b_insn(0xf000b800, -4)	// true:
  							//	b.w X
    };
  
  // Stub used for b.w and bl.w instructions.
  
  static const Insn_template elf32_arm_stub_a8_veneer_b[] =
    {
      Insn_template::thumb32_b_insn(0xf000b800, -4)	// b.w dest
    };
  
  static const Insn_template elf32_arm_stub_a8_veneer_bl[] =
    {
      Insn_template::thumb32_b_insn(0xf000b800, -4)	// b.w dest
    };
  
  // Stub used for Thumb-2 blx.w instructions.  We modified the original blx.w
  // instruction (which switches to ARM mode) to point to this stub.  Jump to
  // the real destination using an ARM-mode branch.
  const Insn_template elf32_arm_stub_a8_veneer_blx[] =
    {
      Insn_template::arm_rel_insn(0xea000000, -8)	// b dest
    };

  // Fill in the stub template look-up table.  Stub templates are constructed
  // per instance of Stub_factory for fast look-up without locking
  // in a thread-enabled environment.

  this->stub_templates_[arm_stub_none] =
    new Stub_template(arm_stub_none, NULL, 0);

#define DEF_STUB(x)	\
  do \
    { \
      size_t array_size \
	= sizeof(elf32_arm_stub_##x) / sizeof(elf32_arm_stub_##x[0]); \
      Stub_type type = arm_stub_##x; \
      this->stub_templates_[type] = \
	new Stub_template(type, elf32_arm_stub_##x, array_size); \
    } \
  while (0);

  DEF_STUBS
#undef DEF_STUB
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

// We are about to emit a dynamic relocation of type R_TYPE.  If the
// dynamic linker does not support it, issue an error.  The GNU linker
// only issues a non-PIC error for an allocated read-only section.
// Here we know the section is allocated, but we don't know that it is
// read-only.  But we check for all the relocation types which the
// glibc dynamic linker supports, so it seems appropriate to issue an
// error even if the section is not read-only.

template<bool big_endian>
void
Target_arm<big_endian>::Scan::check_non_pic(Relobj* object,
					    unsigned int r_type)
{
  switch (r_type)
    {
    // These are the relocation types supported by glibc for ARM.
    case elfcpp::R_ARM_RELATIVE:
    case elfcpp::R_ARM_COPY:
    case elfcpp::R_ARM_GLOB_DAT:
    case elfcpp::R_ARM_JUMP_SLOT:
    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_ABS32_NOI:
    case elfcpp::R_ARM_PC24:
    // FIXME: The following 3 types are not supported by Android's dynamic
    // linker.
    case elfcpp::R_ARM_TLS_DTPMOD32:
    case elfcpp::R_ARM_TLS_DTPOFF32:
    case elfcpp::R_ARM_TLS_TPOFF32:
      return;

    default:
      // This prevents us from issuing more than one error per reloc
      // section.  But we can still wind up issuing more than one
      // error per object file.
      if (this->issued_non_pic_error_)
	return;
      object->error(_("requires unsupported dynamic reloc; "
		      "recompile with -fPIC"));
      this->issued_non_pic_error_ = true;
      return;

    case elfcpp::R_ARM_NONE:
      gold_unreachable();
    }
}

// Scan a relocation for a local symbol.
// FIXME: This only handles a subset of relocation types used by Android
// on ARM v5te devices.

template<bool big_endian>
inline void
Target_arm<big_endian>::Scan::local(const General_options&,
				    Symbol_table* symtab,
				    Layout* layout,
				    Target_arm* target,
				    Sized_relobj<32, big_endian>* object,
				    unsigned int data_shndx,
				    Output_section* output_section,
				    const elfcpp::Rel<32, big_endian>& reloc,
				    unsigned int r_type,
				    const elfcpp::Sym<32, big_endian>&)
{
  r_type = get_real_reloc_type(r_type);
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_ABS32_NOI:
      // If building a shared library (or a position-independent
      // executable), we need to create a dynamic relocation for
      // this location. The relocation applied at link time will
      // apply the link-time value, so we flag the location with
      // an R_ARM_RELATIVE relocation so the dynamic loader can
      // relocate it easily.
      if (parameters->options().output_is_position_independent())
	{
	  Reloc_section* rel_dyn = target->rel_dyn_section(layout);
	  unsigned int r_sym = elfcpp::elf_r_sym<32>(reloc.get_r_info());
   	  // If we are to add more other reloc types than R_ARM_ABS32,
   	  // we need to add check_non_pic(object, r_type) here.
	  rel_dyn->add_local_relative(object, r_sym, elfcpp::R_ARM_RELATIVE,
				      output_section, data_shndx,
				      reloc.get_r_offset());
	}
      break;

    case elfcpp::R_ARM_REL32:
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_PREL31:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_PLT32:
    case elfcpp::R_ARM_THM_ABS5:
    case elfcpp::R_ARM_ABS8:
    case elfcpp::R_ARM_ABS12:
    case elfcpp::R_ARM_ABS16:
    case elfcpp::R_ARM_BASE_ABS:
    case elfcpp::R_ARM_MOVW_ABS_NC:
    case elfcpp::R_ARM_MOVT_ABS:
    case elfcpp::R_ARM_THM_MOVW_ABS_NC:
    case elfcpp::R_ARM_THM_MOVT_ABS:
    case elfcpp::R_ARM_MOVW_PREL_NC:
    case elfcpp::R_ARM_MOVT_PREL:
    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
    case elfcpp::R_ARM_THM_MOVT_PREL:
      break;

    case elfcpp::R_ARM_GOTOFF32:
      // We need a GOT section:
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_ARM_BASE_PREL:
      // FIXME: What about this?
      break;

    case elfcpp::R_ARM_GOT_BREL:
    case elfcpp::R_ARM_GOT_PREL:
      {
	// The symbol requires a GOT entry.
	Output_data_got<32, big_endian>* got =
	  target->got_section(symtab, layout);
	unsigned int r_sym = elfcpp::elf_r_sym<32>(reloc.get_r_info());
	if (got->add_local(object, r_sym, GOT_TYPE_STANDARD))
	  {
	    // If we are generating a shared object, we need to add a
	    // dynamic RELATIVE relocation for this symbol's GOT entry.
	    if (parameters->options().output_is_position_independent())
	      {
		Reloc_section* rel_dyn = target->rel_dyn_section(layout);
		unsigned int r_sym = elfcpp::elf_r_sym<32>(reloc.get_r_info());
		rel_dyn->add_local_relative(
		    object, r_sym, elfcpp::R_ARM_RELATIVE, got,
		    object->local_got_offset(r_sym, GOT_TYPE_STANDARD));
	      }
	  }
      }
      break;

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
// FIXME: This only handles a subset of relocation types used by Android
// on ARM v5te devices.

template<bool big_endian>
inline void
Target_arm<big_endian>::Scan::global(const General_options&,
				     Symbol_table* symtab,
				     Layout* layout,
				     Target_arm* target,
				     Sized_relobj<32, big_endian>* object,
				     unsigned int data_shndx,
				     Output_section* output_section,
				     const elfcpp::Rel<32, big_endian>& reloc,
				     unsigned int r_type,
				     Symbol* gsym)
{
  r_type = get_real_reloc_type(r_type);
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_ABS32_NOI:
      {
	// Make a dynamic relocation if necessary.
	if (gsym->needs_dynamic_reloc(Symbol::ABSOLUTE_REF))
	  {
	    if (target->may_need_copy_reloc(gsym))
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym, reloc);
	      }
	    else if (gsym->can_use_relative_reloc(false))
	      {
   		// If we are to add more other reloc types than R_ARM_ABS32,
   		// we need to add check_non_pic(object, r_type) here.
		Reloc_section* rel_dyn = target->rel_dyn_section(layout);
		rel_dyn->add_global_relative(gsym, elfcpp::R_ARM_RELATIVE,
					     output_section, object,
					     data_shndx, reloc.get_r_offset());
	      }
	    else
	      {
   		// If we are to add more other reloc types than R_ARM_ABS32,
   		// we need to add check_non_pic(object, r_type) here.
		Reloc_section* rel_dyn = target->rel_dyn_section(layout);
		rel_dyn->add_global(gsym, r_type, output_section, object,
				    data_shndx, reloc.get_r_offset());
	      }
	  }
      }
      break;

    case elfcpp::R_ARM_MOVW_ABS_NC:
    case elfcpp::R_ARM_MOVT_ABS:
    case elfcpp::R_ARM_THM_MOVW_ABS_NC:
    case elfcpp::R_ARM_THM_MOVT_ABS:
    case elfcpp::R_ARM_MOVW_PREL_NC:
    case elfcpp::R_ARM_MOVT_PREL:
    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
    case elfcpp::R_ARM_THM_MOVT_PREL:
      break;

    case elfcpp::R_ARM_THM_ABS5:
    case elfcpp::R_ARM_ABS8:
    case elfcpp::R_ARM_ABS12:
    case elfcpp::R_ARM_ABS16:
    case elfcpp::R_ARM_BASE_ABS:
      {
	// No dynamic relocs of this kinds.
	// Report the error in case of PIC.
	int flags = Symbol::NON_PIC_REF;
	if (gsym->type() == elfcpp::STT_FUNC
	    || gsym->type() == elfcpp::STT_ARM_TFUNC)
	  flags |= Symbol::FUNCTION_CALL;
	if (gsym->needs_dynamic_reloc(flags))
	  check_non_pic(object, r_type);
      }
      break;

    case elfcpp::R_ARM_REL32:
    case elfcpp::R_ARM_PREL31:
      {
	// Make a dynamic relocation if necessary.
	int flags = Symbol::NON_PIC_REF;
	if (gsym->needs_dynamic_reloc(flags))
	  {
	    if (target->may_need_copy_reloc(gsym))
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym, reloc);
	      }
	    else
	      {
		check_non_pic(object, r_type);
		Reloc_section* rel_dyn = target->rel_dyn_section(layout);
		rel_dyn->add_global(gsym, r_type, output_section, object,
				    data_shndx, reloc.get_r_offset());
	      }
	  }
      }
      break;

    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_CALL:
      {
	if (Target_arm<big_endian>::Scan::symbol_needs_plt_entry(gsym))
	  target->make_plt_entry(symtab, layout, gsym);
	// Make a dynamic relocation if necessary.
	int flags = Symbol::NON_PIC_REF;
	if (gsym->type() == elfcpp::STT_FUNC
	    || gsym->type() == elfcpp::STT_ARM_TFUNC)
	  flags |= Symbol::FUNCTION_CALL;
	if (gsym->needs_dynamic_reloc(flags))
	  {
	    if (target->may_need_copy_reloc(gsym))
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym,
				   reloc);
	      }
	    else
	      {
		check_non_pic(object, r_type);
		Reloc_section* rel_dyn = target->rel_dyn_section(layout);
		rel_dyn->add_global(gsym, r_type, output_section, object,
				    data_shndx, reloc.get_r_offset());
	      }
	  }
      }
      break;

    case elfcpp::R_ARM_PLT32:
      // If the symbol is fully resolved, this is just a relative
      // local reloc.  Otherwise we need a PLT entry.
      if (gsym->final_value_is_known())
	break;
      // If building a shared library, we can also skip the PLT entry
      // if the symbol is defined in the output file and is protected
      // or hidden.
      if (gsym->is_defined()
	  && !gsym->is_from_dynobj()
	  && !gsym->is_preemptible())
	break;
      target->make_plt_entry(symtab, layout, gsym);
      break;

    case elfcpp::R_ARM_GOTOFF32:
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_ARM_BASE_PREL:
      // FIXME: What about this?
      break;
      
    case elfcpp::R_ARM_GOT_BREL:
    case elfcpp::R_ARM_GOT_PREL:
      {
	// The symbol requires a GOT entry.
	Output_data_got<32, big_endian>* got =
	  target->got_section(symtab, layout);
	if (gsym->final_value_is_known())
	  got->add_global(gsym, GOT_TYPE_STANDARD);
	else
	  {
	    // If this symbol is not fully resolved, we need to add a
	    // GOT entry with a dynamic relocation.
	    Reloc_section* rel_dyn = target->rel_dyn_section(layout);
	    if (gsym->is_from_dynobj()
		|| gsym->is_undefined()
		|| gsym->is_preemptible())
	      got->add_global_with_rel(gsym, GOT_TYPE_STANDARD,
				       rel_dyn, elfcpp::R_ARM_GLOB_DAT);
	    else
	      {
		if (got->add_global(gsym, GOT_TYPE_STANDARD))
		  rel_dyn->add_global_relative(
		      gsym, elfcpp::R_ARM_RELATIVE, got,
		      gsym->got_offset(GOT_TYPE_STANDARD));
	      }
	  }
      }
      break;

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

  // For the ARM target, we need to add a PT_ARM_EXIDX segment for
  // the .ARM.exidx section.
  if (!layout->script_options()->saw_phdrs_clause()
      && !parameters->options().relocatable())
    {
      Output_section* exidx_section =
	layout->find_output_section(".ARM.exidx");

      if (exidx_section != NULL
	  && exidx_section->type() == elfcpp::SHT_ARM_EXIDX)
	{
	  gold_assert(layout->find_output_segment(elfcpp::PT_ARM_EXIDX, 0, 0)
		      == NULL);
	  Output_segment*  exidx_segment =
	    layout->make_output_segment(elfcpp::PT_ARM_EXIDX, elfcpp::PF_R);
	  exidx_segment->add_output_section(exidx_section, elfcpp::PF_R);
	}
    }
}

// Return whether a direct absolute static relocation needs to be applied.
// In cases where Scan::local() or Scan::global() has created
// a dynamic relocation other than R_ARM_RELATIVE, the addend
// of the relocation is carried in the data, and we must not
// apply the static relocation.

template<bool big_endian>
inline bool
Target_arm<big_endian>::Relocate::should_apply_static_reloc(
    const Sized_symbol<32>* gsym,
    int ref_flags,
    bool is_32bit,
    Output_section* output_section)
{
  // If the output section is not allocated, then we didn't call
  // scan_relocs, we didn't create a dynamic reloc, and we must apply
  // the reloc here.
  if ((output_section->flags() & elfcpp::SHF_ALLOC) == 0)
      return true;

  // For local symbols, we will have created a non-RELATIVE dynamic
  // relocation only if (a) the output is position independent,
  // (b) the relocation is absolute (not pc- or segment-relative), and
  // (c) the relocation is not 32 bits wide.
  if (gsym == NULL)
    return !(parameters->options().output_is_position_independent()
	     && (ref_flags & Symbol::ABSOLUTE_REF)
	     && !is_32bit);

  // For global symbols, we use the same helper routines used in the
  // scan pass.  If we did not create a dynamic relocation, or if we
  // created a RELATIVE dynamic relocation, we should apply the static
  // relocation.
  bool has_dyn = gsym->needs_dynamic_reloc(ref_flags);
  bool is_rel = (ref_flags & Symbol::ABSOLUTE_REF)
		 && gsym->can_use_relative_reloc(ref_flags
						 & Symbol::FUNCTION_CALL);
  return !has_dyn || is_rel;
}

// Perform a relocation.

template<bool big_endian>
inline bool
Target_arm<big_endian>::Relocate::relocate(
    const Relocate_info<32, big_endian>* relinfo,
    Target_arm* target,
    Output_section *output_section,
    size_t relnum,
    const elfcpp::Rel<32, big_endian>& rel,
    unsigned int r_type,
    const Sized_symbol<32>* gsym,
    const Symbol_value<32>* psymval,
    unsigned char* view,
    elfcpp::Elf_types<32>::Elf_Addr address,
    section_size_type /* view_size */ )
{
  typedef Arm_relocate_functions<big_endian> Arm_relocate_functions;

  r_type = get_real_reloc_type(r_type);

  // If this the symbol may be a Thumb function, set thumb bit to 1.
  bool has_thumb_bit = ((gsym != NULL)
			&& (gsym->type() == elfcpp::STT_FUNC
			    || gsym->type() == elfcpp::STT_ARM_TFUNC));

  // Pick the value to use for symbols defined in shared objects.
  Symbol_value<32> symval;
  if (gsym != NULL
      && gsym->use_plt_offset(reloc_is_non_pic(r_type)))
    {
      symval.set_output_value(target->plt_section()->address()
			      + gsym->plt_offset());
      psymval = &symval;
      has_thumb_bit = 0;
    }

  const Sized_relobj<32, big_endian>* object = relinfo->object;
  
  // Get the GOT offset if needed.
  // The GOT pointer points to the end of the GOT section.
  // We need to subtract the size of the GOT section to get
  // the actual offset to use in the relocation.
  bool have_got_offset = false;
  unsigned int got_offset = 0;
  switch (r_type)
    {
    case elfcpp::R_ARM_GOT_BREL:
    case elfcpp::R_ARM_GOT_PREL:
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_STANDARD));
	  got_offset = (gsym->got_offset(GOT_TYPE_STANDARD)
			- target->got_size());
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<32>(rel.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD));
	  got_offset = (object->local_got_offset(r_sym, GOT_TYPE_STANDARD)
			- target->got_size());
	}
      have_got_offset = true;
      break;

    default:
      break;
    }

  typename Arm_relocate_functions::Status reloc_status =
	Arm_relocate_functions::STATUS_OKAY;
  switch (r_type)
    {
    case elfcpp::R_ARM_NONE:
      break;

    case elfcpp::R_ARM_ABS8:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, false,
				    output_section))
	reloc_status = Arm_relocate_functions::abs8(view, object, psymval);
      break;

    case elfcpp::R_ARM_ABS12:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, false,
				    output_section))
	reloc_status = Arm_relocate_functions::abs12(view, object, psymval);
      break;

    case elfcpp::R_ARM_ABS16:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, false,
				    output_section))
	reloc_status = Arm_relocate_functions::abs16(view, object, psymval);
      break;

    case elfcpp::R_ARM_ABS32:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	reloc_status = Arm_relocate_functions::abs32(view, object, psymval,
						     has_thumb_bit);
      break;

    case elfcpp::R_ARM_ABS32_NOI:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	// No thumb bit for this relocation: (S + A)
	reloc_status = Arm_relocate_functions::abs32(view, object, psymval,
						     false);
      break;

    case elfcpp::R_ARM_MOVW_ABS_NC:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	reloc_status = Arm_relocate_functions::movw_abs_nc(view, object,
							   psymval,
       						           has_thumb_bit);
      else
	gold_error(_("relocation R_ARM_MOVW_ABS_NC cannot be used when making"
		     "a shared object; recompile with -fPIC"));
      break;

    case elfcpp::R_ARM_MOVT_ABS:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	reloc_status = Arm_relocate_functions::movt_abs(view, object, psymval);
      else
	gold_error(_("relocation R_ARM_MOVT_ABS cannot be used when making"
		     "a shared object; recompile with -fPIC"));
      break;

    case elfcpp::R_ARM_THM_MOVW_ABS_NC:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	reloc_status = Arm_relocate_functions::thm_movw_abs_nc(view, object,
							       psymval,
       						               has_thumb_bit);
      else
	gold_error(_("relocation R_ARM_THM_MOVW_ABS_NC cannot be used when"
		     "making a shared object; recompile with -fPIC"));
      break;

    case elfcpp::R_ARM_THM_MOVT_ABS:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				    output_section))
	reloc_status = Arm_relocate_functions::thm_movt_abs(view, object,
							    psymval);
      else
	gold_error(_("relocation R_ARM_THM_MOVT_ABS cannot be used when"
		     "making a shared object; recompile with -fPIC"));
      break;

    case elfcpp::R_ARM_MOVW_PREL_NC:
      reloc_status = Arm_relocate_functions::movw_prel_nc(view, object,
							  psymval, address,
							  has_thumb_bit);
      break;

    case elfcpp::R_ARM_MOVT_PREL:
      reloc_status = Arm_relocate_functions::movt_prel(view, object,
                                                       psymval, address);
      break;

    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
      reloc_status = Arm_relocate_functions::thm_movw_prel_nc(view, object,
							      psymval, address,
							      has_thumb_bit);
      break;

    case elfcpp::R_ARM_THM_MOVT_PREL:
      reloc_status = Arm_relocate_functions::thm_movt_prel(view, object,
							   psymval, address);
      break;
	
    case elfcpp::R_ARM_REL32:
      reloc_status = Arm_relocate_functions::rel32(view, object, psymval,
						   address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_THM_ABS5:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, false,
				    output_section))
	reloc_status = Arm_relocate_functions::thm_abs5(view, object, psymval);
      break;

    case elfcpp::R_ARM_THM_CALL:
      reloc_status = Arm_relocate_functions::thm_call(view, object, psymval,
						      address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_GOTOFF32:
      {
	elfcpp::Elf_types<32>::Elf_Addr got_origin;
	got_origin = target->got_plt_section()->address();
	reloc_status = Arm_relocate_functions::rel32(view, object, psymval,
						     got_origin, has_thumb_bit);
      }
      break;

    case elfcpp::R_ARM_BASE_PREL:
      {
	uint32_t origin;
	// Get the addressing origin of the output segment defining the 
	// symbol gsym (AAELF 4.6.1.2 Relocation types)
	gold_assert(gsym != NULL); 
	if (gsym->source() == Symbol::IN_OUTPUT_SEGMENT)
	  origin = gsym->output_segment()->vaddr();
	else if (gsym->source () == Symbol::IN_OUTPUT_DATA)
	  origin = gsym->output_data()->address();
	else
	  {
            gold_error_at_location(relinfo, relnum, rel.get_r_offset(),
				   _("cannot find origin of R_ARM_BASE_PREL"));
	    return true;
	  }
	reloc_status = Arm_relocate_functions::base_prel(view, origin, address);
      }
      break;

    case elfcpp::R_ARM_BASE_ABS:
      {
	if (!should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, true,
				      output_section))
	  break;

	uint32_t origin;
	// Get the addressing origin of the output segment defining
	// the symbol gsym (AAELF 4.6.1.2 Relocation types).
	if (gsym == NULL)
	  // R_ARM_BASE_ABS with the NULL symbol will give the
	  // absolute address of the GOT origin (GOT_ORG) (see ARM IHI
	  // 0044C (AAELF): 4.6.1.8 Proxy generating relocations).
	  origin = target->got_plt_section()->address();
	else if (gsym->source() == Symbol::IN_OUTPUT_SEGMENT)
	  origin = gsym->output_segment()->vaddr();
	else if (gsym->source () == Symbol::IN_OUTPUT_DATA)
	  origin = gsym->output_data()->address();
	else
	  {
            gold_error_at_location(relinfo, relnum, rel.get_r_offset(),
				   _("cannot find origin of R_ARM_BASE_ABS"));
	    return true;
	  }

	reloc_status = Arm_relocate_functions::base_abs(view, origin);
      }
      break;

    case elfcpp::R_ARM_GOT_BREL:
      gold_assert(have_got_offset);
      reloc_status = Arm_relocate_functions::got_brel(view, got_offset);
      break;

    case elfcpp::R_ARM_GOT_PREL:
      gold_assert(have_got_offset);
      // Get the address origin for GOT PLT, which is allocated right
      // after the GOT section, to calculate an absolute address of
      // the symbol GOT entry (got_origin + got_offset).
      elfcpp::Elf_types<32>::Elf_Addr got_origin;
      got_origin = target->got_plt_section()->address();
      reloc_status = Arm_relocate_functions::got_prel(view,
						      got_origin + got_offset,
						      address);
      break;

    case elfcpp::R_ARM_PLT32:
      gold_assert(gsym == NULL
		  || gsym->has_plt_offset()
		  || gsym->final_value_is_known()
		  || (gsym->is_defined()
		      && !gsym->is_from_dynobj()
		      && !gsym->is_preemptible()));
      reloc_status = Arm_relocate_functions::plt32(view, object, psymval,
						   address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_CALL:
      reloc_status = Arm_relocate_functions::call(view, object, psymval,
						  address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_JUMP24:
      reloc_status = Arm_relocate_functions::jump24(view, object, psymval,
						    address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_PREL31:
      reloc_status = Arm_relocate_functions::prel31(view, object, psymval,
						    address, has_thumb_bit);
      break;

    case elfcpp::R_ARM_TARGET1:
      // This should have been mapped to another type already.
      // Fall through.
    case elfcpp::R_ARM_COPY:
    case elfcpp::R_ARM_GLOB_DAT:
    case elfcpp::R_ARM_JUMP_SLOT:
    case elfcpp::R_ARM_RELATIVE:
      // These are relocations which should only be seen by the
      // dynamic linker, and should never be seen here.
      gold_error_at_location(relinfo, relnum, rel.get_r_offset(),
			     _("unexpected reloc %u in object file"),
			     r_type);
      break;

    default:
      gold_error_at_location(relinfo, relnum, rel.get_r_offset(),
			     _("unsupported reloc %u"),
			     r_type);
      break;
    }

  // Report any errors.
  switch (reloc_status)
    {
    case Arm_relocate_functions::STATUS_OKAY:
      break;
    case Arm_relocate_functions::STATUS_OVERFLOW:
      gold_error_at_location(relinfo, relnum, rel.get_r_offset(),
			     _("relocation overflow in relocation %u"),
			     r_type);
      break;
    case Arm_relocate_functions::STATUS_BAD_RELOC:
      gold_error_at_location(
	relinfo,
	relnum,
	rel.get_r_offset(),
	_("unexpected opcode while processing relocation %u"),
	r_type);
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
    section_size_type view_size,
    const Reloc_symbol_changes* reloc_symbol_changes)
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
    view_size,
    reloc_symbol_changes);
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

    case elfcpp::R_ARM_ABS8:
      return 1;

    case elfcpp::R_ARM_ABS16:
    case elfcpp::R_ARM_THM_ABS5:
      return 2;

    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_ABS32_NOI:
    case elfcpp::R_ARM_ABS12:
    case elfcpp::R_ARM_BASE_ABS:
    case elfcpp::R_ARM_REL32:
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_GOTOFF32:
    case elfcpp::R_ARM_BASE_PREL:
    case elfcpp::R_ARM_GOT_BREL:
    case elfcpp::R_ARM_GOT_PREL:
    case elfcpp::R_ARM_PLT32:
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_PREL31:
    case elfcpp::R_ARM_MOVW_ABS_NC:
    case elfcpp::R_ARM_MOVT_ABS:
    case elfcpp::R_ARM_THM_MOVW_ABS_NC:
    case elfcpp::R_ARM_THM_MOVT_ABS:
    case elfcpp::R_ARM_MOVW_PREL_NC:
    case elfcpp::R_ARM_MOVT_PREL:
    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
    case elfcpp::R_ARM_THM_MOVT_PREL:
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
