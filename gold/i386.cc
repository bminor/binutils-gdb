// i386.cc -- i386 target support for gold.

#include "gold.h"
#include "elfcpp.h"
#include "i386.h"
#include "object.h"
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

  void
  relocate_section(const Symbol_table* symtab,
		   Sized_object<32, false>*,
		   unsigned int,
		   const unsigned char*,
		   size_t,
		   unsigned int,
		   const elfcpp::Elf_types<32>::Elf_Addr*,
		   Symbol**,
		   unsigned char*,
		   elfcpp::Elf_types<32>::Elf_Addr,
		   off_t);

  // The class which implements relocation.
  struct Relocate
  {
    inline void
    operator()(Sized_object<32, false>*, const elfcpp::Rel<32, false>&,
	       unsigned int r_type, Sized_symbol<32>*,
	       elfcpp::Elf_types<32>::Elf_Addr,
	       unsigned char*, elfcpp::Elf_types<32>::Elf_Addr);

  };

 private:
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

// Perform a relocation.

inline void
Target_i386::Relocate::operator()(Sized_object<32, false>* object,
				  const elfcpp::Rel<32, false>&,
				  unsigned int r_type,
				  Sized_symbol<32>*,
				  elfcpp::Elf_types<32>::Elf_Addr value,
				  unsigned char* view,
				  elfcpp::Elf_types<32>::Elf_Addr address)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
      break;

    case elfcpp::R_386_32:
      {
	elfcpp::Elf_Word* wv = reinterpret_cast<elfcpp::Elf_Word*>(view);
	unsigned int x = elfcpp::read_elf_word<false>(wv);
	elfcpp::write_elf_word<false>(wv, x + value);
      }
      break;

    case elfcpp::R_386_PC32:
      {
	elfcpp::Elf_Word* wv = reinterpret_cast<elfcpp::Elf_Word*>(view);
	unsigned int x = elfcpp::read_elf_word<false>(wv);
	elfcpp::write_elf_word<false>(wv, x + value - address);
      }
      break;

    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name, object->name().c_str(), r_type);
      // gold_exit(false);
    }
}

// Relocate section data.

void
Target_i386::relocate_section(const Symbol_table* symtab,
			      Sized_object<32, false>* object,
			      unsigned int sh_type,
			      const unsigned char* prelocs,
			      size_t reloc_count,
			      unsigned int local_count,
			      const elfcpp::Elf_types<32>::Elf_Addr* values,
			      Symbol** global_syms,
			      unsigned char* view,
			      elfcpp::Elf_types<32>::Elf_Addr address,
			      off_t view_size)
{
  if (sh_type == elfcpp::SHT_RELA)
    {
      fprintf(stderr, _("%s: %s: unsupported RELA reloc section\n"),
	      program_name, object->name().c_str());
      gold_exit(false);
    }

  gold::relocate_section<32, false, elfcpp::SHT_REL, Target_i386::Relocate>(
    symtab,
    object,
    prelocs,
    reloc_count,
    local_count,
    values,
    global_syms,
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
