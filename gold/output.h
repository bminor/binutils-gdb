// output.h -- manage the output file for gold   -*- C++ -*-

#ifndef GOLD_OUTPUT_H
#define GOLD_OUTPUT_H

#include <list>

#include "elfcpp.h"
#include "layout.h"

namespace gold
{

class Object;
class Output_file;

template<int size, bool big_endian>
class Sized_target;

// An abtract class for data which has to go into the output file.

class Output_data
{
 public:
  Output_data(off_t size = 0)
    : size_(size)
  { }

  virtual
  ~Output_data();

  // Return the size of the data.  This can't be called "size" since
  // that interferes with the widely used template parameter name.
  off_t
  get_size()
  { return this->size_; }

  // Write the data to the output file at the specified offset.  This
  // must be implemented by the real class.
  virtual void
  write(Output_file*, off_t off) = 0;

  // Return whether this is an Output_section of the specified type.
  virtual bool
  is_section_type(elfcpp::Elf_Word)
  { return false; }

  // Return whether this is an Output_section with the specified flag
  // set.
  virtual bool
  is_section_flag_set(elfcpp::Elf_Xword)
  { return false; }

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

// A simple case of Output_data in which we have constant data to
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

  // Write the data to the file.
  void
  write(Output_file* output, off_t off);

 private:
  std::string data_;
};

// Output the section headers.

class Output_section_headers : public Output_data
{
 public:
  Output_section_headers(const Layout::Segment_list&,
			 const Layout::Section_list&);

  // Write the data to the file.
  void
  write(Output_file*, off_t);

 private:
  const Layout::Segment_list& segment_list_;
  const Layout::Section_list& section_list_;
};

// Output the segment headers.

class Output_segment_headers : public Output_data
{
 public:
  Output_segment_headers(const Layout::Segment_list& segment_list)
    : segment_list_(segment_list)
  { }

  // Write the data to the file.
  void
  write(Output_file*, off_t);

 private:
  const Layout::Segment_list& segment_list_;
};

// Output the ELF file header.

class Output_file_header : public Output_data
{
 public:
  Output_file_header(const General_options&,
		     const Target*,
		     const Symbol_table*,
		     const Output_segment_headers*,
		     const Output_section_headers*,
		     const Output_section* shstrtab);

  // Write the data to the file.
  void
  write(Output_file*, off_t);

 private:
  const General_options& options_;
  const Target* target_;
  const Symbol_table* symtab_;
  const Output_segment_headers* program_header_;
  const Output_section_headers* section_header_;
  const Output_section* shstrtab_;
};

// An output section.  We don't expect to have too many output
// sections, so we don't bother to do a template on the size.

class Output_section : public Output_data
{
 public:
  // Create an output section, giving the name, type, and flags.
  Output_section(const char* name, elfcpp::Elf_Word, elfcpp::Elf_Xword);
  virtual ~Output_section();

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

  // Write the data to the file.  For a typical Output_section, this
  // does nothing.  We write out the data by looping over all the
  // input sections.
  virtual void
  write(Output_file*, off_t)
  { }

  // Return whether this is a section of the specified type.
  bool
  is_section_type(elfcpp::Elf_Word type)
  { return this->type_ == type; }

  // Return whether the specified section flag is set.
  bool
  is_section_flag_set(elfcpp::Elf_Xword flag)
  { return (this->flags_ & flag) != 0; }

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
  // The section link field.
  unsigned int link_;
  // The section info field.
  unsigned int info_;
  // The section type.
  elfcpp::Elf_Word type_;
  // The section flags.
  elfcpp::Elf_Xword flags_;
};

// A special Output_section which represents the symbol table
// (SHT_SYMTAB).

class Output_section_symtab : public Output_section
{
 public:
  Output_section_symtab();
  ~Output_section_symtab();
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

 private:
  Output_segment(const Output_segment&);
  Output_segment& operator=(const Output_segment&);

  typedef std::list<Output_data*> Output_data_list;

  // The list of output sections attached to this segment.  This is
  // cleared after layout.
  Output_data_list output_data_;
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
