// i386.cc -- i386 target support for gold.

#include "gold.h"
#include "elfcpp.h"
#include "reloc.h"
#include "i386.h"
#include "object.h"
#include "layout.h"
#include "output.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"

namespace
{

using namespace gold;

// The i386 target class.

class Target_i386 : public Sized_target<32, false>
{
 public:
  Target_i386()
    : Sized_target<32, false>(&i386_info)
  { }

  // Scan the relocations to look for symbol adjustments.
  void
  scan_relocs(const General_options& options,
	      Symbol_table* symtab,
	      Sized_object<32, false>* object,
	      unsigned int sh_type,
	      const unsigned char* prelocs,
	      size_t reloc_count,
	      size_t local_symbol_count,
	      const unsigned char* plocal_symbols,
	      Symbol** global_symbols);

  // Relocate a section.
  void
  relocate_section(const Relocate_info<32, false>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   unsigned char* view,
		   elfcpp::Elf_types<32>::Elf_Addr view_address,
		   off_t view_size);

 private:
  // The class which scans relocations.
  struct Scan
  {
    inline void
    local(const General_options& options, Sized_object<32, false>* object,
	  const elfcpp::Rel<32, false>& reloc, unsigned int r_type,
	  const elfcpp::Sym<32, false>& lsym);

    inline void
    global(const General_options& options, Sized_object<32, false>* object,
	   const elfcpp::Rel<32, false>& reloc, unsigned int r_type,
	   Symbol* gsym);
  };

  // The class which implements relocation.
  class Relocate
  {
   public:
    // Do a relocation.
    static inline void
    relocate(const Relocate_info<32, false>*, size_t relnum,
	     const elfcpp::Rel<32, false>&,
	     unsigned int r_type, Sized_symbol<32>*,
	     elfcpp::Elf_types<32>::Elf_Addr,
	     unsigned char*, elfcpp::Elf_types<32>::Elf_Addr,
	     off_t);

   private:
    // Do a TLS relocation.
    static inline void
    relocate_tls(const Relocate_info<32, false>*, size_t relnum,
		 const elfcpp::Rel<32, false>&,
		 unsigned int r_type, Sized_symbol<32>*,
		 elfcpp::Elf_types<32>::Elf_Addr,
		 unsigned char*, elfcpp::Elf_types<32>::Elf_Addr, off_t);

    // Do a TLS Initial-Exec to Local-Exec transition.
    static inline void
    tls_ie_to_le(const Relocate_info<32, false>*, size_t relnum,
		 Output_segment* tls_segment,
		 const elfcpp::Rel<32, false>&, unsigned int r_type,
		 elfcpp::Elf_types<32>::Elf_Addr value,
		 unsigned char* view,
		 off_t view_size);

    // Check the range for a TLS relocation.
    static inline void
    check_range(const Relocate_info<32, false>*, size_t relnum,
		const elfcpp::Rel<32, false>&, off_t, off_t);

    // Check the validity of a TLS relocation.  This is like assert.
    static inline void
    check_tls(const Relocate_info<32, false>*, size_t relnum,
	      const elfcpp::Rel<32, false>&, bool);
  };

  // Adjust TLS relocation type based on the options and whether this
  // is a local symbol.
  static unsigned int
  optimize_tls_reloc(const General_options*, bool is_local, int r_type);

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info i386_info;
};

const Target::Target_info Target_i386::i386_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_386,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve,
  0x08048000,		// text_segment_address,
  0x1000,		// abi_pagesize
  0x1000		// common_pagesize
};

// Optimize the TLS relocation type based on what we know about the
// symbol.  IS_LOCAL is true if this symbol can be resolved entirely
// locally--i.e., does not have to be in the dynamic symbol table.

