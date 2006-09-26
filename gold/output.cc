// output.cc -- manage the output file for gold

#include "gold.h"

#include <cstdlib>

#include "object.h"
#include "output.h"

namespace gold
{

// Output_data methods.

Output_data::~Output_data()
{
}

// Output_data_const methods.

void
Output_data_const::write(Output_file* output, off_t off)
{
  output->write(off, data_.data(), data_.size());
}

// Output_segment_header methods.

void
Output_segment_headers::write(Output_file*, off_t)
{
  // FIXME: Unimplemented.
  abort();
}

// Output_section methods.

// Construct an Output_section.  NAME will point into a Stringpool.

Output_section::Output_section(const char* name, elfcpp::Elf_Word type,
			       elfcpp::Elf_Xword flags)
  : name_(name),
    addr_(0),
    addralign_(0),
    entsize_(0),
    offset_(0),
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

  off_t ssize = this->get_size();
  ssize = (ssize + addralign - 1) &~ (addralign - 1);

  this->set_size(ssize + shdr.get_sh_size());

  return size;
}

// Output segment methods.

Output_segment::Output_segment(elfcpp::Elf_Word type, elfcpp::Elf_Word flags)
  : output_data_(),
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
Output_segment::add_output_section(Output_section* os)
{
  // Update the segment flags.
  this->flags_ |= os->flags() & (elfcpp::PF_R | elfcpp::PF_W | elfcpp::PF_X);

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
      for (Layout::Data_list::iterator p = this->output_data_.begin();
	   p != this->output_data_.end();
	   ++p)
	{
	  if ((*p)->is_section_type(elfcpp::SHT_NOTE))
	    {
	      ++p;
	      this->output_data_.insert(p, os);
	      return;
	    }
	}
    }

  // Similarly, so that PT_TLS segments will work, we need to group
  // SHF_TLS sections.
  if ((os->flags() & elfcpp::SHF_TLS) != 0)
    {
      for (Layout::Data_list::iterator p = this->output_data_.begin();
	   p != this->output_data_.end();
	   ++p)
	{
	  if ((*p)->is_section_flag_set(elfcpp::SHF_TLS))
	    {
	      ++p;
	      this->output_data_.insert(p, os);
	      return;
	    }
	}
    }

  this->output_data_.push_back(os);
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
