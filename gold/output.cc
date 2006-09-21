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

// Output_section methods.

// Construct an Output_section.  NAME will point into a Stringpool.

Output_section::Output_section(const char* name, elfcpp::Elf_Word type,
			       elfcpp::Elf_Xword flags)
  : name_(name),
    addr_(0),
    addralign_(0),
    entsize_(0),
    offset_(0),
    size_(0),
    link_(0),
    info_(0),
    type_(type),
    flags_(flags)
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
  this->size_ = (this->size_ + addralign - 1) &~ (addralign - 1);

  if (addralign > this->addralign_)
    this->addralign_ = addralign;

  off_t ret = this->size_;
  this->size_ += shdr.get_sh_size();

  return ret;
}

// Output segment methods.

Output_segment::Output_segment(elfcpp::Elf_Word type, elfcpp::Elf_Word flags)
  : output_sections_(),
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
  // So that PT_NOTE segments will work correctly, we need to ensure
  // that all SHT_NOTE sections are adjacent.  This will normally
  // happen automatically, because all the SHT_NOTE input sections
  // will wind up in the same output section.  However, it is possible
  // for multiple SHT_NOTE input sections to have different section
  // flags, and thus be in different output sections, but for the
  // different section flags to map into the same segment flags and
  // thus the same output segment.
  if (os->type() == elfcpp::SHT_NOTE)
    {
      for (Section_list::iterator p = this->output_sections_.begin();
	   p != this->output_sections_.end();
	   ++p)
	{
	  if ((*p)->type() == elfcpp::SHT_NOTE)
	    {
	      ++p;
	      this->output_sections_.insert(p, os);
	      return;
	    }
	}
    }

  this->output_sections_.push_back(os);
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
