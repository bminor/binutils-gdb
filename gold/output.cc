// output.cc -- manage the output file for gold

#include "gold.h"

#include <cstdlib>
#include <algorithm>

#include "object.h"
#include "output.h"

namespace gold
{

// Output_data methods.

Output_data::~Output_data()
{
}

// Set the address and offset.

void
Output_data::set_address(uint64_t addr, off_t off)
{
  this->address_ = addr;
  this->offset_ = off;

  // Let the child class know.
  this->do_set_address(addr, off);
}

// Return the default alignment for a size--32 or 64.

uint64_t
Output_data::default_alignment(int size)
{
  if (size == 32)
    return 4;
  else if (size == 64)
    return 8;
  else
    abort();
}

// Output_data_const methods.

void
Output_data_const::do_write(Output_file* output)
{
  output->write(this->offset(), data_.data(), data_.size());
}

// Output_section_header methods.  This currently assumes that the
// segment and section lists are complete at construction time.

Output_section_headers::Output_section_headers(
    int size,
    const Layout::Segment_list& segment_list,
    const Layout::Section_list& section_list)
  : size_(size),
    segment_list_(segment_list),
    section_list_(section_list)
{
  // Count all the sections.
  off_t count = 0;
  for (Layout::Segment_list::const_iterator p = segment_list.begin();
       p != segment_list.end();
       ++p)
    count += (*p)->output_section_count();
  count += section_list.size();

  int shdr_size;
  if (size == 32)
    shdr_size = elfcpp::Elf_sizes<32>::shdr_size;
  else if (size == 64)
    shdr_size = elfcpp::Elf_sizes<64>::shdr_size;
  else
    abort();

  this->set_data_size(count * shdr_size);
}

void
Output_section_headers::do_write(Output_file*)
{
  // FIXME: Unimplemented.
  abort();
}

// Output_segment_header methods.

void
Output_segment_headers::do_write(Output_file*)
{
  // FIXME: Unimplemented.
  abort();
}

// Output_file_header methods.

Output_file_header::Output_file_header(int size,
				       const General_options& options,
				       const Target* target,
				       const Symbol_table* symtab,
				       const Output_segment_headers* osh)
  : size_(size),
    options_(options),
    target_(target),
    symtab_(symtab),
    program_header_(osh),
    section_header_(NULL),
    shstrtab_(NULL)
{
}

// Set the section table information for a file header.

void
Output_file_header::set_section_info(const Output_section_headers* shdrs,
				     const Output_section* shstrtab)
{
  this->section_header_ = shdrs;
  this->shstrtab_ = shstrtab;
}

// Write out the file header.

void
Output_file_header::do_write(Output_file*)
{
  // FIXME: Unimplemented.
  abort();
}

// Output_section methods.

// Construct an Output_section.  NAME will point into a Stringpool.

Output_section::Output_section(const char* name, elfcpp::Elf_Word type,
			       elfcpp::Elf_Xword flags)
  : name_(name),
    addralign_(0),
    entsize_(0),
    link_(0),
    info_(0),
    type_(type),
    flags_(flags)
{
}

Output_section::~Output_section()
{
}

// Add an input section to an Output_section.  We don't keep track of
// input sections for an Output_section.  Instead, each Object keeps
// track of the Output_section for each of its input sections.

template<int size, bool big_endian>
off_t
Output_section::add_input_section(Object* object, const char* secname,
				  const elfcpp::Shdr<size, big_endian>& shdr)
{
  elfcpp::Elf_Xword addralign = shdr.get_sh_addralign();
  if ((addralign & (addralign - 1)) != 0)
    {
      fprintf(stderr, _("%s: %s: invalid alignment %lu for section \"%s\"\n"),
	      program_name, object->name().c_str(),
	      static_cast<unsigned long>(addralign), secname);
      gold_exit(false);
    }

  if (addralign > this->addralign_)
    this->addralign_ = addralign;

  off_t ssize = this->data_size();
  ssize = (ssize + addralign - 1) &~ (addralign - 1);

  // SHF_TLS/SHT_NOBITS sections are handled specially: they are
  // treated as having no size and taking up no space.  We only use
  // the real size when setting the pt_memsz field of the PT_TLS
  // segment.
  if ((this->flags_ & elfcpp::SHF_TLS) == 0
      || this->type_ != elfcpp::SHT_NOBITS)
    this->set_data_size(ssize + shdr.get_sh_size());

  return size;
}

// Output_section_symtab methods.

Output_section_symtab::Output_section_symtab(const char* name, off_t size)
  : Output_section(name, elfcpp::SHT_SYMTAB, 0)
{
  this->set_data_size(size);
}

// Output_section_strtab methods.

Output_section_strtab::Output_section_strtab(const char* name,
					     Stringpool* contents)
  : Output_section(name, elfcpp::SHT_STRTAB, 0),
    contents_(contents)
{
}

void
Output_section_strtab::do_write(Output_file*)
{
  // FIXME: Unimplemented.
  abort();
}

// Output segment methods.

Output_segment::Output_segment(elfcpp::Elf_Word type, elfcpp::Elf_Word flags)
  : output_data_(),
    output_bss_(),
    vaddr_(0),
    paddr_(0),
    memsz_(0),
    align_(0),
    offset_(0),
    filesz_(0),
    type_(type),
    flags_(flags)
{
}

// Add an Output_section to an Output_segment.

void
Output_segment::add_output_section(Output_section* os,
				   elfcpp::Elf_Word seg_flags)
{
  assert((os->flags() & elfcpp::SHF_ALLOC) != 0);

  // Update the segment flags and alignment.
  this->flags_ |= seg_flags;
  uint64_t addralign = os->addralign();
  if (addralign > this->align_)
    this->align_ = addralign;

  Output_segment::Output_data_list* pdl;
  if (os->type() == elfcpp::SHT_NOBITS)
    pdl = &this->output_bss_;
  else
    pdl = &this->output_data_;

  // So that PT_NOTE segments will work correctly, we need to ensure
  // that all SHT_NOTE sections are adjacent.  This will normally
  // happen automatically, because all the SHT_NOTE input sections
  // will wind up in the same output section.  However, it is possible
  // for multiple SHT_NOTE input sections to have different section
  // flags, and thus be in different output sections, but for the
  // different section flags to map into the same segment flags and
  // thus the same output segment.

  // Note that while there may be many input sections in an output
  // section, there are normally only a few output sections in an
  // output segment.  This loop is expected to be fast.

  if (os->type() == elfcpp::SHT_NOTE)
    {
      Layout::Data_list::iterator p = pdl->end();
      do
	{
	  --p;
	  if ((*p)->is_section_type(elfcpp::SHT_NOTE))
	    {
	      ++p;
	      pdl->insert(p, os);
	      return;
	    }
	}
      while (p != pdl->begin());
    }

  // Similarly, so that PT_TLS segments will work, we need to group
  // SHF_TLS sections.  An SHF_TLS/SHT_NOBITS section is a special
  // case: we group the SHF_TLS/SHT_NOBITS sections right after the
  // SHF_TLS/SHT_PROGBITS sections.  This lets us set up PT_TLS
  // correctly.
  if ((os->flags() & elfcpp::SHF_TLS) != 0)
    {
      pdl = &this->output_data_;
      bool nobits = os->type() == elfcpp::SHT_NOBITS;
      Layout::Data_list::iterator p = pdl->end();
      do
	{
	  --p;
	  if ((*p)->is_section_flag_set(elfcpp::SHF_TLS)
	      && (nobits || !(*p)->is_section_type(elfcpp::SHT_NOBITS)))
	    {
	      ++p;
	      pdl->insert(p, os);
	      return;
	    }
	}
      while (p != pdl->begin());
    }

  pdl->push_back(os);
}

// Add an Output_data (which is not an Output_section) to the start of
// a segment.

void
Output_segment::add_initial_output_data(Output_data* od)
{
  uint64_t addralign = od->addralign();
  if (addralign > this->align_)
    this->align_ = addralign;

  this->output_data_.push_front(od);
}

// Return the maximum alignment of the Output_data in Output_segment.
// We keep this up to date as we add Output_sections and Output_data.

uint64_t
Output_segment::max_data_align() const
{
  return this->align_;
}

// Set the section addresses for an Output_segment.  ADDR is the
// address and *POFF is the file offset.  Return the address of the
// immediately following segment.  Update *POFF.

uint64_t
Output_segment::set_section_addresses(uint64_t addr, off_t* poff)
{
  assert(this->type_ == elfcpp::PT_LOAD);

  this->vaddr_ = addr;
  this->paddr_ = addr;

  off_t orig_off = *poff;
  this->offset_ = orig_off;

  addr = this->set_section_list_addresses(&this->output_data_, addr, poff);
  this->filesz_ = *poff - orig_off;

  off_t off = *poff;

  return this->set_section_list_addresses(&this->output_bss_, addr, poff);
  this->memsz_ = *poff - orig_off;

  // Ignore the file offset adjustments made by the BSS Output_data
  // objects.
  *poff = off;
}

// Set the addresses in a list of Output_data structures.

uint64_t
Output_segment::set_section_list_addresses(Output_data_list* pdl,
					   uint64_t addr, off_t* poff)
{
  off_t off = *poff;

  for (Output_data_list::iterator p = pdl->begin();
       p != pdl->end();
       ++p)
    {
      uint64_t addralign = (*p)->addralign();
      addr = (addr + addralign - 1) & ~ (addralign - 1);
      off = (off + addralign - 1) & ~ (addralign - 1);
      (*p)->set_address(addr, off);

      uint64_t size = (*p)->data_size();
      addr += size;
      off += size;
    }

  *poff = off;
  return addr;
}

// For a non-PT_LOAD segment, set the offset from the sections, if
// any.

void
Output_segment::set_offset()
{
  assert(this->type_ != elfcpp::PT_LOAD);

  if (this->output_data_.empty() && this->output_bss_.empty())
    {
      this->vaddr_ = 0;
      this->paddr_ = 0;
      this->memsz_ = 0;
      this->align_ = 0;
      this->offset_ = 0;
      this->filesz_ = 0;
      return;
    }

  const Output_data* first;
  if (this->output_data_.empty())
    first = this->output_bss_.front();
  else
    first = this->output_data_.front();
  this->vaddr_ = first->address();
  this->paddr_ = this->vaddr_;
  this->offset_ = first->offset();

  if (this->output_data_.empty())
    this->filesz_ = 0;
  else
    {
      const Output_data* last_data = this->output_data_.back();
      this->filesz_ = (last_data->address()
		       + last_data->data_size()
		       - this->vaddr_);
    }

  const Output_data* last;
  if (this->output_bss_.empty())
    last = this->output_data_.back();
  else
    last = this->output_bss_.back();
  this->memsz_ = (last->address()
		  + last->data_size()
		  - this->vaddr_);

  // this->align_ was set as we added items.
}

// Return the number of Output_sections in an Output_segment.

unsigned int
Output_segment::output_section_count() const
{
  return (this->output_section_count_list(&this->output_data_)
	  + this->output_section_count_list(&this->output_bss_));
}

// Return the number of Output_sections in an Output_data_list.

unsigned int
Output_segment::output_section_count_list(const Output_data_list* pdl) const
{
  unsigned int count = 0;
  for (Output_data_list::const_iterator p = pdl->begin();
       p != pdl->end();
       ++p)
    {
      if ((*p)->is_section())
	++count;
    }
  return count;
}

// Output_file methods.

void
Output_file::write(off_t, const void*, off_t)
{
  abort();
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

template
off_t
Output_section::add_input_section<32, false>(
    Object* object,
    const char* secname,
    const elfcpp::Shdr<32, false>& shdr);

template
off_t
Output_section::add_input_section<32, true>(
    Object* object,
    const char* secname,
    const elfcpp::Shdr<32, true>& shdr);

template
off_t
Output_section::add_input_section<64, false>(
    Object* object,
    const char* secname,
    const elfcpp::Shdr<64, false>& shdr);

template
off_t
Output_section::add_input_section<64, true>(
    Object* object,
    const char* secname,
    const elfcpp::Shdr<64, true>& shdr);

} // End namespace gold.
