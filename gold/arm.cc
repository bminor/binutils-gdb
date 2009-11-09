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
#include <algorithm>

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
class Stub_table;

template<bool big_endian>
class Arm_input_section;

template<bool big_endian>
class Arm_output_section;

template<bool big_endian>
class Arm_relobj;

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
    // independent Symbol and Relobj classes instead of Sized_symbol<32> and  
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

// A class to hold stubs for the ARM target.

template<bool big_endian>
class Stub_table : public Output_data
{
 public:
  Stub_table(Arm_input_section<big_endian>* owner)
    : Output_data(), addralign_(1), owner_(owner), has_been_changed_(false),
      reloc_stubs_()
  { }

  ~Stub_table()
  { }

  // Owner of this stub table.
  Arm_input_section<big_endian>*
  owner() const
  { return this->owner_; }

  // Whether this stub table is empty.
  bool
  empty() const
  { return this->reloc_stubs_.empty(); }

  // Whether this has been changed.
  bool
  has_been_changed() const
  { return this->has_been_changed_; }

  // Set the has-been-changed flag.
  void
  set_has_been_changed(bool value)
  { this->has_been_changed_ = value; }

  // Return the current data size.
  off_t
  current_data_size() const
  { return this->current_data_size_for_child(); }

  // Add a STUB with using KEY.  Caller is reponsible for avoid adding
  // if already a STUB with the same key has been added. 
  void
  add_reloc_stub(Reloc_stub* stub, const Reloc_stub::Key& key);

  // Look up a relocation stub using KEY.  Return NULL if there is none.
  Reloc_stub*
  find_reloc_stub(const Reloc_stub::Key& key) const
  {
    typename Reloc_stub_map::const_iterator p = this->reloc_stubs_.find(key);
    return (p != this->reloc_stubs_.end()) ? p->second : NULL;
  }

  // Relocate stubs in this stub table.
  void
  relocate_stubs(const Relocate_info<32, big_endian>*,
		 Target_arm<big_endian>*, Output_section*,
		 unsigned char*, Arm_address, section_size_type);

 protected:
  // Write out section contents.
  void
  do_write(Output_file*);
 
  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return this->addralign_; }

  // Finalize data size.
  void
  set_final_data_size()
  { this->set_data_size(this->current_data_size_for_child()); }

  // Reset address and file offset.
  void
  do_reset_address_and_file_offset();

 private:
  // Unordered map of stubs.
  typedef
    Unordered_map<Reloc_stub::Key, Reloc_stub*, Reloc_stub::Key::hash,
		  Reloc_stub::Key::equal_to>
    Reloc_stub_map;

  // Address alignment
  uint64_t addralign_;
  // Owner of this stub table.
  Arm_input_section<big_endian>* owner_;
  // This is set to true during relaxiong if the size of the stub table
  // has been changed.
  bool has_been_changed_;
  // The relocation stubs.
  Reloc_stub_map reloc_stubs_;
};

// A class to wrap an ordinary input section containing executable code.

template<bool big_endian>
class Arm_input_section : public Output_relaxed_input_section
{
 public:
  Arm_input_section(Relobj* relobj, unsigned int shndx)
    : Output_relaxed_input_section(relobj, shndx, 1),
      original_addralign_(1), original_size_(0), stub_table_(NULL)
  { }

  ~Arm_input_section()
  { }

  // Initialize.
  void
  init();
  
  // Whether this is a stub table owner.
  bool
  is_stub_table_owner() const
  { return this->stub_table_ != NULL && this->stub_table_->owner() == this; }

  // Return the stub table.
  Stub_table<big_endian>*
  stub_table() const
  { return this->stub_table_; }

  // Set the stub_table.
  void
  set_stub_table(Stub_table<big_endian>* stub_table)
  { this->stub_table_ = stub_table; }

  // Downcast a base pointer to an Arm_input_section pointer.  This is
  // not type-safe but we only use Arm_input_section not the base class.
  static Arm_input_section<big_endian>*
  as_arm_input_section(Output_relaxed_input_section* poris)
  { return static_cast<Arm_input_section<big_endian>*>(poris); }

 protected:
  // Write data to output file.
  void
  do_write(Output_file*);

  // Return required alignment of this.
  uint64_t
  do_addralign() const
  {
    if (this->is_stub_table_owner())
      return std::max(this->stub_table_->addralign(),
		      this->original_addralign_);
    else
      return this->original_addralign_;
  }

  // Finalize data size.
  void
  set_final_data_size();

  // Reset address and file offset.
  void
  do_reset_address_and_file_offset();

  // Output offset.
  bool
  do_output_offset(const Relobj* object, unsigned int shndx,
		   section_offset_type offset,
                   section_offset_type* poutput) const
  {
    if ((object == this->relobj())
	&& (shndx == this->shndx())
	&& (offset >= 0)
	&& (convert_types<uint64_t, section_offset_type>(offset)
	    <= this->original_size_))
      {
	*poutput = offset;
	return true;
      }
    else
      return false;
  }

 private:
  // Copying is not allowed.
  Arm_input_section(const Arm_input_section&);
  Arm_input_section& operator=(const Arm_input_section&);

  // Address alignment of the original input section.
  uint64_t original_addralign_;
  // Section size of the original input section.
  uint64_t original_size_;
  // Stub table.
  Stub_table<big_endian>* stub_table_;
};

// Arm output section class.  This is defined mainly to add a number of
// stub generation methods.

template<bool big_endian>
class Arm_output_section : public Output_section
{
 public:
  Arm_output_section(const char* name, elfcpp::Elf_Word type,
		     elfcpp::Elf_Xword flags)
    : Output_section(name, type, flags)
  { }

  ~Arm_output_section()
  { }
  
  // Group input sections for stub generation.
  void
  group_sections(section_size_type, bool, Target_arm<big_endian>*);

  // Downcast a base pointer to an Arm_output_section pointer.  This is
  // not type-safe but we only use Arm_output_section not the base class.
  static Arm_output_section<big_endian>*
  as_arm_output_section(Output_section* os)
  { return static_cast<Arm_output_section<big_endian>*>(os); }

 private:
  // For convenience.
  typedef Output_section::Input_section Input_section;
  typedef Output_section::Input_section_list Input_section_list;

  // Create a stub group.
  void create_stub_group(Input_section_list::const_iterator,
			 Input_section_list::const_iterator,
			 Input_section_list::const_iterator,
			 Target_arm<big_endian>*,
			 std::vector<Output_relaxed_input_section*>*);
};

// Arm_relobj class.

template<bool big_endian>
class Arm_relobj : public Sized_relobj<32, big_endian>
{
 public:
  static const Arm_address invalid_address = static_cast<Arm_address>(-1);

  Arm_relobj(const std::string& name, Input_file* input_file, off_t offset,
             const typename elfcpp::Ehdr<32, big_endian>& ehdr)
    : Sized_relobj<32, big_endian>(name, input_file, offset, ehdr),
      stub_tables_(), local_symbol_is_thumb_function_()
  { }

  ~Arm_relobj()
  { }
 
  // Return the stub table of the SHNDX-th section if there is one.
  Stub_table<big_endian>*
  stub_table(unsigned int shndx) const
  {
    gold_assert(shndx < this->stub_tables_.size());
    return this->stub_tables_[shndx];
  }

  // Set STUB_TABLE to be the stub_table of the SHNDX-th section.
  void
  set_stub_table(unsigned int shndx, Stub_table<big_endian>* stub_table)
  {
    gold_assert(shndx < this->stub_tables_.size());
    this->stub_tables_[shndx] = stub_table;
  }

  // Whether a local symbol is a THUMB function.  R_SYM is the symbol table
  // index.  This is only valid after do_count_local_symbol is called.
  bool
  local_symbol_is_thumb_function(unsigned int r_sym) const
  {
    gold_assert(r_sym < this->local_symbol_is_thumb_function_.size());
    return this->local_symbol_is_thumb_function_[r_sym];
  }
  
  // Scan all relocation sections for stub generation.
  void
  scan_sections_for_stubs(Target_arm<big_endian>*, const Symbol_table*,
			  const Layout*);

  // Convert regular input section with index SHNDX to a relaxed section.
  void
  convert_input_section_to_relaxed_section(unsigned shndx)
  {
    // The stubs have relocations and we need to process them after writing
    // out the stubs.  So relocation now must follow section write.
    this->invalidate_section_offset(shndx);
    this->set_relocs_must_follow_section_writes();
  }

  // Downcast a base pointer to an Arm_relobj pointer.  This is
  // not type-safe but we only use Arm_relobj not the base class.
  static Arm_relobj<big_endian>*
  as_arm_relobj(Relobj* relobj)
  { return static_cast<Arm_relobj<big_endian>*>(relobj); }

  // Processor-specific flags in ELF file header.  This is valid only after
  // reading symbols.
  elfcpp::Elf_Word
  processor_specific_flags() const
  { return this->processor_specific_flags_; }

 protected:
  // Post constructor setup.
  void
  do_setup()
  {
    // Call parent's setup method.
    Sized_relobj<32, big_endian>::do_setup();

    // Initialize look-up tables.
    Stub_table_list empty_stub_table_list(this->shnum(), NULL);
    this->stub_tables_.swap(empty_stub_table_list);
  }

  // Count the local symbols.
  void
  do_count_local_symbols(Stringpool_template<char>*,
                         Stringpool_template<char>*);

  void
  do_relocate_sections(const General_options& options,
		       const Symbol_table* symtab, const Layout* layout,
		       const unsigned char* pshdrs,
		       typename Sized_relobj<32, big_endian>::Views* pivews);

  // Read the symbol information.
  void
  do_read_symbols(Read_symbols_data* sd);

 private:
  // List of stub tables.
  typedef std::vector<Stub_table<big_endian>*> Stub_table_list;
  Stub_table_list stub_tables_;
  // Bit vector to tell if a local symbol is a thumb function or not.
  // This is only valid after do_count_local_symbol is called.
  std::vector<bool> local_symbol_is_thumb_function_;
  // processor-specific flags in ELF file header.
  elfcpp::Elf_Word processor_specific_flags_;
};

