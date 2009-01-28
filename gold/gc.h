// gc.h -- garbage collection of unused sections

// Copyright 2009 Free Software Foundation, Inc.
// Written by Sriraman Tallam <tmsriram@google.com>.

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

#ifndef GOLD_GC_H
#define GOLD_GC_H

#include <queue>

#include "elfcpp.h"
#include "symtab.h"

namespace gold
{

class Object;

template<int size, bool big_endian>
class Sized_relobj;

template<int sh_type, int size, bool big_endian>
class Reloc_types;

class Output_section;
class General_options;
class Layout;

typedef std::pair<Object *, unsigned int> Section_id;

class Garbage_collection
{
  struct Section_id_hash
  {
    size_t operator()(const Section_id& loc) const
    { return reinterpret_cast<uintptr_t>(loc.first) ^ loc.second; }
  };

  typedef Unordered_set<Section_id, Section_id_hash> Sections_reachable;
  typedef std::map<Section_id, Sections_reachable> Section_ref;
  typedef std::queue<Section_id> Worklist_type;

  public :  
    Garbage_collection() 
      :is_worklist_ready_(false)
    { }

    // Accessor methods for the private members.

    Sections_reachable&
    referenced_list()
    { return referenced_list_; }

    Section_ref&
    section_reloc_map()
    { return section_reloc_map_; }

    Worklist_type&
    worklist()
    { return work_list_; }
 
    bool
    is_worklist_ready()
    { return is_worklist_ready_; }

    void
    worklist_ready()
    { is_worklist_ready_ = true; }

    void
    do_transitive_closure();

  private :
    Worklist_type work_list_;
    bool is_worklist_ready_;
    Section_ref section_reloc_map_;
    Sections_reachable referenced_list_;
};

// Data to pass between successive invocations of do_layout
// in object.cc while garbage collecting.  This data structure
// is filled by using the data from Read_symbols_data.

struct Symbols_data
{
  // Section headers.
  unsigned char* section_headers_data;
  // Section names.
  unsigned char* section_names_data;
  // Size of section name data in bytes.
  section_size_type section_names_size;
  // Symbol data.
  unsigned char* symbols_data;
  // Size of symbol data in bytes.
  section_size_type symbols_size;
  // Offset of external symbols within symbol data.  This structure
  // sometimes contains only external symbols, in which case this will
  // be zero.  Sometimes it contains all symbols.
  section_offset_type external_symbols_offset;
  // Symbol names.
  unsigned char* symbol_names_data;
  // Size of symbol name data in bytes.
  section_size_type symbol_names_size;
};

// This function implements the  the generic part of reloc 
// processing to map a section to all the sections it 
// references through relocs.  It is used only during garbage 
// collection.

template<int size, bool big_endian, typename Target_type, int sh_type,
	 typename Scan>
inline void
gc_process_relocs(
    const General_options& ,
    Symbol_table* symtab,
    Layout*,
    Target_type* ,
    Sized_relobj<size, big_endian>* object,
    unsigned int data_shndx,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section*,
    bool ,
    size_t local_count,
    const unsigned char* plocal_syms)
{
  Object *src_obj, *dst_obj;
  unsigned int src_indx, dst_indx;

  src_obj = object;
  src_indx = data_shndx; 
  
  typedef typename Reloc_types<sh_type, size, big_endian>::Reloc Reltype;
  const int reloc_size = Reloc_types<sh_type, size, big_endian>::reloc_size;
  const int sym_size = elfcpp::Elf_sizes<size>::sym_size;

  for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
    {
      Reltype reloc(prelocs);
      typename elfcpp::Elf_types<size>::Elf_WXword r_info = reloc.get_r_info();
      unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
      
      if (r_sym < local_count)
        {
          gold_assert(plocal_syms != NULL);
          typename elfcpp::Sym<size, big_endian> lsym(plocal_syms
                                                      + r_sym * sym_size);
          unsigned int shndx = lsym.get_st_shndx();
          bool is_ordinary;
          shndx = object->adjust_sym_shndx(r_sym, shndx, &is_ordinary);
          if (!is_ordinary) 
            continue;
          dst_obj = src_obj;
          if (shndx == src_indx) 
            continue;
          dst_indx = shndx;
        }
      else
        {
          Symbol* gsym = object->global_symbol(r_sym);
          gold_assert(gsym != NULL);
          if (gsym->is_forwarder())
            gsym = symtab->resolve_forwards(gsym);
          if (gsym->source() != Symbol::FROM_OBJECT)
            continue;
          bool is_ordinary;
          dst_obj = gsym->object();
          dst_indx = gsym->shndx(&is_ordinary);
          if (!is_ordinary)
            continue;
        }
      Section_id p1(src_obj, src_indx);
      Section_id p2(dst_obj, dst_indx);
      Garbage_collection::Section_ref::iterator map_it;
      map_it = symtab->gc()->section_reloc_map().find(p1);
      if (map_it == symtab->gc()->section_reloc_map().end())
        {
          symtab->gc()->section_reloc_map()[p1].insert(p2);
        }
      else
        {
          Garbage_collection::Sections_reachable& v(map_it->second);
          v.insert(p2);
        }
    }
  return;
}

} // End of namespace gold.

#endif
