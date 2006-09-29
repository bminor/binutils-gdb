// target-reloc.h -- target specific relocation support  -*- C++ -*-

#ifndef GOLD_TARGET_RELOC_H
#define GOLD_TARGET_RELOC_H

#include "elfcpp.h"
#include "symtab.h"

namespace gold
{

// Pick the ELF relocation accessor class and the size based on
// SH_TYPE, which is either SHT_REL or SHT_RELA.

template<int sh_type, int size, bool big_endian>
struct Reloc_types;

template<int size, bool big_endian>
struct Reloc_types<elfcpp::SHT_REL, size, big_endian>
{
  typedef typename elfcpp::Rel<size, big_endian> Reloc;
  static const int reloc_size = elfcpp::Elf_sizes<size>::rel_size;
};

template<int size, bool big_endian>
struct Reloc_types<elfcpp::SHT_RELA, size, big_endian>
{
  typedef typename elfcpp::Rela<size, big_endian> Reloc;
  static const int reloc_size = elfcpp::Elf_sizes<size>::rela_size;
};

// This function implements the generic part of relocation handling.
// This is an inline function which take a class whose operator()
// implements the machine specific part of relocation.  We do it this
// way to avoid making a function call for each relocation, and to
// avoid repeating the generic relocation handling code for each
// target.

// SIZE is the ELF size: 32 or 64.  BIG_ENDIAN is the endianness of
// the data.  SH_TYPE is the section type: SHT_REL or SHT_RELA.  RELOC
// implements operator() to do a relocation.

// OBJECT is the object for we are processing relocs.  SH_TYPE is the
// type of relocation: SHT_REL or SHT_RELA.  PRELOCS points to the
// relocation data.  RELOC_COUNT is the number of relocs.  LOCAL_COUNT
// is the number of local symbols.  LOCAL_VALUES holds the values of
// the local symbols.  GLOBAL_SYMS points to the global symbols.  VIEW
// is the section data, VIEW_ADDRESS is its memory address, and
// VIEW_SIZE is the size.

template<int size, bool big_endian, int sh_type, typename Relocate>
inline void
relocate_section(
    const Symbol_table* symtab,
    Sized_object<size, big_endian>* object,
    const unsigned char* prelocs,
    size_t reloc_count,
    size_t local_count,
    const typename elfcpp::Elf_types<size>::Elf_Addr* local_values,
    Symbol** global_syms,
    unsigned char* view,
    typename elfcpp::Elf_types<size>::Elf_Addr view_address,
    off_t view_size)
{
  typedef typename Reloc_types<sh_type, size, big_endian>::Reloc Reltype;
  const int reloc_size = Reloc_types<sh_type, size, big_endian>::reloc_size;
  Relocate relocate;

  for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
    {
      Reltype reloc(prelocs);

      off_t offset = reloc.get_r_offset();
      if (offset < 0 || offset >= view_size)
	{
	  fprintf(stderr, _("%s: %s: reloc %zu has bad offset %lu\n"),
		  program_name, object->name().c_str(), i,
		  static_cast<unsigned long>(offset));
	  gold_exit(false);
	}

      typename elfcpp::Elf_types<size>::Elf_WXword r_info = reloc.get_r_info();
      unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
      unsigned int r_type = elfcpp::elf_r_type<size>(r_info);

      Sized_symbol<size>* sym;
      typename elfcpp::Elf_types<size>::Elf_Addr value;

      if (r_sym < local_count)
	{
	  sym = NULL;
	  value = local_values[r_sym];
	}
      else
	{
	  Symbol* gsym = global_syms[r_sym - local_count];
	  if (gsym->is_forwarder())
	    gsym = symtab->resolve_forwards(gsym);

	  sym = static_cast<Sized_symbol<size>*>(gsym);
	  value = sym->value();

	  if (sym->shnum() == elfcpp::SHN_UNDEF
	      && sym->binding() != elfcpp::STB_WEAK)
	    {
	      fprintf(stderr, _("%s: %s: undefined reference to '%s'\n"),
		      program_name, object->name().c_str(), sym->name());
	      // gold_exit(false);
	    }
	}

      relocate(object, reloc, r_type, sym, value, view + offset,
	       view_address + offset);
    }
}

} // End namespace gold.

#endif // !defined(GOLD_TARGET_RELOC_H)
