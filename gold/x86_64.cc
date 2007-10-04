// x86_64.cc -- x86_64 target support for gold.

// Copyright 2006, 2007, Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation; either version 2, or
// (at your option) any later version.

// In addition to the permissions in the GNU Library General Public
// License, the Free Software Foundation gives you unlimited
// permission to link the compiled version of this file into
// combinations with other programs, and to distribute those
// combinations without any restriction coming from the use of this
// file.  (The Library Public License restrictions do apply in other
// respects; for example, they cover modification of the file, and
/// distribution when not linked into a combined executable.)

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.

// You should have received a copy of the GNU Library General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "gold.h"

#include <cstring>

#include "elfcpp.h"
#include "parameters.h"
#include "reloc.h"
#include "x86_64.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"

namespace
{

using namespace gold;

class Output_data_plt_x86_64;

// The x86_64 target class.
// See the ABI at
//   http://www.x86-64.org/documentation/abi.pdf
// TLS info comes from
//   http://people.redhat.com/drepper/tls.pdf
//   http://ia32-abi.googlegroups.com/web/RFC-TLSDESC-x86.txt?gda=kWQJPEQAAACEfYQFX0dubPQ2NuO4whhjkR4HAp8tBMb_I0iuUeQslmG1qiJ7UbTIup-M2XPURDRiZJyPR4BqKR2agJ-5jfT5Ley2_-oiOJ4zLNAGCw24Bg

class Target_x86_64 : public Sized_target<64, false>
{
 public:
  // In the x86_64 ABI (p 68), it says "The AMD64 ABI architectures
  // uses only Elf64_Rela relocation entries with explicit addends."
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, 64, false> Reloc_section;

  Target_x86_64()
    : Sized_target<64, false>(&x86_64_info),
      got_(NULL), plt_(NULL), got_plt_(NULL), rel_dyn_(NULL),
      copy_relocs_(NULL), dynbss_(NULL)
  { }

  // Scan the relocations to look for symbol adjustments.
  void
  scan_relocs(const General_options& options,
	      Symbol_table* symtab,
	      Layout* layout,
	      Sized_relobj<64, false>* object,
	      unsigned int data_shndx,
	      unsigned int sh_type,
	      const unsigned char* prelocs,
	      size_t reloc_count,
	      size_t local_symbol_count,
	      const unsigned char* plocal_symbols,
	      Symbol** global_symbols);

  // Finalize the sections.
  void
  do_finalize_sections(Layout*);

  // Return the value to use for a dynamic which requires special
  // treatment.
  uint64_t
  do_dynsym_value(const Symbol*) const;

  // Relocate a section.
  void
  relocate_section(const Relocate_info<64, false>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   unsigned char* view,
		   elfcpp::Elf_types<64>::Elf_Addr view_address,
		   off_t view_size);

  // Return a string used to fill a code section with nops.
  std::string
  do_code_fill(off_t length);

 private:
  // The class which scans relocations.
  struct Scan
  {
    inline void
    local(const General_options& options, Symbol_table* symtab,
	  Layout* layout, Target_x86_64* target,
	  Sized_relobj<64, false>* object,
	  unsigned int data_shndx,
	  const elfcpp::Rela<64, false>& reloc, unsigned int r_type,
	  const elfcpp::Sym<64, false>& lsym);

    inline void
    global(const General_options& options, Symbol_table* symtab,
	   Layout* layout, Target_x86_64* target,
	   Sized_relobj<64, false>* object,
	   unsigned int data_shndx,
	   const elfcpp::Rela<64, false>& reloc, unsigned int r_type,
	   Symbol* gsym);
  };

  // The class which implements relocation.
  class Relocate
  {
   public:
    Relocate()
      : skip_call_tls_get_addr_(false)
    { }

    ~Relocate()
    {
      if (this->skip_call_tls_get_addr_)
	{
	  // FIXME: This needs to specify the location somehow.
	  fprintf(stderr, _("%s: missing expected TLS relocation\n"),
		  program_name);
	  gold_exit(false);
	}
    }

    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool
    relocate(const Relocate_info<64, false>*, Target_x86_64*, size_t relnum,
	     const elfcpp::Rela<64, false>&,
	     unsigned int r_type, const Sized_symbol<64>*,
	     const Symbol_value<64>*,
	     unsigned char*, elfcpp::Elf_types<64>::Elf_Addr,
	     off_t);

   private:
    // Do a TLS relocation.
    inline void
    relocate_tls(const Relocate_info<64, false>*, size_t relnum,
		 const elfcpp::Rela<64, false>&,
		 unsigned int r_type, const Sized_symbol<64>*,
		 const Symbol_value<64>*,
		 unsigned char*, elfcpp::Elf_types<64>::Elf_Addr, off_t);

    // Do a TLS Initial-Exec to Local-Exec transition.
    static inline void
    tls_ie_to_le(const Relocate_info<64, false>*, size_t relnum,
		 Output_segment* tls_segment,
		 const elfcpp::Rela<64, false>&, unsigned int r_type,
		 elfcpp::Elf_types<64>::Elf_Addr value,
		 unsigned char* view,
		 off_t view_size);

    // Do a TLS Global-Dynamic to Local-Exec transition.
    inline void
    tls_gd_to_le(const Relocate_info<64, false>*, size_t relnum,
		 Output_segment* tls_segment,
		 const elfcpp::Rela<64, false>&, unsigned int r_type,
		 elfcpp::Elf_types<64>::Elf_Addr value,
		 unsigned char* view,
		 off_t view_size);

    // Check the range for a TLS relocation.
    static inline void
    check_range(const Relocate_info<64, false>*, size_t relnum,
		const elfcpp::Rela<64, false>&, off_t, off_t);

    // Check the validity of a TLS relocation.  This is like assert.
    static inline void
    check_tls(const Relocate_info<64, false>*, size_t relnum,
	      const elfcpp::Rela<64, false>&, bool);

    // This is set if we should skip the next reloc, which should be a
    // PLT32 reloc against ___tls_get_addr.
    bool skip_call_tls_get_addr_;
  };

  // Adjust TLS relocation type based on the options and whether this
  // is a local symbol.
  static unsigned int
  optimize_tls_reloc(bool is_final, int r_type);

  // Get the GOT section, creating it if necessary.
  Output_data_got<64, false>*
  got_section(Symbol_table*, Layout*);

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(Symbol_table*, Layout*, Symbol*);

  // Get the PLT section.
  Output_data_plt_x86_64*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Get the dynamic reloc section, creating it if necessary.
  Reloc_section*
  rel_dyn_section(Layout*);

