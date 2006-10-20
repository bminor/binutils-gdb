// elfcpp_internal.h -- internals for elfcpp   -*- C++ -*-

// This is included by elfcpp.h, the external interface, but holds
// information which we want to keep private.

#include "elfcpp_config.h"

#include <byteswap.h>

#ifndef ELFCPP_INTERNAL_H
#define ELFCPP_INTERNAL_H

namespace elfcpp
{

namespace internal
{

#ifdef WORDS_BIGENDIAN
const bool host_big_endian = true;
#else
const bool host_big_endian = false;
#endif

// Conversion routines between target and host.

// Convert Elf_Half.

template<bool same_endian>
Elf_Half
convert_half_host(Elf_Half v);

template<>
inline Elf_Half
convert_half_host<true>(Elf_Half v)
{
  return v;
}

template<>
inline Elf_Half
convert_half_host<false>(Elf_Half v)
{
  return bswap_16(v);
}

template<bool big_endian>
inline Elf_Half
convert_half(Elf_Half v)
{
  return convert_half_host<big_endian == host_big_endian>(v);
}

// Convert Elf_Word.

template<bool same_endian>
Elf_Word
convert_word_host(Elf_Word v);

template<>
inline Elf_Word
convert_word_host<true>(Elf_Word v)
{
  return v;
}

template<>
inline Elf_Word
convert_word_host<false>(Elf_Word v)
{
  return bswap_32(v);
}

template<bool big_endian>
inline Elf_Word
convert_word(Elf_Word v)
{
  return convert_word_host<big_endian == host_big_endian>(v);
}

// Convert Elf_Xword.

template<bool same_endian>
Elf_Xword
convert_xword_host(Elf_Xword v);

template<>
inline Elf_Xword
convert_xword_host<true>(Elf_Xword v)
{
  return v;
}

template<>
inline Elf_Xword
convert_xword_host<false>(Elf_Xword v)
{
  return bswap_64(v);
}

template<bool big_endian>
inline Elf_Xword
convert_xword(Elf_Xword v)
{
  return convert_xword_host<big_endian == host_big_endian>(v);
}

// Convert Elf_addr.

template<int size, bool same_endian>
typename Elf_types<size>::Elf_Addr
convert_addr_size(typename Elf_types<size>::Elf_Addr);

template<>
inline Elf_types<32>::Elf_Addr
convert_addr_size<32, true>(Elf_types<32>::Elf_Addr v)
{
  return v;
}

template<>
inline Elf_types<64>::Elf_Addr
convert_addr_size<64, true>(Elf_types<64>::Elf_Addr v)
{
  return v;
}

template<>
inline Elf_types<32>::Elf_Addr
convert_addr_size<32, false>(Elf_types<32>::Elf_Addr v)
{
  return bswap_32(v);
}

template<>
inline Elf_types<64>::Elf_Addr
convert_addr_size<64, false>(Elf_types<64>::Elf_Addr v)
{
  return bswap_64(v);
}

template<int size, bool big_endian>
inline typename Elf_types<size>::Elf_Addr
convert_addr(typename Elf_types<size>::Elf_Addr v)
{
  return convert_addr_size<size, big_endian == host_big_endian>(v);
}

// Convert Elf_Off.

template<int size, bool big_endian>
inline typename Elf_types<size>::Elf_Off
convert_off(typename Elf_types<size>::Elf_Off v)
{
  return convert_addr_size<size, big_endian == host_big_endian>(v);
}

// Convert Elf_WXword.

template<int size, bool big_endian>
inline typename Elf_types<size>::Elf_WXword
convert_wxword(typename Elf_types<size>::Elf_WXword v)
{
  return convert_addr_size<size, big_endian == host_big_endian>(v);
}

// Convert ELF_Swxword.

template<int size, bool big_endian>
inline typename Elf_types<size>::Elf_Swxword
convert_swxword(typename Elf_types<size>::Elf_Swxword v)
{
  return convert_addr_size<size, big_endian == host_big_endian>(v);
}

// The ELF file header.

template<int size>
struct Ehdr_data
{
  unsigned char e_ident[EI_NIDENT];
  Elf_Half e_type;
  Elf_Half e_machine;
  Elf_Word e_version;
  typename Elf_types<size>::Elf_Addr e_entry;
  typename Elf_types<size>::Elf_Off e_phoff;
  typename Elf_types<size>::Elf_Off e_shoff;
  Elf_Word e_flags;
  Elf_Half e_ehsize;
  Elf_Half e_phentsize;
  Elf_Half e_phnum;
  Elf_Half e_shentsize;
  Elf_Half e_shnum;
  Elf_Half e_shstrndx;
};

// An Elf section header.

template<int size>
struct Shdr_data
{
  Elf_Word sh_name;
  Elf_Word sh_type;
  typename Elf_types<size>::Elf_WXword sh_flags;
  typename Elf_types<size>::Elf_Addr sh_addr;
  typename Elf_types<size>::Elf_Off sh_offset;
  typename Elf_types<size>::Elf_WXword sh_size;
  Elf_Word sh_link;
  Elf_Word sh_info;
  typename Elf_types<size>::Elf_WXword sh_addralign;
  typename Elf_types<size>::Elf_WXword sh_entsize;
};

// An ELF segment header.  We use template specialization for the
// 32-bit and 64-bit versions because the fields are in a different
// order.

template<int size>
struct Phdr_data;

template<>
struct Phdr_data<32>
{
  Elf_Word p_type;
  Elf_types<32>::Elf_Off p_offset;
  Elf_types<32>::Elf_Addr p_vaddr;
  Elf_types<32>::Elf_Addr p_paddr;
  Elf_Word p_filesz;
  Elf_Word p_memsz;
  Elf_Word p_flags;
  Elf_Word p_align;
};

template<>
struct Phdr_data<64>
{
  Elf_Word p_type;
  Elf_Word p_flags;
  Elf_types<64>::Elf_Off p_offset;
  Elf_types<64>::Elf_Addr p_vaddr;
  Elf_types<64>::Elf_Addr p_paddr;
  Elf_Xword p_filesz;
  Elf_Xword p_memsz;
  Elf_Xword p_align;
};

// An ELF symbol table entry.  We use template specialization for the
// 32-bit and 64-bit versions because the fields are in a different
// order.

template<int size>
struct Sym_data;

template<>
struct Sym_data<32>
{
  Elf_Word st_name;
  Elf_types<32>::Elf_Addr st_value;
  Elf_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf_Half st_shndx;
};

template<>
struct Sym_data<64>
{
  Elf_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf_Half st_shndx;
  Elf_types<64>::Elf_Addr st_value;
  Elf_Xword st_size;
};

// Elf relocation table entries.

template<int size>
struct Rel_data
{
  typename Elf_types<size>::Elf_Addr r_offset;
  typename Elf_types<size>::Elf_WXword r_info;
};

template<int size>
struct Rela_data
{
  typename Elf_types<size>::Elf_Addr r_offset;
  typename Elf_types<size>::Elf_WXword r_info;
  typename Elf_types<size>::Elf_Swxword r_addend;
};

} // End namespace internal.

} // End namespace elfcpp.

#endif // !defined(ELFCPP_INTERNAL_H)