// Arm_dynobj class.

template<bool big_endian>
class Arm_dynobj : public Sized_dynobj<32, big_endian>
{
 public:
  Arm_dynobj(const std::string& name, Input_file* input_file, off_t offset,
	     const elfcpp::Ehdr<32, big_endian>& ehdr)
    : Sized_dynobj<32, big_endian>(name, input_file, offset, ehdr),
      processor_specific_flags_(0)
  { }
 
  ~Arm_dynobj()
  { }

  // Downcast a base pointer to an Arm_relobj pointer.  This is
  // not type-safe but we only use Arm_relobj not the base class.
  static Arm_dynobj<big_endian>*
  as_arm_dynobj(Dynobj* dynobj)
  { return static_cast<Arm_dynobj<big_endian>*>(dynobj); }

  // Processor-specific flags in ELF file header.  This is valid only after
  // reading symbols.
  elfcpp::Elf_Word
  processor_specific_flags() const
  { return this->processor_specific_flags_; }

 protected:
  // Read the symbol information.
  void
  do_read_symbols(Read_symbols_data* sd);

 private:
  // processor-specific flags in ELF file header.
  elfcpp::Elf_Word processor_specific_flags_;
};

// Functor to read reloc addends during stub generation.

template<int sh_type, bool big_endian>
struct Stub_addend_reader
{
  // Return the addend for a relocation of a particular type.  Depending
  // on whether this is a REL or RELA relocation, read the addend from a
  // view or from a Reloc object.
  elfcpp::Elf_types<32>::Elf_Swxword
  operator()(
    unsigned int /* r_type */,
    const unsigned char* /* view */,
    const typename Reloc_types<sh_type,
			       32, big_endian>::Reloc& /* reloc */) const;
};

// Specialized Stub_addend_reader for SHT_REL type relocation sections.

template<bool big_endian>
struct Stub_addend_reader<elfcpp::SHT_REL, big_endian>
{
  elfcpp::Elf_types<32>::Elf_Swxword
  operator()(
    unsigned int,
    const unsigned char*,
    const typename Reloc_types<elfcpp::SHT_REL, 32, big_endian>::Reloc&) const;
};

// Specialized Stub_addend_reader for RELA type relocation sections.
// We currently do not handle RELA type relocation sections but it is trivial
// to implement the addend reader.  This is provided for completeness and to
// make it easier to add support for RELA relocation sections in the future.

template<bool big_endian>
struct Stub_addend_reader<elfcpp::SHT_RELA, big_endian>
{
  elfcpp::Elf_types<32>::Elf_Swxword
  operator()(
    unsigned int,
    const unsigned char*,
    const typename Reloc_types<elfcpp::SHT_RELA, 32,
			       big_endian>::Reloc& reloc) const
  { return reloc.get_r_addend(); }
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

  // When were are relocating a stub, we pass this as the relocation number.
  static const size_t fake_relnum_for_stubs = static_cast<size_t>(-1);