  // Copy a relocation against a global symbol.
  void
  copy_reloc(const General_options*, Symbol_table*, Layout*,
	     Sized_relobj<64, false>*, unsigned int,
	     Symbol*, const elfcpp::Rela<64, false>&);

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info x86_64_info;

  // The GOT section.
  Output_data_got<64, false>* got_;
  // The PLT section.
  Output_data_plt_x86_64* plt_;
  // The GOT PLT section.
  Output_data_space* got_plt_;
  // The dynamic reloc section.
  Reloc_section* rel_dyn_;
  // Relocs saved to avoid a COPY reloc.
  Copy_relocs<64, false>* copy_relocs_;
  // Space for variables copied with a COPY reloc.
  Output_data_space* dynbss_;
};

const Target::Target_info Target_x86_64::x86_64_info =
{
  64,			// size
  false,		// is_big_endian
  elfcpp::EM_X86_64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  true,			// has_code_fill
  "/lib/ld64.so.1",     // program interpreter
  0x400000,		// text_segment_address
  0x1000,		// abi_pagesize
  0x1000		// common_pagesize
};

// Get the GOT section, creating it if necessary.

Output_data_got<64, false>*
Target_x86_64::got_section(Symbol_table* symtab, Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(symtab != NULL && layout != NULL);

      this->got_ = new Output_data_got<64, false>();

      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->got_);

      // The old GNU linker creates a .got.plt section.  We just
      // create another set of data in the .got section.  Note that we
      // always create a PLT if we create a GOT, although the PLT
      // might be empty.
      // TODO(csilvers): do we really need an alignment of 8?
      this->got_plt_ = new Output_data_space(8);
      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->got_plt_);

      // The first three entries are reserved.
      this->got_plt_->set_space_size(3 * 8);

      // Define _GLOBAL_OFFSET_TABLE_ at the start of the PLT.
      symtab->define_in_output_data(this, "_GLOBAL_OFFSET_TABLE_", NULL,
				    this->got_plt_,
				    0, 0, elfcpp::STT_OBJECT,
				    elfcpp::STB_LOCAL,
				    elfcpp::STV_HIDDEN, 0,
				    false, false);
    }

  return this->got_;
}

// Get the dynamic reloc section, creating it if necessary.

Target_x86_64::Reloc_section*
Target_x86_64::rel_dyn_section(Layout* layout)
{
  if (this->rel_dyn_ == NULL)
    {
      gold_assert(layout != NULL);
      this->rel_dyn_ = new Reloc_section();
      layout->add_output_section_data(".rela.dyn", elfcpp::SHT_RELA,
				      elfcpp::SHF_ALLOC, this->rel_dyn_);
    }
  return this->rel_dyn_;
}

// A class to handle the PLT data.

class Output_data_plt_x86_64 : public Output_section_data
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, 64, false> Reloc_section;

  Output_data_plt_x86_64(Layout*, Output_data_space*);

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

 private:
  // The size of an entry in the PLT.
  static const int plt_entry_size = 16;

  // The first entry in the PLT.
  // From the AMD64 ABI: "Unlike Intel386 ABI, this ABI uses the same
  // procedure linkage table for both programs and shared objects."
  static unsigned char first_plt_entry[plt_entry_size];

  // Other entries in the PLT for an executable.
  static unsigned char plt_entry[plt_entry_size];

  // Set the final size.
  void
  do_set_address(uint64_t, off_t)
  { this->set_data_size((this->count_ + 1) * plt_entry_size); }

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

Output_data_plt_x86_64::Output_data_plt_x86_64(Layout* layout,
                                               Output_data_space* got_plt)
    // TODO(csilvers): do we really need an alignment of 8?
  : Output_section_data(8), got_plt_(got_plt), count_(0)
{
  this->rel_ = new Reloc_section();
  layout->add_output_section_data(".rela.plt", elfcpp::SHT_RELA,
				  elfcpp::SHF_ALLOC, this->rel_);
}

void
Output_data_plt_x86_64::do_adjust_output_section(Output_section* os)
{
  // UnixWare sets the entsize of .plt to 4, and so does the old GNU
  // linker, and so do we.
  os->set_entsize(4);
}

// Add an entry to the PLT.

void
Output_data_plt_x86_64::add_entry(Symbol* gsym)
{
  gold_assert(!gsym->has_plt_offset());

  // Note that when setting the PLT offset we skip the initial
  // reserved PLT entry.
  gsym->set_plt_offset((this->count_ + 1) * plt_entry_size);

  ++this->count_;

  off_t got_offset = this->got_plt_->data_size();

  // Every PLT entry needs a GOT entry which points back to the PLT
  // entry (this will be changed by the dynamic linker, normally
  // lazily when the function is called).
  this->got_plt_->set_space_size(got_offset + 8);

  // Every PLT entry needs a reloc.
  gsym->set_needs_dynsym_entry();
  this->rel_->add_global(gsym, elfcpp::R_X86_64_JUMP_SLOT, this->got_plt_,
			 got_offset, 0);

  // Note that we don't need to save the symbol.  The contents of the
  // PLT are independent of which symbols are used.  The symbols only
  // appear in the relocations.
}

// The first entry in the PLT for an executable.

unsigned char Output_data_plt_x86_64::first_plt_entry[plt_entry_size] =
{
  // From AMD64 ABI Draft 0.98, page 76
  0xff, 0x35,	// pushq contents of memory address
  0, 0, 0, 0,	// replaced with address of .got + 4
  0xff, 0x25,	// jmp indirect
  0, 0, 0, 0,	// replaced with address of .got + 8
  0x90, 0x90, 0x90, 0x90   // noop (x4)
};

// Subsequent entries in the PLT for an executable.

unsigned char Output_data_plt_x86_64::plt_entry[plt_entry_size] =
{
  // From AMD64 ABI Draft 0.98, page 76
  0xff, 0x25,	// jmpq indirect
  0, 0, 0, 0,	// replaced with address of symbol in .got
  0x68,		// pushq immediate
  0, 0, 0, 0,	// replaced with offset into relocation table
  0xe9,		// jmpq relative
  0, 0, 0, 0	// replaced with offset to start of .plt
};

// Write out the PLT.  This uses the hand-coded instructions above,
// and adjusts them as needed.  This is specified by the AMD64 ABI.

