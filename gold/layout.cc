// layout.cc -- lay out output file sections for gold

#include "gold.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>

#include "output.h"
#include "layout.h"

namespace gold
{

// Layout_task methods.

Layout_task::~Layout_task()
{
}

// This task can be run when it is unblocked.

Task::Is_runnable_type
Layout_task::is_runnable(Workqueue*)
{
  if (this->this_blocker_->is_blocked())
    return IS_BLOCKED;
  return IS_RUNNABLE;
}

// We don't need to hold any locks for the duration of this task.  In
// fact this task will be the only one running.

Task_locker*
Layout_task::locks(Workqueue*)
{
  return NULL;
}

// Lay out the sections.  This is called after all the input objects
// have been read.

void
Layout_task::run(Workqueue*)
{
  Layout layout(this->options_);
  for (Object_list::const_iterator p = this->input_objects_->begin();
       p != this->input_objects_->end();
       ++p)
    (*p)->layout(&layout);
}

// Layout methods.

// Hash a key we use to look up an output section mapping.

size_t
Layout::Hash_key::operator()(const Layout::Key& k) const
{
 return reinterpret_cast<size_t>(k.first) + k.second.first + k.second.second;
}

// Whether to include this section in the link.

template<int size, bool big_endian>
bool
Layout::include_section(Object*, const char*,
			const elfcpp::Shdr<size, big_endian>& shdr)
{
  // Some section types are never linked.  Some are only linked when
  // doing a relocateable link.
  switch (shdr.get_sh_type())
    {
    case elfcpp::SHT_NULL:
    case elfcpp::SHT_SYMTAB:
    case elfcpp::SHT_DYNSYM:
    case elfcpp::SHT_STRTAB:
    case elfcpp::SHT_HASH:
    case elfcpp::SHT_DYNAMIC:
    case elfcpp::SHT_SYMTAB_SHNDX:
      return false;

    case elfcpp::SHT_RELA:
    case elfcpp::SHT_REL:
    case elfcpp::SHT_GROUP:
      return this->options_.is_relocatable();

    default:
      // FIXME: Handle stripping debug sections here.
      return true;
    }
}

// Return the output section to use for input section NAME, with
// header HEADER, from object OBJECT.  Set *OFF to the offset of this
// input section without the output section.

template<int size, bool big_endian>
Output_section*
Layout::layout(Object* object, const char* name,
	       const elfcpp::Shdr<size, big_endian>& shdr, off_t* off)
{
  if (!this->include_section(object, name, shdr))
    return NULL;

  // Unless we are doing a relocateable link, .gnu.linkonce sections
  // are laid out as though they were named for the sections are
  // placed into.
  if (!this->options_.is_relocatable() && Layout::is_linkonce(name))
    name = Layout::linkonce_output_name(name);

  // FIXME: Handle SHF_OS_NONCONFORMING here.

  // Canonicalize the section name.
  name = this->namepool_.add(name);

  // Find the output section.  The output section is selected based on
  // the section name, type, and flags.

  // FIXME: If we want to do relaxation, we need to modify this
  // algorithm.  We also build a list of input sections for each
  // output section.  Then we relax all the input sections.  Then we
  // walk down the list and adjust all the offsets.

  elfcpp::Elf_Word type = shdr.get_sh_type();
  elfcpp::Elf_Xword flags = shdr.get_sh_flags();
  const Key key(name, std::make_pair(type, flags));
  const std::pair<Key, Output_section*> v(key, NULL);
  std::pair<Section_name_map::iterator, bool> ins(
    this->section_name_map_.insert(v));

  Output_section* os;
  if (!ins.second)
    os = ins.first->second;
  else
    {
      // This is the first time we've seen this name/type/flags
      // combination.
      os = this->make_output_section(name, type, flags);
      ins.first->second = os;
    }

  // FIXME: Handle SHF_LINK_ORDER somewhere.

  *off = os->add_input_section(object, name, shdr);

  return os;
}

// Return whether SEG1 should be before SEG2 in the output file.  This
// is based entirely on the segment type and flags.  When this is
// called the segment addresses has normally not yet been set.

bool
Layout::segment_precedes(const Output_segment* seg1,
			 const Output_segment* seg2)
{
  elfcpp::Elf_Word type1 = seg1->type();
  elfcpp::Elf_Word type2 = seg2->type();

  // The single PT_PHDR segment is required to precede any loadable
  // segment.  We simply make it always first.
  if (type1 == elfcpp::PT_PHDR)
    {
      assert(type2 != elfcpp::PT_PHDR);
      return true;
    }
  if (type2 == elfcpp::PT_PHDR)
    return false;

  // The single PT_INTERP segment is required to precede any loadable
  // segment.  We simply make it always second.
  if (type1 == elfcpp::PT_INTERP)
    {
      assert(type2 != elfcpp::PT_INTERP);
      return true;
    }
  if (type2 == elfcpp::PT_INTERP)
    return false;

  // We then put PT_LOAD segments before any other segments.
  if (type1 == elfcpp::PT_LOAD && type2 != elfcpp::PT_LOAD)
    return true;
  if (type2 == elfcpp::PT_LOAD && type1 != elfcpp::PT_LOAD)
    return false;

  const elfcpp::Elf_Word flags1 = seg1->flags();
  const elfcpp::Elf_Word flags2 = seg2->flags();

  // The order of non-PT_LOAD segments is unimportant.  We simply sort
  // by the numeric segment type and flags values.  There should not
  // be more than one segment with the same type and flags.
  if (type1 != elfcpp::PT_LOAD)
    {
      if (type1 != type2)
	return type1 < type2;
      assert(flags1 != flags2);
      return flags1 < flags2;
    }

  // We sort PT_LOAD segments based on the flags.  Readonly segments
  // come before writable segments.  Then executable segments come
  // before non-executable segments.  Then the unlikely case of a
  // non-readable segment comes before the normal case of a readable
  // segment.  If there are multiple segments with the same type and
  // flags, we require that the address be set, and we sort by
  // virtual address and then physical address.
  if ((flags1 & elfcpp::PF_W) != (flags2 & elfcpp::PF_W))
    return (flags1 & elfcpp::PF_W) == 0;
  if ((flags1 & elfcpp::PF_X) != (flags2 & elfcpp::PF_X))
    return (flags1 & elfcpp::PF_X) != 0;
  if ((flags1 & elfcpp::PF_R) != (flags2 & elfcpp::PF_R))
    return (flags1 & elfcpp::PF_R) == 0;

  uint64_t vaddr1 = seg1->vaddr();
  uint64_t vaddr2 = seg2->vaddr();
  if (vaddr1 != vaddr2)
    return vaddr1 < vaddr2;

  uint64_t paddr1 = seg1->paddr();
  uint64_t paddr2 = seg2->paddr();
  assert(paddr1 != paddr2);
  return paddr1 < paddr2;
}

// Map section flags to segment flags.

elfcpp::Elf_Word
Layout::section_flags_to_segment(elfcpp::Elf_Xword flags)
{
  elfcpp::Elf_Word ret = elfcpp::PF_R;
  if ((flags & elfcpp::SHF_WRITE) != 0)
    ret |= elfcpp::PF_W;
  if ((flags & elfcpp::SHF_EXECINSTR) != 0)
    ret |= elfcpp::PF_X;
  return ret;
}

// Make a new Output_section, and attach it to segments as
// appropriate.

Output_section*
Layout::make_output_section(const char* name, elfcpp::Elf_Word type,
			    elfcpp::Elf_Xword flags)
{
  Output_section* os = new Output_section(name, type, flags);

  if ((flags & elfcpp::SHF_ALLOC) == 0)
    this->section_list_.push_back(os);
  else
    {
      // This output section goes into a PT_LOAD segment.

      elfcpp::Elf_Word seg_flags = Layout::section_flags_to_segment(flags);

      // The only thing we really care about for PT_LOAD segments is
      // whether or not they are writable, so that is how we search
      // for them.  People who need segments sorted on some other
      // basis will have to wait until we implement a mechanism for
      // them to describe the segments they want.

      Segment_list::const_iterator p;
      for (p = this->segment_list_.begin();
	   p != this->segment_list_.end();
	   ++p)
	{
	  if ((*p)->type() == elfcpp::PT_LOAD
	      && ((*p)->flags() & elfcpp::PF_W) == (seg_flags & elfcpp::PF_W))
	    {
	      (*p)->add_output_section(os);
	      if ((*p)->flags() != seg_flags)
		(*p)->update_flags(seg_flags);
	      break;
	    }
	}

      if (p == this->segment_list_.end())
	{
	  Output_segment* oseg = new Output_segment(elfcpp::PT_LOAD,
						    seg_flags);
	  this->segment_list_.push_back(oseg);
	  oseg->add_output_section(os);
	}

      // If we see a loadable SHT_NOTE section, we create a PT_NOTE
      // segment.
      if (type == elfcpp::SHT_NOTE)
	{
	  // See if we already have an equivalent PT_NOTE segment.
	  for (p = this->segment_list_.begin();
	       p != segment_list_.end();
	       ++p)
	    {
	      if ((*p)->type() == elfcpp::PT_NOTE
		  && (((*p)->flags() & elfcpp::PF_W)
		      == (seg_flags & elfcpp::PF_W)))
		{
		  (*p)->add_output_section(os);
		  if ((*p)->flags() != seg_flags)
		    (*p)->update_flags(seg_flags);
		  break;
		}
	    }

	  if (p == this->segment_list_.end())
	    {
	      Output_segment* oseg = new Output_segment(elfcpp::PT_NOTE,
							seg_flags);
	      this->segment_list_.push_back(oseg);
	      oseg->add_output_section(os);
	    }
	}
    }

  return os;
}

// The mapping of .gnu.linkonce section names to real section names.

#define MAPPING_INIT(f, t) { f, sizeof(f) - 1, t }
const Layout::Linkonce_mapping Layout::linkonce_mapping[] =
{
  MAPPING_INIT("d.rel.ro", ".data.rel.ro"),	// Must be before "d".
  MAPPING_INIT("t", ".text"),
  MAPPING_INIT("r", ".rodata"),
  MAPPING_INIT("d", ".data"),
  MAPPING_INIT("b", ".bss"),
  MAPPING_INIT("s", ".sdata"),
  MAPPING_INIT("sb", ".sbss"),
  MAPPING_INIT("s2", ".sdata2"),
  MAPPING_INIT("sb2", ".sbss2"),
  MAPPING_INIT("wi", ".debug_info"),
  MAPPING_INIT("td", ".tdata"),
  MAPPING_INIT("tb", ".tbss"),
  MAPPING_INIT("lr", ".lrodata"),
  MAPPING_INIT("l", ".ldata"),
  MAPPING_INIT("lb", ".lbss"),
};
#undef MAPPING_INIT

const int Layout::linkonce_mapping_count =
  sizeof(Layout::linkonce_mapping) / sizeof(Layout::linkonce_mapping[0]);

// Return the name of the output section to use for a .gnu.linkonce
// section.  This is based on the default ELF linker script of the old
// GNU linker.  For example, we map a name like ".gnu.linkonce.t.foo"
// to ".text".

const char*
Layout::linkonce_output_name(const char* name)
{
  const char* s = name + sizeof(".gnu.linkonce") - 1;
  if (*s != '.')
    return name;
  ++s;
  const Linkonce_mapping* plm = linkonce_mapping;
  for (int i = 0; i < linkonce_mapping_count; ++i, ++plm)
    {
      if (strncmp(s, plm->from, plm->fromlen) == 0 && s[plm->fromlen] == '.')
	return plm->to;
    }
  return name;
}

// Record the signature of a comdat section, and return whether to
// include it in the link.  If GROUP is true, this is a regular
// section group.  If GROUP is false, this is a group signature
// derived from the name of a linkonce section.  We want linkonce
// signatures and group signatures to block each other, but we don't
// want a linkonce signature to block another linkonce signature.

bool
Layout::add_comdat(const char* signature, bool group)
{
  std::string sig(signature);
  std::pair<Signatures::iterator, bool> ins(
    this->signatures_.insert(std::make_pair(signature, group)));

  if (ins.second)
    {
      // This is the first time we've seen this signature.
      return true;
    }

  if (ins.first->second)
    {
      // We've already seen a real section group with this signature.
      return false;
    }
  else if (group)
    {
      // This is a real section group, and we've already seen a
      // linkonce section with tihs signature.  Record that we've seen
      // a section group, and don't include this section group.
      ins.first->second = true;
      return false;
    }
  else
    {
      // We've already seen a linkonce section and this is a linkonce
      // section.  These don't block each other--this may be the same
      // symbol name with different section types.
      return true;
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

template
Output_section*
Layout::layout<32, false>(Object* object, const char* name,
			  const elfcpp::Shdr<32, false>& shdr, off_t*);

template
Output_section*
Layout::layout<32, true>(Object* object, const char* name,
			 const elfcpp::Shdr<32, true>& shdr, off_t*);

template
Output_section*
Layout::layout<64, false>(Object* object, const char* name,
			  const elfcpp::Shdr<64, false>& shdr, off_t*);

template
Output_section*
Layout::layout<64, true>(Object* object, const char* name,
			 const elfcpp::Shdr<64, true>& shdr, off_t*);


} // End namespace gold.