  Target_arm()
    : Sized_target<32, big_endian>(&arm_info),
      got_(NULL), plt_(NULL), got_plt_(NULL), rel_dyn_(NULL),
      copy_relocs_(elfcpp::R_ARM_COPY), dynbss_(NULL), stub_tables_(),
      stub_factory_(Stub_factory::get_instance()),
      may_use_blx_(true), should_force_pic_veneer_(false),
      arm_input_section_map_()
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
  gc_process_relocs(Symbol_table* symtab,
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
  scan_relocs(Symbol_table* symtab,
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
  do_finalize_sections(Layout*, const Input_objects*);

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
		   Arm_address view_address,
		   section_size_type view_size,
		   const Reloc_symbol_changes*);

  // Scan the relocs during a relocatable link.
  void
  scan_relocatable_relocs(Symbol_table* symtab,
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
			   Arm_address view_address,
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

  //
  // Methods to support stub-generations.
  //
  
  // Return the stub factory
  const Stub_factory&
  stub_factory() const
  { return this->stub_factory_; }

  // Make a new Arm_input_section object.
  Arm_input_section<big_endian>*
  new_arm_input_section(Relobj*, unsigned int);

  // Find the Arm_input_section object corresponding to the SHNDX-th input
  // section of RELOBJ.
  Arm_input_section<big_endian>*
  find_arm_input_section(Relobj* relobj, unsigned int shndx) const;

  // Make a new Stub_table
  Stub_table<big_endian>*
  new_stub_table(Arm_input_section<big_endian>*);

  // Scan a section for stub generation.
  void
  scan_section_for_stubs(const Relocate_info<32, big_endian>*, unsigned int,
			 const unsigned char*, size_t, Output_section*,
			 bool, const unsigned char*, Arm_address,
			 section_size_type);

  // Get the default ARM target.
  static const Target_arm<big_endian>&
  default_target()
  {
    gold_assert(parameters->target().machine_code() == elfcpp::EM_ARM
		&& parameters->target().is_big_endian() == big_endian);
    return static_cast<const Target_arm<big_endian>&>(parameters->target());
  }

  // Whether relocation type uses LSB to distinguish THUMB addresses.
  static bool
  reloc_uses_thumb_bit(unsigned int r_type);

 protected:
  // Make an ELF object.
  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		     const elfcpp::Ehdr<32, big_endian>& ehdr);

  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		     const elfcpp::Ehdr<32, !big_endian>&)
  { gold_unreachable(); }

  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		      const elfcpp::Ehdr<64, false>&)
  { gold_unreachable(); }

  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		     const elfcpp::Ehdr<64, true>&)
  { gold_unreachable(); }

  // Make an output section.
  Output_section*
  do_make_output_section(const char* name, elfcpp::Elf_Word type,
			 elfcpp::Elf_Xword flags)
  { return new Arm_output_section<big_endian>(name, type, flags); }

  void
  do_adjust_elf_header(unsigned char* view, int len) const;

  // We only need to generate stubs, and hence perform relaxation if we are
  // not doing relocatable linking.
  bool
  do_may_relax() const
  { return !parameters->options().relocatable(); }

  bool
  do_relax(int, const Input_objects*, Symbol_table*, Layout*);

 private:
  // The class which scans relocations.
  class Scan
  {
   public:
    Scan()
      : issued_non_pic_error_(false)
    { }

    inline void
    local(Symbol_table* symtab, Layout* layout, Target_arm* target,
	  Sized_relobj<32, big_endian>* object,
	  unsigned int data_shndx,
	  Output_section* output_section,
	  const elfcpp::Rel<32, big_endian>& reloc, unsigned int r_type,
	  const elfcpp::Sym<32, big_endian>& lsym);

    inline void
    global(Symbol_table* symtab, Layout* layout, Target_arm* target,
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
	     unsigned char*, Arm_address,
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

  // Whether two EABI versions are compatible.
  static bool
  are_eabi_versions_compatible(elfcpp::Elf_Word v1, elfcpp::Elf_Word v2);

  // Merge processor-specific flags from input object and those in the ELF
  // header of the output.
  void
  merge_processor_specific_flags(const std::string&, elfcpp::Elf_Word);

  //
  // Methods to support stub-generations.
  //

  // Group input sections for stub generation.
  void
  group_sections(Layout*, section_size_type, bool);

  // Scan a relocation for stub generation.
  void
  scan_reloc_for_stub(const Relocate_info<32, big_endian>*, unsigned int,
		      const Sized_symbol<32>*, unsigned int,
		      const Symbol_value<32>*,
		      elfcpp::Elf_types<32>::Elf_Swxword, Arm_address);

  // Scan a relocation section for stub.
  template<int sh_type>
  void
  scan_reloc_section_for_stubs(
      const Relocate_info<32, big_endian>* relinfo,
      const unsigned char* prelocs,
      size_t reloc_count,
      Output_section* output_section,
      bool needs_special_offset_handling,
      const unsigned char* view,
      elfcpp::Elf_types<32>::Elf_Addr view_address,
      section_size_type);

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info arm_info;

  // The types of GOT entries needed for this platform.
  enum Got_type
  {
    GOT_TYPE_STANDARD = 0	// GOT entry for a regular symbol
  };

  typedef typename std::vector<Stub_table<big_endian>*> Stub_table_list;

  // Map input section to Arm_input_section.
  typedef Unordered_map<Input_section_specifier,
			Arm_input_section<big_endian>*,
			Input_section_specifier::hash,
			Input_section_specifier::equal_to>
	  Arm_input_section_map;
    
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
  // Vector of Stub_tables created.
  Stub_table_list stub_tables_;
  // Stub factory.
  const Stub_factory &stub_factory_;
  // Whether we can use BLX.
  bool may_use_blx_;
  // Whether we force PIC branch veneers.
  bool should_force_pic_veneer_;
  // Map for locating Arm_input_sections.
  Arm_input_section_map arm_input_section_map_;
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
		    Arm_address address,
		    Arm_address thumb_bit)
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
    Valtype x = (psymval->value(object, addend) | thumb_bit) - address;

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
    Reltype x = psymval->value(object, addend);
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
    Reltype x = psymval->value(object, addend);
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
    Reltype x = psymval->value(object, addend);
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
    Reltype x = psymval->value(object, addend);
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
	Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype addend = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype x = psymval->value(object, addend) | thumb_bit;
    elfcpp::Swap<32, big_endian>::writeval(wv, x);
    return This::STATUS_OKAY;
  }

  // R_ARM_REL32: (S + A) | T - P
  static inline typename This::Status
  rel32(unsigned char *view,
	const Sized_relobj<32, big_endian>* object,
	const Symbol_value<32>* psymval,
	Arm_address address,
	Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype addend = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype x = (psymval->value(object, addend) | thumb_bit) - address;
    elfcpp::Swap<32, big_endian>::writeval(wv, x);
    return This::STATUS_OKAY;
  }

  // R_ARM_THM_CALL: (S + A) | T - P
  static inline typename This::Status
  thm_call(unsigned char *view,
	   const Sized_relobj<32, big_endian>* object,
	   const Symbol_value<32>* psymval,
	   Arm_address address,
	   Arm_address thumb_bit)
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
    Reltype x = (psymval->value(object, addend) | thumb_bit) - address;

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
	    Arm_address origin,
	    Arm_address address)
  {
    Base::rel32(view, origin - address);
    return STATUS_OKAY;
  }

  // R_ARM_BASE_ABS: B(S) + A
  static inline typename This::Status
  base_abs(unsigned char* view,
	    Arm_address origin)
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
	   Arm_address address)
  {
    Base::rel32(view, got_offset - address);
    return This::STATUS_OKAY;
  }

  // R_ARM_PLT32: (S + A) | T - P
  static inline typename This::Status
  plt32(unsigned char *view,
	const Sized_relobj<32, big_endian>* object,
	const Symbol_value<32>* psymval,
	Arm_address address,
	Arm_address thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_PLT32>(view, object, psymval,
						  address, thumb_bit);
  }

  // R_ARM_CALL: (S + A) | T - P
  static inline typename This::Status
  call(unsigned char *view,
       const Sized_relobj<32, big_endian>* object,
       const Symbol_value<32>* psymval,
       Arm_address address,
       Arm_address thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_CALL>(view, object, psymval,
						 address, thumb_bit);
  }

  // R_ARM_JUMP24: (S + A) | T - P
  static inline typename This::Status
  jump24(unsigned char *view,
	 const Sized_relobj<32, big_endian>* object,
	 const Symbol_value<32>* psymval,
	 Arm_address address,
	 Arm_address thumb_bit)
  {
    return arm_branch_common<elfcpp::R_ARM_JUMP24>(view, object, psymval,
						   address, thumb_bit);
  }

  // R_ARM_PREL: (S + A) | T - P
  static inline typename This::Status
  prel31(unsigned char *view,
	 const Sized_relobj<32, big_endian>* object,
	 const Symbol_value<32>* psymval,
	 Arm_address address,
	 Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = utils::sign_extend<31>(val);
    Valtype x = (psymval->value(object, addend) | thumb_bit) - address;
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
	      Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend =  This::extract_arm_movw_movt_addend(val);
    Valtype x = psymval->value(object, addend) | thumb_bit;
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
    Valtype x = psymval->value(object, addend) >> 16;
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  //  R_ARM_THM_MOVW_ABS_NC: S + A | T
  static inline typename This::Status 
  thm_movw_abs_nc(unsigned char *view,
	          const Sized_relobj<32, big_endian>* object,
	          const Symbol_value<32>* psymval,
	          Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = ((elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		   | elfcpp::Swap<16, big_endian>::readval(wv + 1));
    Reltype addend = extract_thumb_movw_movt_addend(val);
    Reltype x = psymval->value(object, addend) | thumb_bit;
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
    Reltype x = psymval->value(object, addend) >> 16;
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
	       Arm_address address,
	       Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = This::extract_arm_movw_movt_addend(val);
    Valtype x = (psymval->value(object, addend) | thumb_bit) - address;
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  // R_ARM_MOVT_PREL: S + A - P
  static inline typename This::Status
  movt_prel(unsigned char *view,
	    const Sized_relobj<32, big_endian>* object,
	    const Symbol_value<32>* psymval,
            Arm_address address)
  {
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
    Valtype addend = This::extract_arm_movw_movt_addend(val);
    Valtype x = (psymval->value(object, addend) - address) >> 16;
    val = This::insert_val_arm_movw_movt(val, x);
    elfcpp::Swap<32, big_endian>::writeval(wv, val);
    return This::STATUS_OKAY;
  }

  // R_ARM_THM_MOVW_PREL_NC: (S + A) | T - P
  static inline typename This::Status
  thm_movw_prel_nc(unsigned char *view,
	           const Sized_relobj<32, big_endian>* object,
	           const Symbol_value<32>* psymval,
	           Arm_address address,
	           Arm_address thumb_bit)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = (elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		  | elfcpp::Swap<16, big_endian>::readval(wv + 1);
    Reltype addend = This::extract_thumb_movw_movt_addend(val);
    Reltype x = (psymval->value(object, addend) | thumb_bit) - address;
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
	        Arm_address address)
  {
    typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
    typedef typename elfcpp::Swap<32, big_endian>::Valtype Reltype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Reltype val = (elfcpp::Swap<16, big_endian>::readval(wv) << 16)
		  | elfcpp::Swap<16, big_endian>::readval(wv + 1);
    Reltype addend = This::extract_thumb_movw_movt_addend(val);
    Reltype x = (psymval->value(object, addend) - address) >> 16;
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
					   this->got_, false);
      os->set_is_relro();

      // The old GNU linker creates a .got.plt section.  We just
      // create another set of data in the .got section.  Note that we
      // always create a PLT if we create a GOT, although the PLT
      // might be empty.
      this->got_plt_ = new Output_data_space(4, "** GOT PLT");
      os = layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
					   (elfcpp::SHF_ALLOC
					    | elfcpp::SHF_WRITE),
					   this->got_plt_, false);
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
				      elfcpp::SHF_ALLOC, this->rel_dyn_, true);
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

// Stub_table methods.

// Add a STUB with using KEY.  Caller is reponsible for avoid adding
// if already a STUB with the same key has been added. 

template<bool big_endian>
void
Stub_table<big_endian>::add_reloc_stub(
    Reloc_stub* stub,
    const Reloc_stub::Key& key)
{
  const Stub_template* stub_template = stub->stub_template();
  gold_assert(stub_template->type() == key.stub_type());
  this->reloc_stubs_[key] = stub;
  if (this->addralign_ < stub_template->alignment())
    this->addralign_ = stub_template->alignment();
  this->has_been_changed_ = true;
}

template<bool big_endian>
void
Stub_table<big_endian>::relocate_stubs(
    const Relocate_info<32, big_endian>* relinfo,
    Target_arm<big_endian>* arm_target,
    Output_section* output_section,
    unsigned char* view,
    Arm_address address,
    section_size_type view_size)
{
  // If we are passed a view bigger than the stub table's.  we need to
  // adjust the view.
  gold_assert(address == this->address()
	      && (view_size
		  == static_cast<section_size_type>(this->data_size())));

  for (typename Reloc_stub_map::const_iterator p = this->reloc_stubs_.begin();
      p != this->reloc_stubs_.end();
      ++p)
    {
      Reloc_stub* stub = p->second;
      const Stub_template* stub_template = stub->stub_template();
      if (stub_template->reloc_count() != 0)
	{
	  // Adjust view to cover the stub only.
	  section_size_type offset = stub->offset();
	  section_size_type stub_size = stub_template->size();
	  gold_assert(offset + stub_size <= view_size);

	  arm_target->relocate_stub(stub, relinfo, output_section,
				    view + offset, address + offset,
				    stub_size);
	}
    }
}

// Reset address and file offset.

template<bool big_endian>
void
Stub_table<big_endian>::do_reset_address_and_file_offset()
{
  off_t off = 0;
  uint64_t max_addralign = 1;
  for (typename Reloc_stub_map::const_iterator p = this->reloc_stubs_.begin();
      p != this->reloc_stubs_.end();
      ++p)
    {
      Reloc_stub* stub = p->second;
      const Stub_template* stub_template = stub->stub_template();
      uint64_t stub_addralign = stub_template->alignment();
      max_addralign = std::max(max_addralign, stub_addralign);
      off = align_address(off, stub_addralign);
      stub->set_offset(off);
      stub->reset_destination_address();
      off += stub_template->size();
    }

  this->addralign_ = max_addralign;
  this->set_current_data_size_for_child(off);
}

// Write out the stubs to file.

template<bool big_endian>
void
Stub_table<big_endian>::do_write(Output_file* of)
{
  off_t offset = this->offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size());
  unsigned char* const oview = of->get_output_view(offset, oview_size);

  for (typename Reloc_stub_map::const_iterator p = this->reloc_stubs_.begin();
      p != this->reloc_stubs_.end();
      ++p)
    {
      Reloc_stub* stub = p->second;
      Arm_address address = this->address() + stub->offset();
      gold_assert(address
		  == align_address(address,
				   stub->stub_template()->alignment()));
      stub->write(oview + stub->offset(), stub->stub_template()->size(),
		  big_endian);
    } 
  of->write_output_view(this->offset(), oview_size, oview);
}

// Arm_input_section methods.

// Initialize an Arm_input_section.

template<bool big_endian>
void
Arm_input_section<big_endian>::init()
{
  Relobj* relobj = this->relobj();
  unsigned int shndx = this->shndx();

  // Cache these to speed up size and alignment queries.  It is too slow
  // to call section_addraglin and section_size every time.
  this->original_addralign_ = relobj->section_addralign(shndx);
  this->original_size_ = relobj->section_size(shndx);

  // We want to make this look like the original input section after
  // output sections are finalized.
  Output_section* os = relobj->output_section(shndx);
  off_t offset = relobj->output_section_offset(shndx);
  gold_assert(os != NULL && !relobj->is_output_section_offset_invalid(shndx));
  this->set_address(os->address() + offset);
  this->set_file_offset(os->offset() + offset);

  this->set_current_data_size(this->original_size_);
  this->finalize_data_size();
}

template<bool big_endian>
void
Arm_input_section<big_endian>::do_write(Output_file* of)
{
  // We have to write out the original section content.
  section_size_type section_size;
  const unsigned char* section_contents =
    this->relobj()->section_contents(this->shndx(), &section_size, false); 
  of->write(this->offset(), section_contents, section_size); 

  // If this owns a stub table and it is not empty, write it.
  if (this->is_stub_table_owner() && !this->stub_table_->empty())
    this->stub_table_->write(of);
}

// Finalize data size.