void
Output_data_plt_x86_64::do_write(Output_file* of)
{
  const off_t offset = this->offset();
  const off_t oview_size = this->data_size();
  unsigned char* const oview = of->get_output_view(offset, oview_size);

  const off_t got_file_offset = this->got_plt_->offset();
  const off_t got_size = this->got_plt_->data_size();
  unsigned char* const got_view = of->get_output_view(got_file_offset,
						      got_size);

  unsigned char* pov = oview;

  elfcpp::Elf_types<32>::Elf_Addr plt_address = this->address();
  elfcpp::Elf_types<32>::Elf_Addr got_address = this->got_plt_->address();

  memcpy(pov, first_plt_entry, plt_entry_size);
  if (!parameters->output_is_shared())
    {
      // We do a jmp relative to the PC at the end of this instruction.
      elfcpp::Swap_unaligned<32, false>::writeval(pov + 2, got_address + 8
                                                  - (plt_address + 6));
      elfcpp::Swap<32, false>::writeval(pov + 8, got_address + 16
                                        - (plt_address + 12));
    }
  pov += plt_entry_size;

  unsigned char* got_pov = got_view;

  memset(got_pov, 0, 24);
  got_pov += 24;

  unsigned int plt_offset = plt_entry_size;
  unsigned int got_offset = 24;
  const unsigned int count = this->count_;
  for (unsigned int plt_index = 0;
       plt_index < count;
       ++plt_index,
	 pov += plt_entry_size,
	 got_pov += 8,
	 plt_offset += plt_entry_size,
	 got_offset += 8)
    {
      // Set and adjust the PLT entry itself.
      memcpy(pov, plt_entry, plt_entry_size);
      if (parameters->output_is_shared())
        // FIXME(csilvers): what's the right thing to write here?
        elfcpp::Swap_unaligned<32, false>::writeval(pov + 2, got_offset);
      else
        elfcpp::Swap_unaligned<32, false>::writeval(pov + 2,
                                                    (got_address + got_offset
                                                     - (plt_address + plt_offset
                                                        + 6)));

      elfcpp::Swap_unaligned<32, false>::writeval(pov + 7, plt_index);
      elfcpp::Swap<32, false>::writeval(pov + 12,
					- (plt_offset + plt_entry_size));

      // Set the entry in the GOT.
      elfcpp::Swap<64, false>::writeval(got_pov, plt_address + plt_offset + 6);
    }

  gold_assert(pov - oview == oview_size);
  gold_assert(got_pov - got_view == got_size);

  of->write_output_view(offset, oview_size, oview);
  of->write_output_view(got_file_offset, got_size, got_view);
}

// Create a PLT entry for a global symbol.

void
Target_x86_64::make_plt_entry(Symbol_table* symtab, Layout* layout,
                              Symbol* gsym)
{
  if (gsym->has_plt_offset())
    return;

  if (this->plt_ == NULL)
    {
      // Create the GOT sections first.
      this->got_section(symtab, layout);

      this->plt_ = new Output_data_plt_x86_64(layout, this->got_plt_);
      layout->add_output_section_data(".plt", elfcpp::SHT_PROGBITS,
				      (elfcpp::SHF_ALLOC
				       | elfcpp::SHF_EXECINSTR),
				      this->plt_);
    }

  this->plt_->add_entry(gsym);
}

// Handle a relocation against a non-function symbol defined in a
// dynamic object.  The traditional way to handle this is to generate
// a COPY relocation to copy the variable at runtime from the shared
// object into the executable's data segment.  However, this is
// undesirable in general, as if the size of the object changes in the
// dynamic object, the executable will no longer work correctly.  If
// this relocation is in a writable section, then we can create a
// dynamic reloc and the dynamic linker will resolve it to the correct
// address at runtime.  However, we do not want do that if the
// relocation is in a read-only section, as it would prevent the
// readonly segment from being shared.  And if we have to eventually
// generate a COPY reloc, then any dynamic relocations will be
// useless.  So this means that if this is a writable section, we need
// to save the relocation until we see whether we have to create a
// COPY relocation for this symbol for any other relocation.

void
Target_x86_64::copy_reloc(const General_options* options,
                          Symbol_table* symtab,
                          Layout* layout,
                          Sized_relobj<64, false>* object,
                          unsigned int data_shndx, Symbol* gsym,
                          const elfcpp::Rela<64, false>& rel)
{
  Sized_symbol<64>* ssym;
  ssym = symtab->get_sized_symbol SELECT_SIZE_NAME(64) (gsym
							SELECT_SIZE(64));

  if (!Copy_relocs<64, false>::need_copy_reloc(options, object,
					       data_shndx, ssym))
    {
      // So far we do not need a COPY reloc.  Save this relocation.
      // If it turns out that we never need a COPY reloc for this
      // symbol, then we will emit the relocation.
      if (this->copy_relocs_ == NULL)
	this->copy_relocs_ = new Copy_relocs<64, false>();
      this->copy_relocs_->save(ssym, object, data_shndx, rel);
    }
  else
    {
      // Allocate space for this symbol in the .bss section.

      elfcpp::Elf_types<64>::Elf_WXword symsize = ssym->symsize();

      // There is no defined way to determine the required alignment
      // of the symbol.  We pick the alignment based on the size.  We
      // set an arbitrary maximum of 256.
      unsigned int align;
      for (align = 1; align < 512; align <<= 1)
	if ((symsize & align) != 0)
	  break;

      if (this->dynbss_ == NULL)
	{
	  this->dynbss_ = new Output_data_space(align);
	  layout->add_output_section_data(".bss",
					  elfcpp::SHT_NOBITS,
					  (elfcpp::SHF_ALLOC
					   | elfcpp::SHF_WRITE),
					  this->dynbss_);
	}

      Output_data_space* dynbss = this->dynbss_;

      if (align > dynbss->addralign())
	dynbss->set_space_alignment(align);

      off_t dynbss_size = dynbss->data_size();
      dynbss_size = align_address(dynbss_size, align);
      off_t offset = dynbss_size;
      dynbss->set_space_size(dynbss_size + symsize);

      // Define the symbol in the .dynbss section.
      symtab->define_in_output_data(this, ssym->name(), ssym->version(),
				    dynbss, offset, symsize, ssym->type(),
				    ssym->binding(), ssym->visibility(),
				    ssym->nonvis(), false, false);

      // Add the COPY reloc.
      ssym->set_needs_dynsym_entry();
      Reloc_section* rel_dyn = this->rel_dyn_section(layout);
      rel_dyn->add_global(ssym, elfcpp::R_X86_64_COPY, dynbss, offset, 0);
    }
}


// Optimize the TLS relocation type based on what we know about the
// symbol.  IS_FINAL is true if the final address of this symbol is
// known at link time.

