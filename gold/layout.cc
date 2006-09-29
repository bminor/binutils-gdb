// layout.cc -- lay out output file sections for gold

#include "gold.h"

#include <cassert>
#include <cstring>
#include <algorithm>
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
Layout_task::run(Workqueue* workqueue)
{
  // Nothing ever frees this.
  Layout* layout = new Layout(this->options_);
  layout->init();
  for (Input_objects::Object_list::const_iterator p =
	 this->input_objects_->begin();
       p != this->input_objects_->end();
       ++p)
    (*p)->layout(layout);
  off_t file_size = layout->finalize(this->input_objects_, this->symtab_);

  // Now we know the final size of the output file and we know where
  // each piece of information goes.
  Output_file* of = new Output_file(this->options_);
  of->open(file_size);

  // Queue up the final set of tasks.
  gold::queue_final_tasks(this->options_, this->input_objects_,
			  this->symtab_, layout, workqueue, of);
}

// Layout methods.

Layout::Layout(const General_options& options)
  : options_(options), last_shndx_(0), namepool_(), sympool_(), signatures_(),
    section_name_map_(), segment_list_(), section_list_(),
    special_output_list_()
{
}

// Prepare for doing layout.

void
Layout::init()
{
  // Make space for more than enough segments for a typical file.
  // This is just for efficiency--it's OK if we wind up needing more.
  segment_list_.reserve(12);
}

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
  // We discard empty input sections.
  if (shdr.get_sh_size() == 0)
    return NULL;

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
  ++this->last_shndx_;
  Output_section* os = new Output_section(name, type, flags,
					  this->last_shndx_);

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
	      (*p)->add_output_section(os, seg_flags);
	      break;
	    }
	}

      if (p == this->segment_list_.end())
	{
	  Output_segment* oseg = new Output_segment(elfcpp::PT_LOAD,
						    seg_flags);
	  this->segment_list_.push_back(oseg);
	  oseg->add_output_section(os, seg_flags);
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
		  (*p)->add_output_section(os, seg_flags);
		  break;
		}
	    }

	  if (p == this->segment_list_.end())
	    {
	      Output_segment* oseg = new Output_segment(elfcpp::PT_NOTE,
							seg_flags);
	      this->segment_list_.push_back(oseg);
	      oseg->add_output_section(os, seg_flags);
	    }
	}

      // If we see a loadable SHF_TLS section, we create a PT_TLS
      // segment.
      if ((flags & elfcpp::SHF_TLS) != 0)
	{
	  // See if we already have an equivalent PT_TLS segment.
	  for (p = this->segment_list_.begin();
	       p != segment_list_.end();
	       ++p)
	    {
	      if ((*p)->type() == elfcpp::PT_TLS
		  && (((*p)->flags() & elfcpp::PF_W)
		      == (seg_flags & elfcpp::PF_W)))
		{
		  (*p)->add_output_section(os, seg_flags);
		  break;
		}
	    }

	  if (p == this->segment_list_.end())
	    {
	      Output_segment* oseg = new Output_segment(elfcpp::PT_TLS,
							seg_flags);
	      this->segment_list_.push_back(oseg);
	      oseg->add_output_section(os, seg_flags);
	    }
	}
    }

  return os;
}

// Find the first read-only PT_LOAD segment, creating one if
// necessary.

Output_segment*
Layout::find_first_load_seg()
{
  for (Segment_list::const_iterator p = this->segment_list_.begin();
       p != this->segment_list_.end();
       ++p)
    {
      if ((*p)->type() == elfcpp::PT_LOAD
	  && ((*p)->flags() & elfcpp::PF_R) != 0
	  && ((*p)->flags() & elfcpp::PF_W) == 0)
	return *p;
    }

  Output_segment* load_seg = new Output_segment(elfcpp::PT_LOAD, elfcpp::PF_R);
  this->segment_list_.push_back(load_seg);
  return load_seg;
}

// Finalize the layout.  When this is called, we have created all the
// output sections and all the output segments which are based on
// input sections.  We have several things to do, and we have to do
// them in the right order, so that we get the right results correctly
// and efficiently.