template<bool big_endian>
void
Arm_input_section<big_endian>::set_final_data_size()
{
  // If this owns a stub table, finalize its data size as well.
  if (this->is_stub_table_owner())
    {
      uint64_t address = this->address();

      // The stub table comes after the original section contents.
      address += this->original_size_;
      address = align_address(address, this->stub_table_->addralign());
      off_t offset = this->offset() + (address - this->address());
      this->stub_table_->set_address_and_file_offset(address, offset);
      address += this->stub_table_->data_size();
      gold_assert(address == this->address() + this->current_data_size());
    }

  this->set_data_size(this->current_data_size());
}

// Reset address and file offset.

template<bool big_endian>
void
Arm_input_section<big_endian>::do_reset_address_and_file_offset()
{
  // Size of the original input section contents.
  off_t off = convert_types<off_t, uint64_t>(this->original_size_);

  // If this is a stub table owner, account for the stub table size.
  if (this->is_stub_table_owner())
    {
      Stub_table<big_endian>* stub_table = this->stub_table_;

      // Reset the stub table's address and file offset.  The
      // current data size for child will be updated after that.
      stub_table_->reset_address_and_file_offset();
      off = align_address(off, stub_table_->addralign());
      off += stub_table->current_data_size();
    }

  this->set_current_data_size(off);
}

// Arm_output_section methods.

// Create a stub group for input sections from BEGIN to END.  OWNER
// points to the input section to be the owner a new stub table.

template<bool big_endian>
void
Arm_output_section<big_endian>::create_stub_group(
  Input_section_list::const_iterator begin,
  Input_section_list::const_iterator end,
  Input_section_list::const_iterator owner,
  Target_arm<big_endian>* target,
  std::vector<Output_relaxed_input_section*>* new_relaxed_sections)
{
  // Currently we convert ordinary input sections into relaxed sections only
  // at this point but we may want to support creating relaxed input section
  // very early.  So we check here to see if owner is already a relaxed
  // section.
  
  Arm_input_section<big_endian>* arm_input_section;
  if (owner->is_relaxed_input_section())
    {
      arm_input_section =
	Arm_input_section<big_endian>::as_arm_input_section(
	  owner->relaxed_input_section());
    }
  else
    {
      gold_assert(owner->is_input_section());
      // Create a new relaxed input section.
      arm_input_section =
	target->new_arm_input_section(owner->relobj(), owner->shndx());
      new_relaxed_sections->push_back(arm_input_section);
    }

  // Create a stub table.
  Stub_table<big_endian>* stub_table =
    target->new_stub_table(arm_input_section);

  arm_input_section->set_stub_table(stub_table);
  
  Input_section_list::const_iterator p = begin;
  Input_section_list::const_iterator prev_p;

  // Look for input sections or relaxed input sections in [begin ... end].
  do
    {
      if (p->is_input_section() || p->is_relaxed_input_section())
	{
	  // The stub table information for input sections live
	  // in their objects.
	  Arm_relobj<big_endian>* arm_relobj =
	    Arm_relobj<big_endian>::as_arm_relobj(p->relobj());
	  arm_relobj->set_stub_table(p->shndx(), stub_table);
	}
      prev_p = p++;
    }
  while (prev_p != end);
}

// Group input sections for stub generation.  GROUP_SIZE is roughly the limit
// of stub groups.  We grow a stub group by adding input section until the
// size is just below GROUP_SIZE.  The last input section will be converted
// into a stub table.  If STUB_ALWAYS_AFTER_BRANCH is false, we also add
// input section after the stub table, effectively double the group size.
// 
// This is similar to the group_sections() function in elf32-arm.c but is
// implemented differently.

template<bool big_endian>
void
Arm_output_section<big_endian>::group_sections(
    section_size_type group_size,
    bool stubs_always_after_branch,
    Target_arm<big_endian>* target)
{
  // We only care about sections containing code.
  if ((this->flags() & elfcpp::SHF_EXECINSTR) == 0)
    return;

  // States for grouping.
  typedef enum
  {
    // No group is being built.
    NO_GROUP,
    // A group is being built but the stub table is not found yet.
    // We keep group a stub group until the size is just under GROUP_SIZE.
    // The last input section in the group will be used as the stub table.
    FINDING_STUB_SECTION,
    // A group is being built and we have already found a stub table.
    // We enter this state to grow a stub group by adding input section
    // after the stub table.  This effectively doubles the group size.
    HAS_STUB_SECTION
  } State;

  // Any newly created relaxed sections are stored here.
  std::vector<Output_relaxed_input_section*> new_relaxed_sections;

  State state = NO_GROUP;
  section_size_type off = 0;
  section_size_type group_begin_offset = 0;
  section_size_type group_end_offset = 0;
  section_size_type stub_table_end_offset = 0;
  Input_section_list::const_iterator group_begin =
    this->input_sections().end();
  Input_section_list::const_iterator stub_table =
    this->input_sections().end();
  Input_section_list::const_iterator group_end = this->input_sections().end();
  for (Input_section_list::const_iterator p = this->input_sections().begin();
       p != this->input_sections().end();
       ++p)
    {
      section_size_type section_begin_offset =
	align_address(off, p->addralign());
      section_size_type section_end_offset =
	section_begin_offset + p->data_size(); 
      
      // Check to see if we should group the previously seens sections.
      switch (state)
	{
	case NO_GROUP:
	  break;

	case FINDING_STUB_SECTION:
	  // Adding this section makes the group larger than GROUP_SIZE.
	  if (section_end_offset - group_begin_offset >= group_size)
	    {
	      if (stubs_always_after_branch)
		{	
		  gold_assert(group_end != this->input_sections().end());
		  this->create_stub_group(group_begin, group_end, group_end,
					  target, &new_relaxed_sections);
		  state = NO_GROUP;
		}
	      else
		{
		  // But wait, there's more!  Input sections up to
		  // stub_group_size bytes after the stub table can be
		  // handled by it too.
		  state = HAS_STUB_SECTION;
		  stub_table = group_end;
		  stub_table_end_offset = group_end_offset;
		}
	    }
	    break;

	case HAS_STUB_SECTION:
	  // Adding this section makes the post stub-section group larger
	  // than GROUP_SIZE.
	  if (section_end_offset - stub_table_end_offset >= group_size)
	   {
	     gold_assert(group_end != this->input_sections().end());
	     this->create_stub_group(group_begin, group_end, stub_table,
				     target, &new_relaxed_sections);
	     state = NO_GROUP;
	   }
	   break;

	  default:
	    gold_unreachable();
	}	

      // If we see an input section and currently there is no group, start
      // a new one.  Skip any empty sections.
      if ((p->is_input_section() || p->is_relaxed_input_section())
	  && (p->relobj()->section_size(p->shndx()) != 0))
	{
	  if (state == NO_GROUP)
	    {
	      state = FINDING_STUB_SECTION;
	      group_begin = p;
	      group_begin_offset = section_begin_offset;
	    }

	  // Keep track of the last input section seen.
	  group_end = p;
	  group_end_offset = section_end_offset;
	}

      off = section_end_offset;
    }

  // Create a stub group for any ungrouped sections.
  if (state == FINDING_STUB_SECTION || state == HAS_STUB_SECTION)
    {
      gold_assert(group_end != this->input_sections().end());
      this->create_stub_group(group_begin, group_end,
			      (state == FINDING_STUB_SECTION
			       ? group_end
			       : stub_table),
			       target, &new_relaxed_sections);
    }

  // Convert input section into relaxed input section in a batch.
  if (!new_relaxed_sections.empty())
    this->convert_input_sections_to_relaxed_sections(new_relaxed_sections);

  // Update the section offsets
  for (size_t i = 0; i < new_relaxed_sections.size(); ++i)
    {
      Arm_relobj<big_endian>* arm_relobj =
	Arm_relobj<big_endian>::as_arm_relobj(
	  new_relaxed_sections[i]->relobj());
      unsigned int shndx = new_relaxed_sections[i]->shndx();
      // Tell Arm_relobj that this input section is converted.
      arm_relobj->convert_input_section_to_relaxed_section(shndx);
    }
}

// Arm_relobj methods.

// Scan relocations for stub generation.

template<bool big_endian>
void
Arm_relobj<big_endian>::scan_sections_for_stubs(
    Target_arm<big_endian>* arm_target,
    const Symbol_table* symtab,
    const Layout* layout)
{
  unsigned int shnum = this->shnum();
  const unsigned int shdr_size = elfcpp::Elf_sizes<32>::shdr_size;

  // Read the section headers.
  const unsigned char* pshdrs = this->get_view(this->elf_file()->shoff(),
					       shnum * shdr_size,
					       true, true);

  // To speed up processing, we set up hash tables for fast lookup of
  // input offsets to output addresses.
  this->initialize_input_to_output_maps();

  const Relobj::Output_sections& out_sections(this->output_sections());

  Relocate_info<32, big_endian> relinfo;
  relinfo.symtab = symtab;
  relinfo.layout = layout;
  relinfo.object = this;

  const unsigned char* p = pshdrs + shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, p += shdr_size)
    {
      typename elfcpp::Shdr<32, big_endian> shdr(p);

      unsigned int sh_type = shdr.get_sh_type();
      if (sh_type != elfcpp::SHT_REL && sh_type != elfcpp::SHT_RELA)
	continue;

      off_t sh_size = shdr.get_sh_size();
      if (sh_size == 0)
	continue;

      unsigned int index = this->adjust_shndx(shdr.get_sh_info());
      if (index >= this->shnum())
	{
	  // Ignore reloc section with bad info.  This error will be
	  // reported in the final link.
	  continue;
	}

      Output_section* os = out_sections[index];
      if (os == NULL)
	{
	  // This relocation section is against a section which we
	  // discarded.
	  continue;
	}
      Arm_address output_offset = this->get_output_section_offset(index);

      if (this->adjust_shndx(shdr.get_sh_link()) != this->symtab_shndx())
	{
	  // Ignore reloc section with unexpected symbol table.  The
	  // error will be reported in the final link.
	  continue;
	}

      const unsigned char* prelocs = this->get_view(shdr.get_sh_offset(),
						    sh_size, true, false);

      unsigned int reloc_size;
      if (sh_type == elfcpp::SHT_REL)
	reloc_size = elfcpp::Elf_sizes<32>::rel_size;
      else
	reloc_size = elfcpp::Elf_sizes<32>::rela_size;

      if (reloc_size != shdr.get_sh_entsize())
	{
	  // Ignore reloc section with unexpected entsize.  The error
	  // will be reported in the final link.
	  continue;
	}

      size_t reloc_count = sh_size / reloc_size;
      if (static_cast<off_t>(reloc_count * reloc_size) != sh_size)
	{
	  // Ignore reloc section with uneven size.  The error will be
	  // reported in the final link.
	  continue;
	}

      gold_assert(output_offset != invalid_address
		  || this->relocs_must_follow_section_writes());

      // Get the section contents.  This does work for the case in which
      // we modify the contents of an input section.  We need to pass the
      // output view under such circumstances.
      section_size_type input_view_size = 0;
      const unsigned char* input_view =
	this->section_contents(index, &input_view_size, false);

      relinfo.reloc_shndx = i;
      relinfo.data_shndx = index;
      arm_target->scan_section_for_stubs(&relinfo, sh_type, prelocs,
					 reloc_count, os,
					 output_offset == invalid_address,
					 input_view,
					 os->address(),
					 input_view_size);
    }

  // After we've done the relocations, we release the hash tables,
  // since we no longer need them.
  this->free_input_to_output_maps();
}