unsigned int
Target_x86_64::optimize_tls_reloc(bool is_final, int r_type)
{
  // If we are generating a shared library, then we can't do anything
  // in the linker.
  if (parameters->output_is_shared())
    return r_type;

  switch (r_type)
    {
    case elfcpp::R_X86_64_TLSGD:
    case elfcpp::R_X86_64_GOTPC32_TLSDESC:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSDESC_CALL:  // TODO(csilvers): correct?
      // These are Global-Dynamic which permits fully general TLS
      // access.  Since we know that we are generating an executable,
      // we can convert this to Initial-Exec.  If we also know that
      // this is a local symbol, we can further switch to Local-Exec.
      if (is_final)
	return elfcpp::R_X86_64_TPOFF32;
      return elfcpp::R_X86_64_GOTTPOFF;     // used for Initial-exec

    case elfcpp::R_X86_64_TLSLD:
      // This is Local-Dynamic, which refers to a local symbol in the
      // dynamic TLS block.  Since we know that we generating an
      // executable, we can switch to Local-Exec.
      return elfcpp::R_X86_64_TPOFF32;

    case elfcpp::R_X86_64_GOTTPOFF:
      // These are Initial-Exec relocs which get the thread offset
      // from the GOT.  If we know that we are linking against the
      // local symbol, we can switch to Local-Exec, which links the
      // thread offset into the instruction.
      if (is_final)
	return elfcpp::R_X86_64_TPOFF32;
      return r_type;

    case elfcpp::R_X86_64_TPOFF32:
      // When we already have Local-Exec, there is nothing further we
      // can do.
      return r_type;

    default:
      gold_unreachable();
    }
}

// Scan a relocation for a local symbol.

inline void
Target_x86_64::Scan::local(const General_options&,
                           Symbol_table* symtab,
                           Layout* layout,
                           Target_x86_64* target,
                           Sized_relobj<64, false>* object,
                           unsigned int,
                           const elfcpp::Rela<64, false>&,
                           unsigned int r_type,
                           const elfcpp::Sym<64, false>&)
{
  switch (r_type)
    {
    case elfcpp::R_X86_64_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_X86_64_64:
    case elfcpp::R_X86_64_32:
    case elfcpp::R_X86_64_32S:
    case elfcpp::R_X86_64_16:
    case elfcpp::R_X86_64_8:
      // FIXME: If we are generating a shared object we need to copy
      // this relocation into the object.
      gold_assert(!parameters->output_is_shared());
      break;

    case elfcpp::R_X86_64_PC64:
    case elfcpp::R_X86_64_PC32:
    case elfcpp::R_X86_64_PC16:
    case elfcpp::R_X86_64_PC8:
      break;

    case elfcpp::R_X86_64_GOTPC32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTOFF64:
    case elfcpp::R_X86_64_GOTPC64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_PLTOFF64:  // TODO(csilvers): correct?
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_X86_64_COPY:
    case elfcpp::R_X86_64_GLOB_DAT:
    case elfcpp::R_X86_64_JUMP_SLOT:
    case elfcpp::R_X86_64_RELATIVE:
      // These are outstanding tls relocs, which are unexpected when linking
    case elfcpp::R_X86_64_TPOFF64:
    case elfcpp::R_X86_64_DTPMOD64:
    case elfcpp::R_X86_64_DTPOFF64:
    case elfcpp::R_X86_64_DTPOFF32:
    case elfcpp::R_X86_64_TLSDESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

      // These are initial tls relocs, which are expected when linking
    case elfcpp::R_X86_64_TLSGD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSLD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTTPOFF:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TPOFF32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTPC32_TLSDESC:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSDESC_CALL:  // TODO(csilvers): correct?
      {
	bool output_is_shared = parameters->output_is_shared();
	r_type = Target_x86_64::optimize_tls_reloc(!output_is_shared, r_type);
	switch (r_type)
	  {
          case elfcpp::R_X86_64_TPOFF32:     // Local-exec
	    // FIXME: If generating a shared object, we need to copy
	    // this relocation into the object.
	    gold_assert(!output_is_shared);
	    break;

          case elfcpp::R_X86_64_GOTTPOFF:       // Initial-exec
          case elfcpp::R_X86_64_TLSGD:          // General Dynamic
          case elfcpp::R_X86_64_TLSLD:          // Local Dynamic
          case elfcpp::R_X86_64_GOTPC32_TLSDESC:
          case elfcpp::R_X86_64_TLSDESC_CALL:
	    fprintf(stderr,
		    _("%s: %s: unsupported reloc %u against local symbol\n"),
		    program_name, object->name().c_str(), r_type);
	    break;
	  }
      }
      break;

    case elfcpp::R_X86_64_GOT64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOT32:
    case elfcpp::R_X86_64_GOTPCREL64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTPCREL:
    case elfcpp::R_X86_64_GOTPLT64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_PLT32:
    case elfcpp::R_X86_64_SIZE32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_SIZE64:  // TODO(csilvers): correct?
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u against local symbol\n"),
	      program_name, object->name().c_str(), r_type);
      break;
    }
}


// Scan a relocation for a global symbol.