unsigned int
Target_i386::optimize_tls_reloc(const General_options* options, bool is_local,
				int r_type)
{
  // If we are generating a shared library, then we can't do anything
  // in the linker.
  if (options->is_shared())
    return r_type;

  switch (r_type)
    {
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      // These are Global-Dynamic which permits fully general TLS
      // access.  Since we know that we are generating an executable,
      // we can convert this to Initial-Exec.  If we also know that
      // this is a local symbol, we can further switch to Local-Exec.
      if (is_local)
	return elfcpp::R_386_TLS_LE_32;
      return elfcpp::R_386_TLS_IE_32;

    case elfcpp::R_386_TLS_LDM:
      // This is Local-Dynamic, which refers to a local symbol in the
      // dynamic TLS block.  Since we know that we generating an
      // executable, we can switch to Local-Exec.
      return elfcpp::R_386_TLS_LE_32;

    case elfcpp::R_386_TLS_LDO_32:
      // Another type of Local-Dynamic relocation.
      return elfcpp::R_386_TLS_LE;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_IE_32:
      // These are Initial-Exec relocs which get the thread offset
      // from the GOT.  If we know that we are linking against the
      // local symbol, we can switch to Local-Exec, which links the
      // thread offset into the instruction.
      if (is_local)
	return elfcpp::R_386_TLS_LE_32;
      return r_type;
	
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_LE_32:
      // When we already have Local-Exec, there is nothing further we
      // can do.
      return r_type;

    default:
      abort();
    }
}

// Scan a relocation for a local symbol.

inline void
Target_i386::Scan::local(const General_options& options,
			 Sized_object<32, false>* object,
			 const elfcpp::Rel<32, false>&, unsigned int r_type,
			 const elfcpp::Sym<32, false>&)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_386_32:
    case elfcpp::R_386_16:
    case elfcpp::R_386_8:
      // FIXME: If we are generating a shared object we need to copy
      // this relocation into the object.
      break;

    case elfcpp::R_386_PC32:
    case elfcpp::R_386_PC16:
    case elfcpp::R_386_PC8:
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      r_type = Target_i386::optimize_tls_reloc(&options, true, r_type);
      switch (r_type)
	{
	case elfcpp::R_386_TLS_LE:
	case elfcpp::R_386_TLS_LE_32:
	  // FIXME: If generating a shared object, we need to copy
	  // this relocation into the object.
	  break;

	case elfcpp::R_386_TLS_IE:
	case elfcpp::R_386_TLS_GOTIE:
	case elfcpp::R_386_TLS_GD:
	case elfcpp::R_386_TLS_LDM:
	case elfcpp::R_386_TLS_LDO_32:
	case elfcpp::R_386_TLS_IE_32:
	case elfcpp::R_386_TLS_GOTDESC:
	case elfcpp::R_386_TLS_DESC_CALL:
	  fprintf(stderr,
		  _("%s: %s: unsupported reloc %u against local symbol\n"),
		  program_name, object->name().c_str(), r_type);
	  break;
	}
      break;

    case elfcpp::R_386_GOT32:
    case elfcpp::R_386_PLT32:
    case elfcpp::R_386_GOTOFF:
    case elfcpp::R_386_GOTPC:
    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u against local symbol\n"),
	      program_name, object->name().c_str(), r_type);
      break;
    }
}

// Scan a relocation for a global symbol.

inline void
Target_i386::Scan::global(const General_options& options,
			  Sized_object<32, false>* object,
			  const elfcpp::Rel<32, false>&, unsigned int r_type,
			  Symbol* gsym)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY: 
      break;

    case elfcpp::R_386_32:
    case elfcpp::R_386_PC32:
    case elfcpp::R_386_16:
    case elfcpp::R_386_PC16:
    case elfcpp::R_386_8:
    case elfcpp::R_386_PC8:
      // FIXME: If we are generating a shared object we may need to
      // copy this relocation into the object.  If this symbol is
      // defined in a shared object, we may need to copy this
      // relocation in order to avoid a COPY relocation.
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      r_type = Target_i386::optimize_tls_reloc(&options,
					       !gsym->in_dynsym(),
					       r_type);
      switch (r_type)
	{
	case elfcpp::R_386_TLS_LE:
	case elfcpp::R_386_TLS_LE_32:
	  // FIXME: If generating a shared object, we need to copy
	  // this relocation into the object.
	  break;

	case elfcpp::R_386_TLS_IE:
	case elfcpp::R_386_TLS_GOTIE:
	case elfcpp::R_386_TLS_GD:
	case elfcpp::R_386_TLS_LDM:
	case elfcpp::R_386_TLS_LDO_32:
	case elfcpp::R_386_TLS_IE_32:
	case elfcpp::R_386_TLS_GOTDESC:
	case elfcpp::R_386_TLS_DESC_CALL:
	  fprintf(stderr,
		  _("%s: %s: unsupported reloc %u against global symbol %s\n"),
		  program_name, object->name().c_str(), r_type, gsym->name());
	  break;
	}
      break;

    case elfcpp::R_386_GOT32:
    case elfcpp::R_386_PLT32:
    case elfcpp::R_386_GOTOFF:
    case elfcpp::R_386_GOTPC:
    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr,
	      _("%s: %s: unsupported reloc %u against global symbol %s\n"),
	      program_name, object->name().c_str(), r_type, gsym->name());
      break;
    }
}