// Count the local symbols.  The ARM backend needs to know if a symbol
// is a THUMB function or not.  For global symbols, it is easy because
// the Symbol object keeps the ELF symbol type.  For local symbol it is
// harder because we cannot access this information.   So we override the
// do_count_local_symbol in parent and scan local symbols to mark
// THUMB functions.  This is not the most efficient way but I do not want to
// slow down other ports by calling a per symbol targer hook inside
// Sized_relobj<size, big_endian>::do_count_local_symbols. 

template<bool big_endian>
void
Arm_relobj<big_endian>::do_count_local_symbols(
    Stringpool_template<char>* pool,
    Stringpool_template<char>* dynpool)
{
  // We need to fix-up the values of any local symbols whose type are
  // STT_ARM_TFUNC.
  
  // Ask parent to count the local symbols.
  Sized_relobj<32, big_endian>::do_count_local_symbols(pool, dynpool);
  const unsigned int loccount = this->local_symbol_count();
  if (loccount == 0)
    return;

  // Intialize the thumb function bit-vector.
  std::vector<bool> empty_vector(loccount, false);
  this->local_symbol_is_thumb_function_.swap(empty_vector);

  // Read the symbol table section header.
  const unsigned int symtab_shndx = this->symtab_shndx();
  elfcpp::Shdr<32, big_endian>
      symtabshdr(this, this->elf_file()->section_header(symtab_shndx));
  gold_assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);

  // Read the local symbols.
  const int sym_size =elfcpp::Elf_sizes<32>::sym_size;
  gold_assert(loccount == symtabshdr.get_sh_info());
  off_t locsize = loccount * sym_size;
  const unsigned char* psyms = this->get_view(symtabshdr.get_sh_offset(),
					      locsize, true, true);

  // Loop over the local symbols and mark any local symbols pointing
  // to THUMB functions.

  // Skip the first dummy symbol.
  psyms += sym_size;
  typename Sized_relobj<32, big_endian>::Local_values* plocal_values =
    this->local_values();
  for (unsigned int i = 1; i < loccount; ++i, psyms += sym_size)
    {
      elfcpp::Sym<32, big_endian> sym(psyms);
      elfcpp::STT st_type = sym.get_st_type();
      Symbol_value<32>& lv((*plocal_values)[i]);
      Arm_address input_value = lv.input_value();

      if (st_type == elfcpp::STT_ARM_TFUNC
	  || (st_type == elfcpp::STT_FUNC && ((input_value & 1) != 0)))
	{
	  // This is a THUMB function.  Mark this and canonicalize the
	  // symbol value by setting LSB.
	  this->local_symbol_is_thumb_function_[i] = true;
	  if ((input_value & 1) == 0)
	    lv.set_input_value(input_value | 1);
	}
    }
}

// Relocate sections.
template<bool big_endian>
void
Arm_relobj<big_endian>::do_relocate_sections(
    const General_options& options,
    const Symbol_table* symtab,
    const Layout* layout,
    const unsigned char* pshdrs,
    typename Sized_relobj<32, big_endian>::Views* pviews)
{
  // Call parent to relocate sections.
  Sized_relobj<32, big_endian>::do_relocate_sections(options, symtab, layout,
						     pshdrs, pviews); 

  // We do not generate stubs if doing a relocatable link.
  if (parameters->options().relocatable())
    return;

  // Relocate stub tables.
  unsigned int shnum = this->shnum();

  Target_arm<big_endian>* arm_target =
    Target_arm<big_endian>::default_target();

  Relocate_info<32, big_endian> relinfo;
  relinfo.options = &options;
  relinfo.symtab = symtab;
  relinfo.layout = layout;
  relinfo.object = this;

  for (unsigned int i = 1; i < shnum; ++i)
    {
      Arm_input_section<big_endian>* arm_input_section =
	arm_target->find_arm_input_section(this, i);

      if (arm_input_section == NULL
	  || !arm_input_section->is_stub_table_owner()
	  || arm_input_section->stub_table()->empty())
	continue;

      // We cannot discard a section if it owns a stub table.
      Output_section* os = this->output_section(i);
      gold_assert(os != NULL);

      relinfo.reloc_shndx = elfcpp::SHN_UNDEF;
      relinfo.reloc_shdr = NULL;
      relinfo.data_shndx = i;
      relinfo.data_shdr = pshdrs + i * elfcpp::Elf_sizes<32>::shdr_size;

      gold_assert((*pviews)[i].view != NULL);

      // We are passed the output section view.  Adjust it to cover the
      // stub table only.
      Stub_table<big_endian>* stub_table = arm_input_section->stub_table();
      gold_assert((stub_table->address() >= (*pviews)[i].address)
		  && ((stub_table->address() + stub_table->data_size())
		      <= (*pviews)[i].address + (*pviews)[i].view_size));

      off_t offset = stub_table->address() - (*pviews)[i].address;
      unsigned char* view = (*pviews)[i].view + offset;
      Arm_address address = stub_table->address();
      section_size_type view_size = stub_table->data_size();
 
      stub_table->relocate_stubs(&relinfo, arm_target, os, view, address,
				 view_size);
    }
}

// Read the symbol information.

template<bool big_endian>
void
Arm_relobj<big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  // Call parent class to read symbol information.
  Sized_relobj<32, big_endian>::do_read_symbols(sd);

  // Read processor-specific flags in ELF file header.
  const unsigned char* pehdr = this->get_view(elfcpp::file_header_offset,
					      elfcpp::Elf_sizes<32>::ehdr_size,
					      true, false);
  elfcpp::Ehdr<32, big_endian> ehdr(pehdr);
  this->processor_specific_flags_ = ehdr.get_e_flags();
}

// Arm_dynobj methods.

// Read the symbol information.

template<bool big_endian>
void
Arm_dynobj<big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  // Call parent class to read symbol information.
  Sized_dynobj<32, big_endian>::do_read_symbols(sd);

  // Read processor-specific flags in ELF file header.
  const unsigned char* pehdr = this->get_view(elfcpp::file_header_offset,
					      elfcpp::Elf_sizes<32>::ehdr_size,
					      true, false);
  elfcpp::Ehdr<32, big_endian> ehdr(pehdr);
  this->processor_specific_flags_ = ehdr.get_e_flags();
}

// Stub_addend_reader methods.

// Read the addend of a REL relocation of type R_TYPE at VIEW.