inline void
Target_x86_64::Scan::global(const General_options& options,
                            Symbol_table* symtab,
                            Layout* layout,
                            Target_x86_64* target,
                            Sized_relobj<64, false>* object,
                            unsigned int data_shndx,
                            const elfcpp::Rela<64, false>& reloc,
                            unsigned int r_type,
                            Symbol* gsym)
{
  switch (r_type)
    {
    case elfcpp::R_X86_64_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_X86_64_64:
    case elfcpp::R_X86_64_PC64:
    case elfcpp::R_X86_64_32:
    case elfcpp::R_X86_64_32S:
    case elfcpp::R_X86_64_PC32:
    case elfcpp::R_X86_64_16:
    case elfcpp::R_X86_64_PC16:
    case elfcpp::R_X86_64_8:
    case elfcpp::R_X86_64_PC8:
      // FIXME: If we are generating a shared object we may need to
      // copy this relocation into the object.  If this symbol is
      // defined in a shared object, we may need to copy this
      // relocation in order to avoid a COPY relocation.
      gold_assert(!parameters->output_is_shared());

      if (gsym->is_from_dynobj())
	{
	  // This symbol is defined in a dynamic object.  If it is a
	  // function, we make a PLT entry.  Otherwise we need to
	  // either generate a COPY reloc or copy this reloc.
	  if (gsym->type() == elfcpp::STT_FUNC)
	    {
	      target->make_plt_entry(symtab, layout, gsym);

	      // If this is not a PC relative reference, then we may
	      // be taking the address of the function.  In that case
	      // we need to set the entry in the dynamic symbol table
	      // to the address of the PLT entry.
	      if (r_type != elfcpp::R_X86_64_PC64
		  && r_type != elfcpp::R_X86_64_PC32
		  && r_type != elfcpp::R_X86_64_PC16
		  && r_type != elfcpp::R_X86_64_PC8)
		gsym->set_needs_dynsym_value();
	    }
	  else
	    target->copy_reloc(&options, symtab, layout, object, data_shndx,
			       gsym, reloc);
	}

      break;

    case elfcpp::R_X86_64_GOT64:
    case elfcpp::R_X86_64_GOT32:
    case elfcpp::R_X86_64_GOTPCREL64:
    case elfcpp::R_X86_64_GOTPCREL:
    case elfcpp::R_X86_64_GOTPLT64:
      {
        // The symbol requires a GOT entry.
        Output_data_got<64, false>* got = target->got_section(symtab, layout);
        if (got->add_global(gsym))
	  {
            // If this symbol is not fully resolved, we need to add a
            // dynamic relocation for it.
            if (!gsym->final_value_is_known())
              {
                Reloc_section* rel_dyn = target->rel_dyn_section(layout);
                rel_dyn->add_global(gsym, elfcpp::R_X86_64_GLOB_DAT, got,
                                    gsym->got_offset(), 0);
              }
          }
      }
      break;

    case elfcpp::R_X86_64_PLT32:
      // If the symbol is fully resolved, this is just a PC32 reloc.
      // Otherwise we need a PLT entry.
      if (gsym->final_value_is_known())
	break;
      target->make_plt_entry(symtab, layout, gsym);
      break;

    case elfcpp::R_X86_64_GOTPC32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTOFF64:
    case elfcpp::R_X86_64_GOTPC64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_PLTOFF64:  // TODO(csilvers): correct?
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_X86_64_COPY:
    case elfcpp::R_X86_64_GLOB_DAT:
    case elfcpp::R_X86_64_JUMP_SLOT:
    case elfcpp::R_X86_64_RELATIVE:
      // These are outstanding tls relocs, which are unexpected when linking
    case elfcpp::R_X86_64_TPOFF64:
    case elfcpp::R_X86_64_DTPMOD64:
    case elfcpp::R_X86_64_DTPOFF64:
    case elfcpp::R_X86_64_DTPOFF32:
    case elfcpp::R_X86_64_TLSDESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

      // These are initial tls relocs, which are expected for global()
    case elfcpp::R_X86_64_TLSGD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSLD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTTPOFF:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TPOFF32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTPC32_TLSDESC:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSDESC_CALL:  // TODO(csilvers): correct?
      {
	const bool is_final = gsym->final_value_is_known();
	r_type = Target_x86_64::optimize_tls_reloc(is_final, r_type);
	switch (r_type)
	  {
          case elfcpp::R_X86_64_TPOFF32:     // Local-exec
	    // FIXME: If generating a shared object, we need to copy
	    // this relocation into the object.
	    gold_assert(!parameters->output_is_shared());
	    break;

          case elfcpp::R_X86_64_GOTTPOFF:       // Initial-exec
          case elfcpp::R_X86_64_TLSGD:          // General Dynamic
          case elfcpp::R_X86_64_TLSLD:          // Local Dynamic
          case elfcpp::R_X86_64_GOTPC32_TLSDESC:
          case elfcpp::R_X86_64_TLSDESC_CALL:
	    fprintf(stderr,
		    _("%s: %s: unsupported reloc %u "
		      "against global symbol %s\n"),
		    program_name, object->name().c_str(), r_type,
		    gsym->name());
	    break;
	  }
      }
      break;

    case elfcpp::R_X86_64_SIZE32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_SIZE64:  // TODO(csilvers): correct?
    default:
      fprintf(stderr,
	      _("%s: %s: unsupported reloc %u against global symbol %s\n"),
	      program_name, object->name().c_str(), r_type, gsym->name());
      break;
    }
}

// Scan relocations for a section.

void
Target_x86_64::scan_relocs(const General_options& options,
                           Symbol_table* symtab,
                           Layout* layout,
                           Sized_relobj<64, false>* object,
                           unsigned int data_shndx,
                           unsigned int sh_type,
                           const unsigned char* prelocs,
                           size_t reloc_count,
                           size_t local_symbol_count,
                           const unsigned char* plocal_symbols,
                           Symbol** global_symbols)
{
  if (sh_type == elfcpp::SHT_REL)
    {
      fprintf(stderr, _("%s: %s: unsupported REL reloc section\n"),
	      program_name, object->name().c_str());
      gold_exit(false);
    }

  gold::scan_relocs<64, false, Target_x86_64, elfcpp::SHT_RELA,
      Target_x86_64::Scan>(
    options,
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    local_symbol_count,
    plocal_symbols,
    global_symbols);
}

// Finalize the sections.

void
Target_x86_64::do_finalize_sections(Layout* layout)
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
	  odyn->add_constant(elfcpp::DT_PLTREL, elfcpp::DT_RELA);
	}

      if (this->rel_dyn_ != NULL)
	{
	  const Output_data* od = this->rel_dyn_;
	  odyn->add_section_address(elfcpp::DT_RELA, od);
	  odyn->add_section_size(elfcpp::DT_RELASZ, od);
	  odyn->add_constant(elfcpp::DT_RELAENT,
			     elfcpp::Elf_sizes<64>::rela_size);
	}

      if (!parameters->output_is_shared())
	{
	  // The value of the DT_DEBUG tag is filled in by the dynamic
	  // linker at run time, and used by the debugger.
	  odyn->add_constant(elfcpp::DT_DEBUG, 0);
	}
    }

  // Emit any relocs we saved in an attempt to avoid generating COPY
  // relocs.
  if (this->copy_relocs_ == NULL)
    return;
  if (this->copy_relocs_->any_to_emit())
    {
      Reloc_section* rel_dyn = this->rel_dyn_section(layout);
      this->copy_relocs_->emit(rel_dyn);
    }
  delete this->copy_relocs_;
  this->copy_relocs_ = NULL;
}

// Perform a relocation.