// Scan relocations for a section.

void
Target_i386::scan_relocs(const General_options& options,
			 Symbol_table* symtab,
			 Sized_object<32, false>* object,
			 unsigned int sh_type,
			 const unsigned char* prelocs,
			 size_t reloc_count,
			 size_t local_symbol_count,
			 const unsigned char* plocal_symbols,
			 Symbol** global_symbols)
{
  if (sh_type == elfcpp::SHT_RELA)
    {
      fprintf(stderr, _("%s: %s: unsupported RELA reloc section\n"),
	      program_name, object->name().c_str());
      gold_exit(false);
    }

  gold::scan_relocs<32, false, elfcpp::SHT_REL, Target_i386::Scan>(
    options,
    symtab,
    object,
    prelocs,
    reloc_count,
    local_symbol_count,
    plocal_symbols,
    global_symbols);
}

// Perform a relocation.

inline void
Target_i386::Relocate::relocate(const Relocate_info<32, false>* relinfo,
				size_t relnum,
				const elfcpp::Rel<32, false>& rel,
				unsigned int r_type,
				Sized_symbol<32>* gsym,
				elfcpp::Elf_types<32>::Elf_Addr value,
				unsigned char* view,
				elfcpp::Elf_types<32>::Elf_Addr address,
				off_t view_size)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_386_32:
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_PC32:
      Relocate_functions<32, false>::pcrel32(view, value, address);
      break;

    case elfcpp::R_386_16:
      Relocate_functions<32, false>::rel16(view, value);
      break;

    case elfcpp::R_386_PC16:
      Relocate_functions<32, false>::pcrel16(view, value, address);
      break;

    case elfcpp::R_386_8:
      Relocate_functions<32, false>::rel8(view, value);
      break;

    case elfcpp::R_386_PC8:
      Relocate_functions<32, false>::pcrel8(view, value, address);
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      Target_i386::Relocate::relocate_tls(relinfo, relnum, rel, r_type,
					  gsym, value, view, address,
					  view_size);
      break;

    case elfcpp::R_386_GOT32:
    case elfcpp::R_386_PLT32:
    case elfcpp::R_386_GOTOFF:
    case elfcpp::R_386_GOTPC:
    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;
    }
}

// Perform a TLS relocation.