template<bool big_endian>
elfcpp::Elf_types<32>::Elf_Swxword
Stub_addend_reader<elfcpp::SHT_REL, big_endian>::operator()(
    unsigned int r_type,
    const unsigned char* view,
    const typename Reloc_types<elfcpp::SHT_REL, 32, big_endian>::Reloc&) const
{
  switch (r_type)
    {
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_PLT32:
      {
	typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
	const Valtype* wv = reinterpret_cast<const Valtype*>(view);
	Valtype val = elfcpp::Swap<32, big_endian>::readval(wv);
	return utils::sign_extend<26>(val << 2);
      }

    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_THM_JUMP24:
    case elfcpp::R_ARM_THM_XPC22:
      {
	// Fetch the addend.  We use the Thumb-2 encoding (backwards
	// compatible with Thumb-1) involving the J1 and J2 bits.
	typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
	const Valtype* wv = reinterpret_cast<const Valtype*>(view);
	Valtype upper_insn = elfcpp::Swap<16, big_endian>::readval(wv);
	Valtype lower_insn = elfcpp::Swap<16, big_endian>::readval(wv + 1);

	uint32_t s = (upper_insn & (1 << 10)) >> 10;
	uint32_t upper = upper_insn & 0x3ff;
	uint32_t lower = lower_insn & 0x7ff;
	uint32_t j1 = (lower_insn & (1 << 13)) >> 13;
	uint32_t j2 = (lower_insn & (1 << 11)) >> 11;
	uint32_t i1 = j1 ^ s ? 0 : 1;
	uint32_t i2 = j2 ^ s ? 0 : 1;

	return utils::sign_extend<25>((s << 24) | (i1 << 23) | (i2 << 22)
				      | (upper << 12) | (lower << 1));
      }

    case elfcpp::R_ARM_THM_JUMP19:
      {
	typedef typename elfcpp::Swap<16, big_endian>::Valtype Valtype;
	const Valtype* wv = reinterpret_cast<const Valtype*>(view);
	Valtype upper_insn = elfcpp::Swap<16, big_endian>::readval(wv);
	Valtype lower_insn = elfcpp::Swap<16, big_endian>::readval(wv + 1);

	// Reconstruct the top three bits and squish the two 11 bit pieces
	// together.
	uint32_t S = (upper_insn & 0x0400) >> 10;
	uint32_t J1 = (lower_insn & 0x2000) >> 13;
	uint32_t J2 = (lower_insn & 0x0800) >> 11;
	uint32_t upper =
	  (S << 8) | (J2 << 7) | (J1 << 6) | (upper_insn & 0x003f);
	uint32_t lower = (lower_insn & 0x07ff);
	return utils::sign_extend<23>((upper << 12) | (lower << 1));
      }

    default:
      gold_unreachable();
    }
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
				  elfcpp::SHF_ALLOC, this->rel_, true);
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

  Arm_address plt_address = this->address();
  Arm_address got_address = this->got_plt_->address();

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
				      this->plt_, false);
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
Target_arm<big_endian>::Scan::local(Symbol_table* symtab,
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
Target_arm<big_endian>::Scan::global(Symbol_table* symtab,
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
Target_arm<big_endian>::gc_process_relocs(Symbol_table* symtab,
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
Target_arm<big_endian>::scan_relocs(Symbol_table* symtab,
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
Target_arm<big_endian>::do_finalize_sections(
    Layout* layout,
    const Input_objects* input_objects)
{
  // Merge processor-specific flags.
  for (Input_objects::Relobj_iterator p = input_objects->relobj_begin();
       p != input_objects->relobj_end();
       ++p)
    {
      Arm_relobj<big_endian>* arm_relobj =
	Arm_relobj<big_endian>::as_arm_relobj(*p);
      this->merge_processor_specific_flags(
	  arm_relobj->name(),
	  arm_relobj->processor_specific_flags());
    } 

  for (Input_objects::Dynobj_iterator p = input_objects->dynobj_begin();
       p != input_objects->dynobj_end();
       ++p)
    {
      Arm_dynobj<big_endian>* arm_dynobj =
	Arm_dynobj<big_endian>::as_arm_dynobj(*p);
      this->merge_processor_specific_flags(
	  arm_dynobj->name(),
	  arm_dynobj->processor_specific_flags());
    }

  // Fill in some more dynamic tags.
  Output_data_dynamic* const odyn = layout->dynamic_data();
  if (odyn != NULL)
    {
      if (this->got_plt_ != NULL
	  && this->got_plt_->output_section() != NULL)
	odyn->add_section_address(elfcpp::DT_PLTGOT, this->got_plt_);

      if (this->plt_ != NULL
	  && this->plt_->output_section() != NULL)
	{
	  const Output_data* od = this->plt_->rel_plt();
	  odyn->add_section_size(elfcpp::DT_PLTRELSZ, od);
	  odyn->add_section_address(elfcpp::DT_JMPREL, od);
	  odyn->add_constant(elfcpp::DT_PLTREL, elfcpp::DT_REL);
	}

      if (this->rel_dyn_ != NULL
	  && this->rel_dyn_->output_section() != NULL)
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
	  exidx_segment->add_output_section(exidx_section, elfcpp::PF_R,
					    false);
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
    Arm_address address,
    section_size_type /* view_size */ )
{
  typedef Arm_relocate_functions<big_endian> Arm_relocate_functions;

  r_type = get_real_reloc_type(r_type);

  const Arm_relobj<big_endian>* object =
    Arm_relobj<big_endian>::as_arm_relobj(relinfo->object);

  // If the final branch target of a relocation is THUMB instruction, this
  // is 1.  Otherwise it is 0.
  Arm_address thumb_bit = 0;
  Symbol_value<32> symval;
  if (relnum != Target_arm<big_endian>::fake_relnum_for_stubs)
    {
      if (gsym != NULL)
	{
	  // This is a global symbol.  Determine if we use PLT and if the
	  // final target is THUMB.
	  if (gsym->use_plt_offset(reloc_is_non_pic(r_type)))
	    {
	      // This uses a PLT, change the symbol value.
	      symval.set_output_value(target->plt_section()->address()
				      + gsym->plt_offset());
	      psymval = &symval;
	    }
	  else
	    {
	      // Set thumb bit if symbol:
	      // -Has type STT_ARM_TFUNC or
	      // -Has type STT_FUNC, is defined and with LSB in value set.
	      thumb_bit =
		(((gsym->type() == elfcpp::STT_ARM_TFUNC)
		 || (gsym->type() == elfcpp::STT_FUNC
		     && !gsym->is_undefined()
		     && ((psymval->value(object, 0) & 1) != 0)))
		? 1
		: 0);
	    }
	}
      else
	{
          // This is a local symbol.  Determine if the final target is THUMB.
          // We saved this information when all the local symbols were read.
	  elfcpp::Elf_types<32>::Elf_WXword r_info = rel.get_r_info();
	  unsigned int r_sym = elfcpp::elf_r_sym<32>(r_info);
	  thumb_bit = object->local_symbol_is_thumb_function(r_sym) ? 1 : 0;
	}
    }
  else
    {
      // This is a fake relocation synthesized for a stub.  It does not have
      // a real symbol.  We just look at the LSB of the symbol value to
      // determine if the target is THUMB or not.
      thumb_bit = ((psymval->value(object, 0) & 1) != 0);
    }

  // Strip LSB if this points to a THUMB target.
  if (thumb_bit != 0
      && Target_arm<big_endian>::reloc_uses_thumb_bit(r_type) 
      && ((psymval->value(object, 0) & 1) != 0))
    {
      Arm_address stripped_value =
	psymval->value(object, 0) & ~static_cast<Arm_address>(1);
      symval.set_output_value(stripped_value);
      psymval = &symval;
    } 

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
						     thumb_bit);
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
       						           thumb_bit);
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
       						               thumb_bit);
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
							  thumb_bit);
      break;

    case elfcpp::R_ARM_MOVT_PREL:
      reloc_status = Arm_relocate_functions::movt_prel(view, object,
                                                       psymval, address);
      break;

    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
      reloc_status = Arm_relocate_functions::thm_movw_prel_nc(view, object,
							      psymval, address,
							      thumb_bit);
      break;

    case elfcpp::R_ARM_THM_MOVT_PREL:
      reloc_status = Arm_relocate_functions::thm_movt_prel(view, object,
							   psymval, address);
      break;
	
    case elfcpp::R_ARM_REL32:
      reloc_status = Arm_relocate_functions::rel32(view, object, psymval,
						   address, thumb_bit);
      break;

    case elfcpp::R_ARM_THM_ABS5:
      if (should_apply_static_reloc(gsym, Symbol::ABSOLUTE_REF, false,
				    output_section))
	reloc_status = Arm_relocate_functions::thm_abs5(view, object, psymval);
      break;

    case elfcpp::R_ARM_THM_CALL:
      reloc_status = Arm_relocate_functions::thm_call(view, object, psymval,
						      address, thumb_bit);
      break;

    case elfcpp::R_ARM_GOTOFF32:
      {
	Arm_address got_origin;
	got_origin = target->got_plt_section()->address();
	reloc_status = Arm_relocate_functions::rel32(view, object, psymval,
						     got_origin, thumb_bit);
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
      Arm_address got_origin;
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
						   address, thumb_bit);
      break;

    case elfcpp::R_ARM_CALL:
      reloc_status = Arm_relocate_functions::call(view, object, psymval,
						  address, thumb_bit);
      break;

    case elfcpp::R_ARM_JUMP24:
      reloc_status = Arm_relocate_functions::jump24(view, object, psymval,
						    address, thumb_bit);
      break;

    case elfcpp::R_ARM_PREL31:
      reloc_status = Arm_relocate_functions::prel31(view, object, psymval,
						    address, thumb_bit);
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
    Arm_address address,
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
    Arm_address view_address,
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

// Whether if two EABI versions V1 and V2 are compatible.

template<bool big_endian>
bool
Target_arm<big_endian>::are_eabi_versions_compatible(
    elfcpp::Elf_Word v1,
    elfcpp::Elf_Word v2)
{
  // v4 and v5 are the same spec before and after it was released,
  // so allow mixing them.
  if ((v1 == elfcpp::EF_ARM_EABI_VER4 && v2 == elfcpp::EF_ARM_EABI_VER5)
      || (v1 == elfcpp::EF_ARM_EABI_VER5 && v2 == elfcpp::EF_ARM_EABI_VER4))
    return true;

  return v1 == v2;
}

// Combine FLAGS from an input object called NAME and the processor-specific
// flags in the ELF header of the output.  Much of this is adapted from the
// processor-specific flags merging code in elf32_arm_merge_private_bfd_data
// in bfd/elf32-arm.c.

template<bool big_endian>
void
Target_arm<big_endian>::merge_processor_specific_flags(
    const std::string& name,
    elfcpp::Elf_Word flags)
{
  if (this->are_processor_specific_flags_set())
    {
      elfcpp::Elf_Word out_flags = this->processor_specific_flags();

      // Nothing to merge if flags equal to those in output.
      if (flags == out_flags)
	return;

      // Complain about various flag mismatches.
      elfcpp::Elf_Word version1 = elfcpp::arm_eabi_version(flags);
      elfcpp::Elf_Word version2 = elfcpp::arm_eabi_version(out_flags);
      if (!this->are_eabi_versions_compatible(version1, version2))
	gold_error(_("Source object %s has EABI version %d but output has "
		     "EABI version %d."),
		   name.c_str(),
		   (flags & elfcpp::EF_ARM_EABIMASK) >> 24,
		   (out_flags & elfcpp::EF_ARM_EABIMASK) >> 24);
    }
  else
    {
      // If the input is the default architecture and had the default
      // flags then do not bother setting the flags for the output
      // architecture, instead allow future merges to do this.  If no
      // future merges ever set these flags then they will retain their
      // uninitialised values, which surprise surprise, correspond
      // to the default values.
      if (flags == 0)
	return;

      // This is the first time, just copy the flags.
      // We only copy the EABI version for now.
      this->set_processor_specific_flags(flags & elfcpp::EF_ARM_EABIMASK);
    }
}

// Adjust ELF file header.
template<bool big_endian>
void
Target_arm<big_endian>::do_adjust_elf_header(
    unsigned char* view,
    int len) const
{
  gold_assert(len == elfcpp::Elf_sizes<32>::ehdr_size);

  elfcpp::Ehdr<32, big_endian> ehdr(view);
  unsigned char e_ident[elfcpp::EI_NIDENT];
  memcpy(e_ident, ehdr.get_e_ident(), elfcpp::EI_NIDENT);

  if (elfcpp::arm_eabi_version(this->processor_specific_flags())
      == elfcpp::EF_ARM_EABI_UNKNOWN)
    e_ident[elfcpp::EI_OSABI] = elfcpp::ELFOSABI_ARM;
  else
    e_ident[elfcpp::EI_OSABI] = 0;
  e_ident[elfcpp::EI_ABIVERSION] = 0;

  // FIXME: Do EF_ARM_BE8 adjustment.

  elfcpp::Ehdr_write<32, big_endian> oehdr(view);
  oehdr.put_e_ident(e_ident);
}

// do_make_elf_object to override the same function in the base class.
// We need to use a target-specific sub-class of Sized_relobj<32, big_endian>
// to store ARM specific information.  Hence we need to have our own
// ELF object creation.

template<bool big_endian>
Object*
Target_arm<big_endian>::do_make_elf_object(
    const std::string& name,
    Input_file* input_file,
    off_t offset, const elfcpp::Ehdr<32, big_endian>& ehdr)
{
  int et = ehdr.get_e_type();
  if (et == elfcpp::ET_REL)
    {
      Arm_relobj<big_endian>* obj =
        new Arm_relobj<big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else if (et == elfcpp::ET_DYN)
    {
      Sized_dynobj<32, big_endian>* obj =
        new Arm_dynobj<big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else
    {
      gold_error(_("%s: unsupported ELF file type %d"),
                 name.c_str(), et);
      return NULL;
    }
}

// Return whether a relocation type used the LSB to distinguish THUMB
// addresses.
template<bool big_endian>
bool
Target_arm<big_endian>::reloc_uses_thumb_bit(unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_ARM_PC24:
    case elfcpp::R_ARM_ABS32:
    case elfcpp::R_ARM_REL32:
    case elfcpp::R_ARM_SBREL32:
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_GLOB_DAT:
    case elfcpp::R_ARM_JUMP_SLOT:
    case elfcpp::R_ARM_GOTOFF32:
    case elfcpp::R_ARM_PLT32:
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_THM_JUMP24:
    case elfcpp::R_ARM_SBREL31:
    case elfcpp::R_ARM_PREL31:
    case elfcpp::R_ARM_MOVW_ABS_NC:
    case elfcpp::R_ARM_MOVW_PREL_NC:
    case elfcpp::R_ARM_THM_MOVW_ABS_NC:
    case elfcpp::R_ARM_THM_MOVW_PREL_NC:
    case elfcpp::R_ARM_THM_JUMP19:
    case elfcpp::R_ARM_THM_ALU_PREL_11_0:
    case elfcpp::R_ARM_ALU_PC_G0_NC:
    case elfcpp::R_ARM_ALU_PC_G0:
    case elfcpp::R_ARM_ALU_PC_G1_NC:
    case elfcpp::R_ARM_ALU_PC_G1:
    case elfcpp::R_ARM_ALU_PC_G2:
    case elfcpp::R_ARM_ALU_SB_G0_NC:
    case elfcpp::R_ARM_ALU_SB_G0:
    case elfcpp::R_ARM_ALU_SB_G1_NC:
    case elfcpp::R_ARM_ALU_SB_G1:
    case elfcpp::R_ARM_ALU_SB_G2:
    case elfcpp::R_ARM_MOVW_BREL_NC:
    case elfcpp::R_ARM_MOVW_BREL:
    case elfcpp::R_ARM_THM_MOVW_BREL_NC:
    case elfcpp::R_ARM_THM_MOVW_BREL:
      return true;
    default:
      return false;
    }
}

// Stub-generation methods for Target_arm.

// Make a new Arm_input_section object.

template<bool big_endian>
Arm_input_section<big_endian>*
Target_arm<big_endian>::new_arm_input_section(
    Relobj* relobj,
    unsigned int shndx)
{
  Input_section_specifier iss(relobj, shndx);

  Arm_input_section<big_endian>* arm_input_section =
    new Arm_input_section<big_endian>(relobj, shndx);
  arm_input_section->init();

  // Register new Arm_input_section in map for look-up.
  std::pair<typename Arm_input_section_map::iterator, bool> ins =
    this->arm_input_section_map_.insert(std::make_pair(iss, arm_input_section));

  // Make sure that it we have not created another Arm_input_section
  // for this input section already.
  gold_assert(ins.second);

  return arm_input_section; 
}

// Find the Arm_input_section object corresponding to the SHNDX-th input
// section of RELOBJ.

template<bool big_endian>
Arm_input_section<big_endian>*
Target_arm<big_endian>::find_arm_input_section(
    Relobj* relobj,
    unsigned int shndx) const
{
  Input_section_specifier iss(relobj, shndx);
  typename Arm_input_section_map::const_iterator p =
    this->arm_input_section_map_.find(iss);
  return (p != this->arm_input_section_map_.end()) ? p->second : NULL;
}

// Make a new stub table.

template<bool big_endian>
Stub_table<big_endian>*
Target_arm<big_endian>::new_stub_table(Arm_input_section<big_endian>* owner)
{
  Stub_table<big_endian>* stub_table =
    new Stub_table<big_endian>(owner);
  this->stub_tables_.push_back(stub_table);

  stub_table->set_address(owner->address() + owner->data_size());
  stub_table->set_file_offset(owner->offset() + owner->data_size());
  stub_table->finalize_data_size();

  return stub_table;
}

// Scan a relocation for stub generation.

template<bool big_endian>
void
Target_arm<big_endian>::scan_reloc_for_stub(
    const Relocate_info<32, big_endian>* relinfo,
    unsigned int r_type,
    const Sized_symbol<32>* gsym,
    unsigned int r_sym,
    const Symbol_value<32>* psymval,
    elfcpp::Elf_types<32>::Elf_Swxword addend,
    Arm_address address)
{
  typedef typename Target_arm<big_endian>::Relocate Relocate;

  const Arm_relobj<big_endian>* arm_relobj =
    Arm_relobj<big_endian>::as_arm_relobj(relinfo->object);

  bool target_is_thumb;
  Symbol_value<32> symval;
  if (gsym != NULL)
    {
      // This is a global symbol.  Determine if we use PLT and if the
      // final target is THUMB.
      if (gsym->use_plt_offset(Relocate::reloc_is_non_pic(r_type)))
	{
	  // This uses a PLT, change the symbol value.
	  symval.set_output_value(this->plt_section()->address()
				  + gsym->plt_offset());
	  psymval = &symval;
	  target_is_thumb = false;
	}
      else if (gsym->is_undefined())
	// There is no need to generate a stub symbol is undefined.
	return;
      else
	{
	  target_is_thumb =
	    ((gsym->type() == elfcpp::STT_ARM_TFUNC)
	     || (gsym->type() == elfcpp::STT_FUNC
		 && !gsym->is_undefined()
		 && ((psymval->value(arm_relobj, 0) & 1) != 0)));
	}
    }
  else
    {
      // This is a local symbol.  Determine if the final target is THUMB.
      target_is_thumb = arm_relobj->local_symbol_is_thumb_function(r_sym);
    }

  // Strip LSB if this points to a THUMB target.
  if (target_is_thumb
      && Target_arm<big_endian>::reloc_uses_thumb_bit(r_type)
      && ((psymval->value(arm_relobj, 0) & 1) != 0))
    {
      Arm_address stripped_value =
	psymval->value(arm_relobj, 0) & ~static_cast<Arm_address>(1);
      symval.set_output_value(stripped_value);
      psymval = &symval;
    } 

  // Get the symbol value.
  Symbol_value<32>::Value value = psymval->value(arm_relobj, 0);

  // Owing to pipelining, the PC relative branches below actually skip
  // two instructions when the branch offset is 0.
  Arm_address destination;
  switch (r_type)
    {
    case elfcpp::R_ARM_CALL:
    case elfcpp::R_ARM_JUMP24:
    case elfcpp::R_ARM_PLT32:
      // ARM branches.
      destination = value + addend + 8;
      break;
    case elfcpp::R_ARM_THM_CALL:
    case elfcpp::R_ARM_THM_XPC22:
    case elfcpp::R_ARM_THM_JUMP24:
    case elfcpp::R_ARM_THM_JUMP19:
      // THUMB branches.
      destination = value + addend + 4;
      break;
    default:
      gold_unreachable();
    }

  Stub_type stub_type =
    Reloc_stub::stub_type_for_reloc(r_type, address, destination,
				    target_is_thumb);

  // This reloc does not need a stub.
  if (stub_type == arm_stub_none)
    return;

  // Try looking up an existing stub from a stub table.
  Stub_table<big_endian>* stub_table = 
    arm_relobj->stub_table(relinfo->data_shndx);
  gold_assert(stub_table != NULL);
   
  // Locate stub by destination.
  Reloc_stub::Key stub_key(stub_type, gsym, arm_relobj, r_sym, addend);

  // Create a stub if there is not one already
  Reloc_stub* stub = stub_table->find_reloc_stub(stub_key);
  if (stub == NULL)
    {
      // create a new stub and add it to stub table.
      stub = this->stub_factory().make_reloc_stub(stub_type);
      stub_table->add_reloc_stub(stub, stub_key);
    }

  // Record the destination address.
  stub->set_destination_address(destination
				| (target_is_thumb ? 1 : 0));
}

// This function scans a relocation sections for stub generation.
// The template parameter Relocate must be a class type which provides
// a single function, relocate(), which implements the machine
// specific part of a relocation.

// BIG_ENDIAN is the endianness of the data.  SH_TYPE is the section type:
// SHT_REL or SHT_RELA.

// PRELOCS points to the relocation data.  RELOC_COUNT is the number
// of relocs.  OUTPUT_SECTION is the output section.
// NEEDS_SPECIAL_OFFSET_HANDLING is true if input offsets need to be
// mapped to output offsets.

// VIEW is the section data, VIEW_ADDRESS is its memory address, and
// VIEW_SIZE is the size.  These refer to the input section, unless
// NEEDS_SPECIAL_OFFSET_HANDLING is true, in which case they refer to
// the output section.

template<bool big_endian>
template<int sh_type>
void inline
Target_arm<big_endian>::scan_reloc_section_for_stubs(
    const Relocate_info<32, big_endian>* relinfo,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    const unsigned char* view,
    elfcpp::Elf_types<32>::Elf_Addr view_address,
    section_size_type)
{
  typedef typename Reloc_types<sh_type, 32, big_endian>::Reloc Reltype;
  const int reloc_size =
    Reloc_types<sh_type, 32, big_endian>::reloc_size;

  Arm_relobj<big_endian>* arm_object =
    Arm_relobj<big_endian>::as_arm_relobj(relinfo->object);
  unsigned int local_count = arm_object->local_symbol_count();

  Comdat_behavior comdat_behavior = CB_UNDETERMINED;

  for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
    {
      Reltype reloc(prelocs);

      typename elfcpp::Elf_types<32>::Elf_WXword r_info = reloc.get_r_info();
      unsigned int r_sym = elfcpp::elf_r_sym<32>(r_info);
      unsigned int r_type = elfcpp::elf_r_type<32>(r_info);

      r_type = this->get_real_reloc_type(r_type);

      // Only a few relocation types need stubs.
      if ((r_type != elfcpp::R_ARM_CALL)
         && (r_type != elfcpp::R_ARM_JUMP24)
         && (r_type != elfcpp::R_ARM_PLT32)
         && (r_type != elfcpp::R_ARM_THM_CALL)
         && (r_type != elfcpp::R_ARM_THM_XPC22)
         && (r_type != elfcpp::R_ARM_THM_JUMP24)
         && (r_type != elfcpp::R_ARM_THM_JUMP19))
	continue;

      section_offset_type offset =
	convert_to_section_size_type(reloc.get_r_offset());

      if (needs_special_offset_handling)
	{
	  offset = output_section->output_offset(relinfo->object,
						 relinfo->data_shndx,
						 offset);
	  if (offset == -1)
	    continue;
	}

      // Get the addend.
      Stub_addend_reader<sh_type, big_endian> stub_addend_reader;
      elfcpp::Elf_types<32>::Elf_Swxword addend =
	stub_addend_reader(r_type, view + offset, reloc);

      const Sized_symbol<32>* sym;

      Symbol_value<32> symval;
      const Symbol_value<32> *psymval;
      if (r_sym < local_count)
	{
	  sym = NULL;
	  psymval = arm_object->local_symbol(r_sym);

          // If the local symbol belongs to a section we are discarding,
          // and that section is a debug section, try to find the
          // corresponding kept section and map this symbol to its
          // counterpart in the kept section.  The symbol must not 
          // correspond to a section we are folding.
	  bool is_ordinary;
	  unsigned int shndx = psymval->input_shndx(&is_ordinary);
	  if (is_ordinary
	      && shndx != elfcpp::SHN_UNDEF
	      && !arm_object->is_section_included(shndx) 
              && !(relinfo->symtab->is_section_folded(arm_object, shndx)))
	    {
	      if (comdat_behavior == CB_UNDETERMINED)
	        {
	          std::string name =
		    arm_object->section_name(relinfo->data_shndx);
	          comdat_behavior = get_comdat_behavior(name.c_str());
	        }
	      if (comdat_behavior == CB_PRETEND)
	        {
                  bool found;
	          typename elfcpp::Elf_types<32>::Elf_Addr value =
	            arm_object->map_to_kept_section(shndx, &found);
	          if (found)
	            symval.set_output_value(value + psymval->input_value());
                  else
                    symval.set_output_value(0);
	        }
	      else
	        {
                  symval.set_output_value(0);
	        }
	      symval.set_no_output_symtab_entry();
	      psymval = &symval;
	    }
	}
      else
	{
	  const Symbol* gsym = arm_object->global_symbol(r_sym);
	  gold_assert(gsym != NULL);
	  if (gsym->is_forwarder())
	    gsym = relinfo->symtab->resolve_forwards(gsym);

	  sym = static_cast<const Sized_symbol<32>*>(gsym);
	  if (sym->has_symtab_index())
	    symval.set_output_symtab_index(sym->symtab_index());
	  else
	    symval.set_no_output_symtab_entry();

	  // We need to compute the would-be final value of this global
	  // symbol.
	  const Symbol_table* symtab = relinfo->symtab;
	  const Sized_symbol<32>* sized_symbol =
	    symtab->get_sized_symbol<32>(gsym);
	  Symbol_table::Compute_final_value_status status;
	  Arm_address value =
	    symtab->compute_final_value<32>(sized_symbol, &status);

	  // Skip this if the symbol has not output section.
	  if (status == Symbol_table::CFVS_NO_OUTPUT_SECTION)
	    continue;

	  symval.set_output_value(value);
	  psymval = &symval;
	}

      // If symbol is a section symbol, we don't know the actual type of
      // destination.  Give up.
      if (psymval->is_section_symbol())
	continue;

      this->scan_reloc_for_stub(relinfo, r_type, sym, r_sym, psymval,
				addend, view_address + offset);
    }
}

// Scan an input section for stub generation.

template<bool big_endian>
void
Target_arm<big_endian>::scan_section_for_stubs(
    const Relocate_info<32, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    const unsigned char* view,
    Arm_address view_address,
    section_size_type view_size)
{
  if (sh_type == elfcpp::SHT_REL)
    this->scan_reloc_section_for_stubs<elfcpp::SHT_REL>(
	relinfo,
	prelocs,
	reloc_count,
	output_section,
	needs_special_offset_handling,
	view,
	view_address,
	view_size);
  else if (sh_type == elfcpp::SHT_RELA)
    // We do not support RELA type relocations yet.  This is provided for
    // completeness.
    this->scan_reloc_section_for_stubs<elfcpp::SHT_RELA>(
	relinfo,
	prelocs,
	reloc_count,
	output_section,
	needs_special_offset_handling,
	view,
	view_address,
	view_size);
  else
    gold_unreachable();
}

// Group input sections for stub generation.
//
// We goup input sections in an output sections so that the total size,
// including any padding space due to alignment is smaller than GROUP_SIZE
// unless the only input section in group is bigger than GROUP_SIZE already.
// Then an ARM stub table is created to follow the last input section
// in group.  For each group an ARM stub table is created an is placed
// after the last group.  If STUB_ALWATS_AFTER_BRANCH is false, we further
// extend the group after the stub table.

template<bool big_endian>
void
Target_arm<big_endian>::group_sections(
    Layout* layout,
    section_size_type group_size,
    bool stubs_always_after_branch)
{
  // Group input sections and insert stub table
  Layout::Section_list section_list;
  layout->get_allocated_sections(&section_list);
  for (Layout::Section_list::const_iterator p = section_list.begin();
       p != section_list.end();
       ++p)
    {
      Arm_output_section<big_endian>* output_section =
	Arm_output_section<big_endian>::as_arm_output_section(*p);
      output_section->group_sections(group_size, stubs_always_after_branch,
				     this);
    }
}

// Relaxation hook.  This is where we do stub generation.

template<bool big_endian>
bool
Target_arm<big_endian>::do_relax(
    int pass,
    const Input_objects* input_objects,
    Symbol_table* symtab,
    Layout* layout)
{
  // No need to generate stubs if this is a relocatable link.
  gold_assert(!parameters->options().relocatable());

  // If this is the first pass, we need to group input sections into
  // stub groups.
  if (pass == 1)
    {
      // Determine the stub group size.  The group size is the absolute
      // value of the parameter --stub-group-size.  If --stub-group-size
      // is passed a negative value, we restict stubs to be always after
      // the stubbed branches.
      int32_t stub_group_size_param =
	parameters->options().stub_group_size();
      bool stubs_always_after_branch = stub_group_size_param < 0;
      section_size_type stub_group_size = abs(stub_group_size_param);

      if (stub_group_size == 1)
	{
	  // Default value.
	  // Thumb branch range is +-4MB has to be used as the default
	  // maximum size (a given section can contain both ARM and Thumb
	  // code, so the worst case has to be taken into account).
	  //
	  // This value is 24K less than that, which allows for 2025
	  // 12-byte stubs.  If we exceed that, then we will fail to link.
	  // The user will have to relink with an explicit group size
	  // option.
	  stub_group_size = 4170000;
	}

      group_sections(layout, stub_group_size, stubs_always_after_branch);
    }

  // clear changed flags for all stub_tables
  typedef typename Stub_table_list::iterator Stub_table_iterator;
  for (Stub_table_iterator sp = this->stub_tables_.begin();
       sp != this->stub_tables_.end();
       ++sp)
    (*sp)->set_has_been_changed(false);

  // scan relocs for stubs
  for (Input_objects::Relobj_iterator op = input_objects->relobj_begin();
       op != input_objects->relobj_end();
       ++op)
    {
      Arm_relobj<big_endian>* arm_relobj =
	Arm_relobj<big_endian>::as_arm_relobj(*op);
      arm_relobj->scan_sections_for_stubs(this, symtab, layout);
    }

  bool any_stub_table_changed = false;
  for (Stub_table_iterator sp = this->stub_tables_.begin();
       (sp != this->stub_tables_.end()) && !any_stub_table_changed;
       ++sp)
    {
      if ((*sp)->has_been_changed())
	any_stub_table_changed = true;
    }

  return any_stub_table_changed;
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