inline bool
Target_x86_64::Relocate::relocate(const Relocate_info<64, false>* relinfo,
                                  Target_x86_64* target,
                                  size_t relnum,
                                  const elfcpp::Rela<64, false>& rel,
                                  unsigned int r_type,
                                  const Sized_symbol<64>* gsym,
                                  const Symbol_value<64>* psymval,
                                  unsigned char* view,
                                  elfcpp::Elf_types<64>::Elf_Addr address,
                                  off_t view_size)
{
  if (this->skip_call_tls_get_addr_)
    {
      if (r_type != elfcpp::R_X86_64_PLT32
	  || gsym == NULL
	  || strcmp(gsym->name(), "___tls_get_addr") != 0)
	{
	  fprintf(stderr, _("%s: %s: missing expected TLS relocation\n"),
		  program_name,
		  relinfo->location(relnum, rel.get_r_offset()).c_str());
	  gold_exit(false);
	}

      this->skip_call_tls_get_addr_ = false;

      return false;
    }

  // Pick the value to use for symbols defined in shared objects.
  Symbol_value<64> symval;
  if (gsym != NULL && gsym->is_from_dynobj() && gsym->has_plt_offset())
    {
      symval.set_output_value(target->plt_section()->address()
			      + gsym->plt_offset());
      psymval = &symval;
    }

  const Sized_relobj<64, false>* object = relinfo->object;
  const elfcpp::Elf_Xword addend = rel.get_r_addend();

  switch (r_type)
    {
    case elfcpp::R_X86_64_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_X86_64_64:
      Relocate_functions<64, false>::rela64(view, object, psymval, addend);
      break;

    case elfcpp::R_X86_64_PC64:
      Relocate_functions<64, false>::pcrela64(view, object, psymval, addend,
                                              address);
      break;

    case elfcpp::R_X86_64_32:
      // FIXME: we need to verify that value + addend fits into 32 bits:
      //    uint64_t x = value + addend;
      //    x == static_cast<uint64_t>(static_cast<uint32_t>(x))
      // Likewise for other <=32-bit relocations (but see R_X86_64_32S).
      Relocate_functions<64, false>::rela32(view, object, psymval, addend);
      break;

    case elfcpp::R_X86_64_32S:
      // FIXME: we need to verify that value + addend fits into 32 bits:
      //    int64_t x = value + addend;   // note this quantity is signed!
      //    x == static_cast<int64_t>(static_cast<int32_t>(x))
      Relocate_functions<64, false>::rela32(view, object, psymval, addend);
      break;

    case elfcpp::R_X86_64_PC32:
      Relocate_functions<64, false>::pcrela32(view, object, psymval, addend,
                                              address);
      break;

    case elfcpp::R_X86_64_16:
      Relocate_functions<64, false>::rela16(view, object, psymval, addend);
      break;

    case elfcpp::R_X86_64_PC16:
      Relocate_functions<64, false>::pcrela16(view, object, psymval, addend,
                                              address);
      break;

    case elfcpp::R_X86_64_8:
      Relocate_functions<64, false>::rela8(view, object, psymval, addend);
      break;

    case elfcpp::R_X86_64_PC8:
      Relocate_functions<64, false>::pcrela8(view, object, psymval, addend,
                                             address);
      break;

    case elfcpp::R_X86_64_PLT32:
      gold_assert(gsym->has_plt_offset()
		  || gsym->final_value_is_known());
      Relocate_functions<64, false>::pcrela32(view, object, psymval, addend,
                                              address);
      break;

    case elfcpp::R_X86_64_GOT32:
      // Local GOT offsets not yet supported.
      gold_assert(gsym);
      gold_assert(gsym->has_got_offset());
      Relocate_functions<64, false>::rela32(view, gsym->got_offset(), addend);
      break;

    case elfcpp::R_X86_64_GOTPC32:
      {
        gold_assert(gsym);
	elfcpp::Elf_types<64>::Elf_Addr value;
	value = target->got_section(NULL, NULL)->address();
	Relocate_functions<64, false>::pcrela32(view, value, addend, address);
      }
      break;

    case elfcpp::R_X86_64_GOT64:
      // The ABI doc says "Like GOT64, but indicates a PLT entry is needed."
      // Since we always add a PLT entry, this is equivalent.
    case elfcpp::R_X86_64_GOTPLT64:  // TODO(csilvers): correct?
      // Local GOT offsets not yet supported.
      gold_assert(gsym);
      gold_assert(gsym->has_got_offset());
      Relocate_functions<64, false>::rela64(view, gsym->got_offset(), addend);
      break;

    case elfcpp::R_X86_64_GOTPC64:
      {
        gold_assert(gsym);
	elfcpp::Elf_types<64>::Elf_Addr value;
	value = target->got_section(NULL, NULL)->address();
	Relocate_functions<64, false>::pcrela64(view, value, addend, address);
      }
      break;

    case elfcpp::R_X86_64_GOTOFF64:
      {
	elfcpp::Elf_types<64>::Elf_Addr value;
	value = (psymval->value(object, 0)
		 - target->got_section(NULL, NULL)->address());
	Relocate_functions<64, false>::rela64(view, value, addend);
      }
      break;

    case elfcpp::R_X86_64_GOTPCREL:
      {
        // Local GOT offsets not yet supported.
        gold_assert(gsym);
        gold_assert(gsym->has_got_offset());
	elfcpp::Elf_types<64>::Elf_Addr value;
	value = (target->got_section(NULL, NULL)->address()
                 + gsym->got_offset());
	Relocate_functions<64, false>::pcrela32(view, value, addend, address);
      }
      break;

    case elfcpp::R_X86_64_GOTPCREL64:
      {
        // Local GOT offsets not yet supported.
        gold_assert(gsym);
        gold_assert(gsym->has_got_offset());
	elfcpp::Elf_types<64>::Elf_Addr value;
	value = (target->got_section(NULL, NULL)->address()
                 + gsym->got_offset());
	Relocate_functions<64, false>::pcrela64(view, value, addend, address);
      }
      break;

    case elfcpp::R_X86_64_COPY:
    case elfcpp::R_X86_64_GLOB_DAT:
    case elfcpp::R_X86_64_JUMP_SLOT:
    case elfcpp::R_X86_64_RELATIVE:
      // These are outstanding tls relocs, which are unexpected when linking
    case elfcpp::R_X86_64_TPOFF64:
    case elfcpp::R_X86_64_DTPMOD64:
    case elfcpp::R_X86_64_DTPOFF64:
    case elfcpp::R_X86_64_DTPOFF32:
    case elfcpp::R_X86_64_TLSDESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      gold_exit(false);
      break;

      // These are initial tls relocs, which are expected when linking
    case elfcpp::R_X86_64_TLSGD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSLD:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTTPOFF:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TPOFF32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_GOTPC32_TLSDESC:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_TLSDESC_CALL:  // TODO(csilvers): correct?
      this->relocate_tls(relinfo, relnum, rel, r_type, gsym, psymval, view,
			 address, view_size);
      break;

    case elfcpp::R_X86_64_SIZE32:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_SIZE64:  // TODO(csilvers): correct?
    case elfcpp::R_X86_64_PLTOFF64:  // TODO(csilvers): implement me!
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;
    }

  return true;
}