inline void
Target_i386::Relocate::relocate_tls(const Relocate_info<32, false>* relinfo,
				    size_t relnum,
				    const elfcpp::Rel<32, false>& rel,
				    unsigned int r_type,
				    Sized_symbol<32>* gsym,
				    elfcpp::Elf_types<32>::Elf_Addr value,
				    unsigned char* view,
				    elfcpp::Elf_types<32>::Elf_Addr,
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

  const bool is_local = gsym == NULL || !gsym->in_dynsym();
  const unsigned int opt_r_type =
    Target_i386::optimize_tls_reloc(relinfo->options, is_local, r_type);
  switch (r_type)
    {
    case elfcpp::R_386_TLS_LE_32:
      value = tls_segment->vaddr() + tls_segment->memsz() - value;
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_TLS_LE:
      value = value - (tls_segment->vaddr() + tls_segment->memsz());
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_IE_32:
      if (opt_r_type == elfcpp::R_386_TLS_LE_32)
	{
	  Target_i386::Relocate::tls_ie_to_le(relinfo, relnum, tls_segment,
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

    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
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

inline void
Target_i386::Relocate::tls_ie_to_le(const Relocate_info<32, false>* relinfo,
				    size_t relnum,
				    Output_segment* tls_segment,
				    const elfcpp::Rel<32, false>& rel,
				    unsigned int r_type,
				    elfcpp::Elf_types<32>::Elf_Addr value,
				    unsigned char* view,
				    off_t view_size)
{
  // We have to actually change the instructions, which means that we
  // need to examine the opcodes to figure out which instruction we
  // are looking at.
  if (r_type == elfcpp::R_386_TLS_IE)
    {
      // movl %gs:XX,%eax  ==>  movl $YY,%eax
      // movl %gs:XX,%reg  ==>  movl $YY,%reg
      // addl %gs:XX,%reg  ==>  addl $YY,%reg
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -1);
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, 4);

      unsigned char op1 = view[-1];
      if (op1 == 0xa1)
	{
	  // movl XX,%eax  ==>  movl $YY,%eax
	  view[-1] = 0xb8;
	}
      else
	{
	  Target_i386::Relocate::check_range(relinfo, relnum, rel,
					     view_size, -2);

	  unsigned char op2 = view[-2];
	  if (op2 == 0x8b)
	    {
	      // movl XX,%reg  ==>  movl $YY,%reg
	      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
					       (op1 & 0xc7) == 0x05);
	      view[-2] = 0xc7;
	      view[-1] = 0xc0 | ((op1 >> 3) & 7);
	    }
	  else if (op2 == 0x03)
	    {
	      // addl XX,%reg  ==>  addl $YY,%reg
	      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
					       (op1 & 0xc7) == 0x05);
	      view[-2] = 0x81;
	      view[-1] = 0xc0 | ((op1 >> 3) & 7);
	    }
	  else
	    Target_i386::Relocate::check_tls(relinfo, relnum, rel, 0);
	}
    }
  else
    {
      // subl %gs:XX(%reg1),%reg2  ==>  subl $YY,%reg2
      // movl %gs:XX(%reg1),%reg2  ==>  movl $YY,%reg2
      // addl %gs:XX(%reg1),%reg2  ==>  addl $YY,$reg2
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -2);
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, 4);

      unsigned char op1 = view[-1];
      unsigned char op2 = view[-2];
      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				       (op1 & 0xc0) == 0x80 && (op1 & 7) != 4);
      if (op2 == 0x8b)
	{
	  // movl %gs:XX(%reg1),%reg2  ==>  movl $YY,%reg2
	  view[-2] = 0xc7;
	  view[-1] = 0xc0 | ((op1 >> 3) & 7);
	}
      else if (op2 == 0x2b)
	{
	  // subl %gs:XX(%reg1),%reg2  ==>  subl $YY,%reg2
	  view[-2] = 0x81;
	  view[-1] = 0xe8 | ((op1 >> 3) & 7);
	}
      else if (op2 == 0x03)
	{
	  // addl %gs:XX(%reg1),%reg2  ==>  addl $YY,$reg2
	  view[-2] = 0x81;
	  view[-1] = 0xc0 | ((op1 >> 3) & 7);
	}
      else
	Target_i386::Relocate::check_tls(relinfo, relnum, rel, 0);
    }

  if (r_type == elfcpp::R_386_TLS_IE_32)
    value = tls_segment->vaddr() + tls_segment->memsz() - value;
  else // elfcpp::R_386_TLS_IE, elfcpp::R_386_TLS_GOTIE
    value = value - (tls_segment->vaddr() + tls_segment->memsz());

  Relocate_functions<32, false>::rel32(view, value);
}

// Check the range for a TLS relocation.

inline void
Target_i386::Relocate::check_range(const Relocate_info<32, false>* relinfo,
				   size_t relnum,
				   const elfcpp::Rel<32, false>& rel,
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
Target_i386::Relocate::check_tls(const Relocate_info<32, false>* relinfo,
				 size_t relnum,
				 const elfcpp::Rel<32, false>& rel,
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
Target_i386::relocate_section(const Relocate_info<32, false>* relinfo,
			      unsigned int sh_type,
			      const unsigned char* prelocs,
			      size_t reloc_count,
			      unsigned char* view,
			      elfcpp::Elf_types<32>::Elf_Addr address,
			      off_t view_size)
{
  assert(sh_type == elfcpp::SHT_REL);

  gold::relocate_section<32, false, elfcpp::SHT_REL, Target_i386::Relocate>(
    relinfo,
    prelocs,
    reloc_count,
    view,
    address,
    view_size);
}

// The i386 target.

Target_i386 target_i386;

// The selector for i386 object files.

class Target_selector_i386 : public Target_selector
{
public:
  Target_selector_i386()
    : Target_selector(elfcpp::EM_386, 32, false)
  { }

  Target*
  recognize(int machine, int osabi, int abiversion) const;
};

// Recognize an i386 object file when we already know that the machine
// number is EM_386.

Target*
Target_selector_i386::recognize(int, int, int) const
{
  return &target_i386;
}

Target_selector_i386 target_selector_i386;

} // End anonymous namespace.
