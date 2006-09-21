// output.h -- manage the output file for gold   -*- C++ -*-

#ifndef GOLD_OUTPUT_H
#define GOLD_OUTPUT_H

#include <list>

#include "elfcpp.h"

namespace gold
{

class Object;
class Output_file;

// An abtract class for data which has to go into the output file
// which is not associated with any input section.

class Output_data
{
 public:
  Output_data(off_t size = 0)
    : size_(size)
  { }

  virtual
  ~Output_data();

  // Return the size of the data.
  off_t
  size()
  { return this->size_; }

  // Write the data to the output file at the specified offset.  This
  // must be implemented by the real class.
  virtual void
  write(Output_file*, off_t off) = 0;

 protected:
  // Set the size of the data.
  void
  set_size(off_t size)
  { this->size_ = size; }

 private:
  Output_data(const Output_data&);
  Output_data& operator=(const Output_data&);

  // Size of data in file.
  off_t size_;
};

// A simple cass of Output_data in which we have constant data to
// output.

class Output_data_const : public Output_data
{
 public:
  Output_data_const(const std::string& data)
    : Output_data(data.size()), data_(data)
  { }

  Output_data_const(const char* p, off_t len)
    : Output_data(len), data_(p, len)
  { }

  void
  write(Output_file* output, off_t off);

 private:
  std::string data_;
};

// An output section.  We don't expect to have too many output
// sections, so we don't bother to do a template on the size.

class Output_section
{
 public:
  // Create an output section, giving the name, type, and flags.
  Output_section(const char* name, elfcpp::Elf_Word, elfcpp::Elf_Xword);
  ~Output_section();

  // Add a new input section named NAME with header SHDR from object
  // OBJECT.  Return the offset within the output section.
  template<int size, bool big_endian>
  off_t
  add_input_section(Object* object, const char *name,
		    const elfcpp::Shdr<size, big_endian>& shdr);

  // Return the section name.
  const char*
  name() const
  { return this->name_; }

  // Return the section type.
  elfcpp::Elf_Word
  type() const
  { return this->type_; }

  // Return the section flags.
  elfcpp::Elf_Xword
  flags() const
  { return this->flags_; }

 private:
  // Most of these fields are only valid after layout.

  // The name of the section.  This will point into a Stringpool.
  const char* name_;
  // The section address.
  uint64_t addr_;
  // The section alignment.
  uint64_t addralign_;
  // The section entry size.
  uint64_t entsize_;
  // The file offset.
  off_t offset_;
  // The section size.
  off_t size_;
  // The section link field.
  unsigned int link_;
  // The section info field.
  unsigned int info_;
  // The section type.
  elfcpp::Elf_Word type_;
  // The section flags.
  elfcpp::Elf_Xword flags_;
};

// An output segment.  PT_LOAD segments are built from collections of
// output sections.  Other segments typically point within PT_LOAD
// segments, and are built directly as needed.

class Output_segment
{
 public:
  // Create an output segment, specifying the type and flags.
  Output_segment(elfcpp::Elf_Word, elfcpp::Elf_Word);

  // Return the virtual address.
  uint64_t
  vaddr() const
  { return this->vaddr_; }

  // Return the physical address.
  uint64_t
  paddr() const
  { return this->paddr_; }

  // Return the segment type.
  elfcpp::Elf_Word
  type() const
  { return this->type_; }

  // Return the segment flags.
  elfcpp::Elf_Word
  flags() const
  { return this->flags_; }

  // Add an Output_section to this segment.
  void
  add_output_section(Output_section*);

  // Update the segment flags to be compatible with FLAGS.
  void
  update_flags(elfcpp::Elf_Word flags)
  { this->flags_ |= flags & (elfcpp::PF_R | elfcpp::PF_W | elfcpp::PF_X); }

 private:
  Output_segment(const Output_segment&);
  Output_segment& operator=(const Output_segment&);

  typedef std::list<Output_section*> Section_list;

  // The list of output sections attached to this segment.  This is
  // cleared after layout.
  Section_list output_sections_;
  // The segment virtual address.
  uint64_t vaddr_;
  // The segment physical address.
  uint64_t paddr_;
  // The size of the segment in memory.
  uint64_t memsz_;
  // The segment alignment.
  uint64_t align_;
  // The offset of the segment data within the file.
  off_t offset_;
  // The size of the segment data in the file.
  off_t filesz_;
  // The segment type;
  elfcpp::Elf_Word type_;
  // The segment flags.
  elfcpp::Elf_Word flags_;
};

// This class represents the output file.  The output file is a
// collection of output segments and a collection of output sections
// which are not associated with segments.

class Output_file
{
 public:
  Output_file();
  ~Output_file();

  // Write data to the output file.
  void
  write(off_t off, const void* data, off_t len);
};

} // End namespace gold.

#endif // !defined(GOLD_OUTPUT_H)