// Perform a TLS relocation.

inline void
Target_x86_64::Relocate::relocate_tls(const Relocate_info<64, false>* relinfo,
                                      size_t relnum,
                                      const elfcpp::Rela<64, false>& rel,
                                      unsigned int r_type,
                                      const Sized_symbol<64>* gsym,
                                      const Symbol_value<64>* psymval,
                                      unsigned char* view,
                                      elfcpp::Elf_types<64>::Elf_Addr,
                                      off_t view_size)
{
  Output_segment* tls_segment = relinfo->layout->tls_segment();
  if (tls_segment == NULL)
    {
      fprintf(stderr, _("%s: %s: TLS reloc but no TLS segment\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }

  elfcpp::Elf_types<64>::Elf_Addr value = psymval->value(relinfo->object, 0);

  const bool is_final = (gsym == NULL
			 ? !parameters->output_is_shared()
			 : gsym->final_value_is_known());
  const unsigned int opt_r_type =
    Target_x86_64::optimize_tls_reloc(is_final, r_type);
  switch (r_type)
    {
    case elfcpp::R_X86_64_TPOFF32:   // Local-exec reloc
      value = value - (tls_segment->vaddr() + tls_segment->memsz());
      Relocate_functions<64, false>::rel32(view, value);
      break;

    case elfcpp::R_X86_64_GOTTPOFF:  // Initial-exec reloc
      if (opt_r_type == elfcpp::R_X86_64_TPOFF32)
	{
	  Target_x86_64::Relocate::tls_ie_to_le(relinfo, relnum, tls_segment,
                                                rel, r_type, value, view,
                                                view_size);
	  break;
	}
      fprintf(stderr, _("%s: %s: unsupported reloc type %u\n"),
              program_name,
              relinfo->location(relnum, rel.get_r_offset()).c_str(),
              r_type);
      // gold_exit(false);
      break;

    case elfcpp::R_X86_64_TLSGD:
      if (opt_r_type == elfcpp::R_X86_64_TPOFF32)
	{
	  this->tls_gd_to_le(relinfo, relnum, tls_segment,
			     rel, r_type, value, view,
			     view_size);
	  break;
	}
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;

    case elfcpp::R_X86_64_TLSLD:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;
    }
}

// Do a relocation in which we convert a TLS Initial-Exec to a
// Local-Exec.
// TODO(csilvers): verify this is right.

inline void
Target_x86_64::Relocate::tls_ie_to_le(const Relocate_info<64, false>* relinfo,
                                      size_t relnum,
                                      Output_segment* tls_segment,
                                      const elfcpp::Rela<64, false>& rel,
                                      unsigned int,
                                      elfcpp::Elf_types<64>::Elf_Addr value,
                                      unsigned char* view,
                                      off_t view_size)
{
  // We have to actually change the instructions, which means that we
  // need to examine the opcodes to figure out which instruction we
  // are looking at.

  // movl %gs:XX,%eax  ==>  movl $YY,%eax
  // movl %gs:XX,%reg  ==>  movl $YY,%reg
  // addl %gs:XX,%reg  ==>  addl $YY,%reg
  Target_x86_64::Relocate::check_range(relinfo, relnum, rel, view_size, -1);
  Target_x86_64::Relocate::check_range(relinfo, relnum, rel, view_size, 4);

  unsigned char op1 = view[-1];
  if (op1 == 0xa1)
    {
      // movl XX,%eax  ==>  movl $YY,%eax
      view[-1] = 0xb8;
    }
  else
    {
      Target_x86_64::Relocate::check_range(relinfo, relnum, rel,
                                           view_size, -2);

      unsigned char op2 = view[-2];
      if (op2 == 0x8b)
        {
          // movl XX,%reg  ==>  movl $YY,%reg
          Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                             (op1 & 0xc7) == 0x05);
          view[-2] = 0xc7;
          view[-1] = 0xc0 | ((op1 >> 3) & 7);
        }
      else if (op2 == 0x03)
        {
          // addl XX,%reg  ==>  addl $YY,%reg
          Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                             (op1 & 0xc7) == 0x05);
          view[-2] = 0x81;
          view[-1] = 0xc0 | ((op1 >> 3) & 7);
        }
      else
        Target_x86_64::Relocate::check_tls(relinfo, relnum, rel, 0);
    }

  value = value - (tls_segment->vaddr() + tls_segment->memsz());
  Relocate_functions<64, false>::rel32(view, value);
}

// Do a relocation in which we convert a TLS Global-Dynamic to a
// Local-Exec.
// TODO(csilvers): verify this is right.

inline void
Target_x86_64::Relocate::tls_gd_to_le(const Relocate_info<64, false>* relinfo,
                                      size_t relnum,
                                      Output_segment* tls_segment,
                                      const elfcpp::Rela<64, false>& rel,
                                      unsigned int,
                                      elfcpp::Elf_types<64>::Elf_Addr value,
                                      unsigned char* view,
                                      off_t view_size)
{
  // leal foo(,%reg,1),%eax; call ___tls_get_addr
  //  ==> movl %gs,0,%eax; subl $foo@tpoff,%eax
  // leal foo(%reg),%eax; call ___tls_get_addr
  //  ==> movl %gs:0,%eax; subl $foo@tpoff,%eax

  Target_x86_64::Relocate::check_range(relinfo, relnum, rel, view_size, -2);
  Target_x86_64::Relocate::check_range(relinfo, relnum, rel, view_size, 9);

  unsigned char op1 = view[-1];
  unsigned char op2 = view[-2];

  Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                     op2 == 0x8d || op2 == 0x04);
  Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                     view[4] == 0xe8);

  int roff = 5;

  if (op2 == 0x04)
    {
      Target_x86_64::Relocate::check_range(relinfo, relnum, rel, view_size, -3);
      Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                         view[-3] == 0x8d);
      Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                         ((op1 & 0xc7) == 0x05
                                          && op1 != (4 << 3)));
      memcpy(view - 3, "\x65\xa1\0\0\0\0\x81\xe8\0\0\0", 12);
    }
  else
    {
      Target_x86_64::Relocate::check_tls(relinfo, relnum, rel,
                                         (op1 & 0xf8) == 0x80 && (op1 & 7) != 4);
      if (static_cast<off_t>(rel.get_r_offset() + 9) < view_size
          && view[9] == 0x90)
	{
	  // There is a trailing nop.  Use the size byte subl.
	  memcpy(view - 2, "\x65\xa1\0\0\0\0\x81\xe8\0\0\0", 12);
	  roff = 6;
	}
      else
	{
	  // Use the five byte subl.
	  memcpy(view - 2, "\x65\xa1\0\0\0\0\x2d\0\0\0", 11);
	}
    }

  value = tls_segment->vaddr() + tls_segment->memsz() - value;
  Relocate_functions<64, false>::rel32(view + roff, value);

  // The next reloc should be a PLT32 reloc against __tls_get_addr.
  // We can skip it.
  this->skip_call_tls_get_addr_ = true;
}