// 1) Finalize the list of output segments and create the segment
// table header.

// 2) Finalize the dynamic symbol table and associated sections.

// 3) Determine the final file offset of all the output segments.

// 4) Determine the final file offset of all the SHF_ALLOC output
// sections.

// 5) Create the symbol table sections and the section name table
// section.

// 6) Finalize the symbol table: set symbol values to their final
// value and make a final determination of which symbols are going
// into the output symbol table.

// 7) Create the section table header.

// 8) Determine the final file offset of all the output sections which
// are not SHF_ALLOC, including the section table header.

// 9) Finalize the ELF file header.

// This function returns the size of the output file.

off_t
Layout::finalize(const Input_objects* input_objects, Symbol_table* symtab)
{
  if (input_objects->any_dynamic())
    {
      // If there are any dynamic objects in the link, then we need
      // some additional segments: PT_PHDRS, PT_INTERP, and
      // PT_DYNAMIC.  We also need to finalize the dynamic symbol
      // table and create the dynamic hash table.
      abort();
    }

  // FIXME: Handle PT_GNU_STACK.

  Output_segment* load_seg = this->find_first_load_seg();

  // Lay out the segment headers.
  int size = input_objects->target()->get_size();
  bool big_endian = input_objects->target()->is_big_endian();
  Output_segment_headers* segment_headers;
  segment_headers = new Output_segment_headers(size, big_endian,
					       this->segment_list_);
  load_seg->add_initial_output_data(segment_headers);
  this->special_output_list_.push_back(segment_headers);
  // FIXME: Attach them to PT_PHDRS if necessary.

  // Lay out the file header.
  Output_file_header* file_header;
  file_header = new Output_file_header(size,
				       big_endian,
				       this->options_,
				       input_objects->target(),
				       symtab,
				       segment_headers);
  load_seg->add_initial_output_data(file_header);
  this->special_output_list_.push_back(file_header);

  // Set the file offsets of all the segments.
  off_t off = this->set_segment_offsets(input_objects->target(), load_seg);

  // Create the symbol table sections.
  // FIXME: We don't need to do this if we are stripping symbols.
  Output_section* osymtab;
  Output_section* ostrtab;
  this->create_symtab_sections(size, input_objects, symtab, &off,
			       &osymtab, &ostrtab);

  // Create the .shstrtab section.
  Output_section* shstrtab_section = this->create_shstrtab();

  // Set the file offsets of all the sections not associated with
  // segments.
  off = this->set_section_offsets(off);

  // Create the section table header.
  Output_section_headers* oshdrs = this->create_shdrs(size, big_endian, &off);

  file_header->set_section_info(oshdrs, shstrtab_section);

  // Now we know exactly where everything goes in the output file.

  return off;
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

// Set the file offsets of all the segments.  They have all been
// created.  LOAD_SEG must be be laid out first.  Return the offset of
// the data to follow.

off_t
Layout::set_segment_offsets(const Target* target, Output_segment* load_seg)
{
  // Sort them into the final order.
  std::sort(this->segment_list_.begin(), this->segment_list_.end(),
	    Layout::Compare_segments());

  // Find the PT_LOAD segments, and set their addresses and offsets
  // and their section's addresses and offsets.
  uint64_t addr = target->text_segment_address();
  off_t off = 0;
  bool was_readonly = false;
  for (Segment_list::iterator p = this->segment_list_.begin();
       p != this->segment_list_.end();
       ++p)
    {
      if ((*p)->type() == elfcpp::PT_LOAD)
	{
	  if (load_seg != NULL && load_seg != *p)
	    abort();
	  load_seg = NULL;

	  // If the last segment was readonly, and this one is not,
	  // then skip the address forward one page, maintaining the
	  // same position within the page.  This lets us store both
	  // segments overlapping on a single page in the file, but
	  // the loader will put them on different pages in memory.

	  uint64_t orig_addr = addr;
	  uint64_t orig_off = off;

	  uint64_t aligned_addr = addr;
	  uint64_t abi_pagesize = target->abi_pagesize();
	  if (was_readonly && ((*p)->flags() & elfcpp::PF_W) != 0)
	    {
	      uint64_t align = (*p)->max_data_align();

	      addr = (addr + align - 1) & ~ (align - 1);
	      aligned_addr = addr;
	      if ((addr & (abi_pagesize - 1)) != 0)
		addr = addr + abi_pagesize;
	    }

	  off = orig_off + ((addr - orig_addr) & (abi_pagesize - 1));
	  uint64_t new_addr = (*p)->set_section_addresses(addr, &off);

	  // Now that we know the size of this segment, we may be able
	  // to save a page in memory, at the cost of wasting some
	  // file space, by instead aligning to the start of a new
	  // page.  Here we use the real machine page size rather than
	  // the ABI mandated page size.

	  if (aligned_addr != addr)
	    {
	      uint64_t common_pagesize = target->common_pagesize();
	      uint64_t first_off = (common_pagesize
				    - (aligned_addr
				       & (common_pagesize - 1)));
	      uint64_t last_off = new_addr & (common_pagesize - 1);
	      if (first_off > 0
		  && last_off > 0
		  && ((aligned_addr & ~ (common_pagesize - 1))
		      != (new_addr & ~ (common_pagesize - 1)))
		  && first_off + last_off <= common_pagesize)
		{
		  addr = ((aligned_addr + common_pagesize - 1)
			  & ~ (common_pagesize - 1));
		  off = orig_off + ((addr - orig_addr) & (abi_pagesize - 1));
		  new_addr = (*p)->set_section_addresses(addr, &off);
		}
	    }

	  addr = new_addr;

	  if (((*p)->flags() & elfcpp::PF_W) == 0)
	    was_readonly = true;
	}
    }

  // Handle the non-PT_LOAD segments, setting their offsets from their
  // section's offsets.
  for (Segment_list::iterator p = this->segment_list_.begin();
       p != this->segment_list_.end();
       ++p)
    {
      if ((*p)->type() != elfcpp::PT_LOAD)
	(*p)->set_offset();
    }

  return off;
}

// Set the file offset of all the sections not associated with a
// segment.

off_t
Layout::set_section_offsets(off_t off)
{
  for (Layout::Section_list::iterator p = this->section_list_.begin();
       p != this->section_list_.end();
       ++p)
    {
      if ((*p)->offset() != -1)
	continue;
      uint64_t addralign = (*p)->addralign();
      if (addralign != 0)
	off = (off + addralign - 1) & ~ (addralign - 1);
      (*p)->set_address(0, off);
      off += (*p)->data_size();
    }
  return off;
}

// Create the symbol table sections.

void
Layout::create_symtab_sections(int size, const Input_objects* input_objects,
			       Symbol_table* symtab,
			       off_t* poff,
			       Output_section** posymtab,
			       Output_section** postrtab)
{
  int symsize;
  unsigned int align;
  if (size == 32)
    {
      symsize = elfcpp::Elf_sizes<32>::sym_size;
      align = 4;
    }
  else if (size == 64)
    {
      symsize = elfcpp::Elf_sizes<64>::sym_size;
      align = 8;
    }
  else
    abort();

  off_t off = *poff;
  off = (off + align - 1) & ~ (align - 1);
  off_t startoff = off;

  // Save space for the dummy symbol at the start of the section.  We
  // never bother to write this out--it will just be left as zero.
  off += symsize;

  for (Input_objects::Object_list::const_iterator p = input_objects->begin();
       p != input_objects->end();
       ++p)
    {
      Task_lock_obj<Object> tlo(**p);
      off = (*p)->finalize_local_symbols(off, &this->sympool_);
    }

  unsigned int local_symcount = (off - startoff) / symsize;
  assert(local_symcount * symsize == off - startoff);

  off = symtab->finalize(off, &this->sympool_);

  this->sympool_.set_string_offsets();

  ++this->last_shndx_;
  const char* symtab_name = this->namepool_.add(".symtab");
  Output_section* osymtab = new Output_section_symtab(symtab_name,
						      off - startoff,
						      this->last_shndx_);
  this->section_list_.push_back(osymtab);

  ++this->last_shndx_;
  const char* strtab_name = this->namepool_.add(".strtab");
  Output_section *ostrtab = new Output_section_strtab(strtab_name,
						      &this->sympool_,
						      this->last_shndx_);
  this->section_list_.push_back(ostrtab);
  this->special_output_list_.push_back(ostrtab);

  osymtab->set_address(0, startoff);
  osymtab->set_link(ostrtab->shndx());
  osymtab->set_info(local_symcount);
  osymtab->set_entsize(symsize);
  osymtab->set_addralign(align);

  *poff = off;
  *posymtab = osymtab;
  *postrtab = ostrtab;
}

// Create the .shstrtab section, which holds the names of the
// sections.  At the time this is called, we have created all the
// output sections except .shstrtab itself.

Output_section*
Layout::create_shstrtab()
{
  // FIXME: We don't need to create a .shstrtab section if we are
  // stripping everything.

  const char* name = this->namepool_.add(".shstrtab");

  this->namepool_.set_string_offsets();

  ++this->last_shndx_;
  Output_section* os = new Output_section_strtab(name,
						 &this->namepool_,
						 this->last_shndx_);

  this->section_list_.push_back(os);
  this->special_output_list_.push_back(os);

  return os;
}

// Create the section headers.  SIZE is 32 or 64.  OFF is the file
// offset.

Output_section_headers*
Layout::create_shdrs(int size, bool big_endian, off_t* poff)
{
  Output_section_headers* oshdrs;
  oshdrs = new Output_section_headers(size, big_endian, this->segment_list_,
				      this->section_list_,
				      &this->namepool_);
  uint64_t addralign = oshdrs->addralign();
  off_t off = (*poff + addralign - 1) & ~ (addralign - 1);
  oshdrs->set_address(0, off);
  off += oshdrs->data_size();
  *poff = off;
  this->special_output_list_.push_back(oshdrs);
  return oshdrs;
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

// Write out data not associated with a section or the symbol table.

void
Layout::write_data(Output_file* of) const
{
  for (Data_list::const_iterator p = this->special_output_list_.begin();
       p != this->special_output_list_.end();
       ++p)
    (*p)->write(of);
}

// Write_data_task methods.

// We can always run this task.

Task::Is_runnable_type
Write_data_task::is_runnable(Workqueue*)
{
  return IS_RUNNABLE;
}

// We need to unlock FINAL_BLOCKER when finished.

Task_locker*
Write_data_task::locks(Workqueue* workqueue)
{
  return new Task_locker_block(*this->final_blocker_, workqueue);
}

// Run the task--write out the data.

void
Write_data_task::run(Workqueue*)
{
  this->layout_->write_data(this->of_);
}

// Write_symbols_task methods.

// We can always run this task.

Task::Is_runnable_type
Write_symbols_task::is_runnable(Workqueue*)
{
  return IS_RUNNABLE;
}

// We need to unlock FINAL_BLOCKER when finished.

Task_locker*
Write_symbols_task::locks(Workqueue* workqueue)
{
  return new Task_locker_block(*this->final_blocker_, workqueue);
}

// Run the task--write out the symbols.

void
Write_symbols_task::run(Workqueue*)
{
  this->symtab_->write_globals(this->target_, this->sympool_, this->of_);
}

// Close_task methods.

// We can't run until FINAL_BLOCKER is unblocked.

Task::Is_runnable_type
Close_task::is_runnable(Workqueue*)
{
  if (this->final_blocker_->is_blocked())
    return IS_BLOCKED;
  return IS_RUNNABLE;
}

// We don't lock anything.

Task_locker*
Close_task::locks(Workqueue*)
{
  return NULL;
}

// Run the task--close the file.

void
Close_task::run(Workqueue*)
{
  this->of_->close();
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