// Check the range for a TLS relocation.

inline void
Target_x86_64::Relocate::check_range(const Relocate_info<64, false>* relinfo,
                                     size_t relnum,
                                     const elfcpp::Rela<64, false>& rel,
                                     off_t view_size, off_t off)
{
  off_t offset = rel.get_r_offset() + off;
  if (offset < 0 || offset > view_size)
    {
      fprintf(stderr, _("%s: %s: TLS relocation out of range\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }
}

// Check the validity of a TLS relocation.  This is like assert.

inline void
Target_x86_64::Relocate::check_tls(const Relocate_info<64, false>* relinfo,
                                   size_t relnum,
                                   const elfcpp::Rela<64, false>& rel,
                                   bool valid)
{
  if (!valid)
    {
      fprintf(stderr,
	      _("%s: %s: TLS relocation against invalid instruction\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }
}

// Relocate section data.

void
Target_x86_64::relocate_section(const Relocate_info<64, false>* relinfo,
                                unsigned int sh_type,
                                const unsigned char* prelocs,
                                size_t reloc_count,
                                unsigned char* view,
                                elfcpp::Elf_types<64>::Elf_Addr address,
                                off_t view_size)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_section<64, false, Target_x86_64, elfcpp::SHT_RELA,
			 Target_x86_64::Relocate>(
    relinfo,
    this,
    prelocs,
    reloc_count,
    view,
    address,
    view_size);
}

// Return the value to use for a dynamic which requires special
// treatment.  This is how we support equality comparisons of function
// pointers across shared library boundaries, as described in the
// processor specific ABI supplement.

uint64_t
Target_x86_64::do_dynsym_value(const Symbol* gsym) const
{
  gold_assert(gsym->is_from_dynobj() && gsym->has_plt_offset());
  return this->plt_section()->address() + gsym->plt_offset();
}

// Return a string used to fill a code section with nops to take up
// the specified length.

std::string
Target_x86_64::do_code_fill(off_t length)
{
  if (length >= 16)
    {
      // Build a jmpq instruction to skip over the bytes.
      unsigned char jmp[5];
      jmp[0] = 0xe9;
      elfcpp::Swap_unaligned<64, false>::writeval(jmp + 1, length - 5);
      return (std::string(reinterpret_cast<char*>(&jmp[0]), 5)
              + std::string(length - 5, '\0'));
    }

  // Nop sequences of various lengths.
  const char nop1[1] = { 0x90 };                   // nop
  const char nop2[2] = { 0x66, 0x90 };             // xchg %ax %ax
  const char nop3[3] = { 0x8d, 0x76, 0x00 };       // leal 0(%esi),%esi
  const char nop4[4] = { 0x8d, 0x74, 0x26, 0x00};  // leal 0(%esi,1),%esi
  const char nop5[5] = { 0x90, 0x8d, 0x74, 0x26,   // nop
                         0x00 };                   // leal 0(%esi,1),%esi
  const char nop6[6] = { 0x8d, 0xb6, 0x00, 0x00,   // leal 0L(%esi),%esi
                         0x00, 0x00 };
  const char nop7[7] = { 0x8d, 0xb4, 0x26, 0x00,   // leal 0L(%esi,1),%esi
                         0x00, 0x00, 0x00 };
  const char nop8[8] = { 0x90, 0x8d, 0xb4, 0x26,   // nop
                         0x00, 0x00, 0x00, 0x00 }; // leal 0L(%esi,1),%esi
  const char nop9[9] = { 0x89, 0xf6, 0x8d, 0xbc,   // movl %esi,%esi
                         0x27, 0x00, 0x00, 0x00,   // leal 0L(%edi,1),%edi
                         0x00 };
  const char nop10[10] = { 0x8d, 0x76, 0x00, 0x8d, // leal 0(%esi),%esi
                           0xbc, 0x27, 0x00, 0x00, // leal 0L(%edi,1),%edi
                           0x00, 0x00 };
  const char nop11[11] = { 0x8d, 0x74, 0x26, 0x00, // leal 0(%esi,1),%esi
                           0x8d, 0xbc, 0x27, 0x00, // leal 0L(%edi,1),%edi
                           0x00, 0x00, 0x00 };
  const char nop12[12] = { 0x8d, 0xb6, 0x00, 0x00, // leal 0L(%esi),%esi
                           0x00, 0x00, 0x8d, 0xbf, // leal 0L(%edi),%edi
                           0x00, 0x00, 0x00, 0x00 };
  const char nop13[13] = { 0x8d, 0xb6, 0x00, 0x00, // leal 0L(%esi),%esi
                           0x00, 0x00, 0x8d, 0xbc, // leal 0L(%edi,1),%edi
                           0x27, 0x00, 0x00, 0x00,
                           0x00 };
  const char nop14[14] = { 0x8d, 0xb4, 0x26, 0x00, // leal 0L(%esi,1),%esi
                           0x00, 0x00, 0x00, 0x8d, // leal 0L(%edi,1),%edi
                           0xbc, 0x27, 0x00, 0x00,
                           0x00, 0x00 };
  const char nop15[15] = { 0xeb, 0x0d, 0x90, 0x90, // jmp .+15
                           0x90, 0x90, 0x90, 0x90, // nop,nop,nop,...
                           0x90, 0x90, 0x90, 0x90,
                           0x90, 0x90, 0x90 };

  const char* nops[16] = {
    NULL,
    nop1, nop2, nop3, nop4, nop5, nop6, nop7,
    nop8, nop9, nop10, nop11, nop12, nop13, nop14, nop15
  };

  return std::string(nops[length], length);
}

// The selector for x86_64 object files.

class Target_selector_x86_64 : public Target_selector
{
public:
  Target_selector_x86_64()
    : Target_selector(elfcpp::EM_X86_64, 64, false)
  { }

  Target*
  recognize(int machine, int osabi, int abiversion);

 private:
  Target_x86_64* target_;
};

// Recognize an x86_64 object file when we already know that the machine
// number is EM_X86_64.

Target*
Target_selector_x86_64::recognize(int, int, int)
{
  if (this->target_ == NULL)
    this->target_ = new Target_x86_64();
  return this->target_;
}

Target_selector_x86_64 target_selector_x86_64;

} // End anonymous namespace.
