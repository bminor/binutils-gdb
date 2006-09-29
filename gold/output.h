// output.h -- manage the output file for gold   -*- C++ -*-

#ifndef GOLD_OUTPUT_H
#define GOLD_OUTPUT_H

#include <cassert>
#include <list>

#include "elfcpp.h"
#include "layout.h"

namespace gold
{

class General_options;
class Object;
class Output_file;

template<int size, bool big_endian>
class Sized_target;

// An abtract class for data which has to go into the output file.

class Output_data
{
 public:
  explicit Output_data(off_t data_size = 0)
    : address_(0), data_size_(data_size), offset_(-1)
  { }

  virtual
  ~Output_data();

  // Return the address.
  uint64_t
  address() const
  { return this->address_; }

  // Return the size of the data.
  off_t
  data_size() const
  { return this->data_size_; }

  // Return the file offset.
  off_t
  offset() const
  { return this->offset_; }

  // Return the required alignment.
  uint64_t
  addralign() const
  { return this->do_addralign(); }

  // Return whether this is an Output_section.
  bool
  is_section() const
  { return this->do_is_section(); }

  // Return whether this is an Output_section of the specified type.
  bool
  is_section_type(elfcpp::Elf_Word stt) const
  { return this->do_is_section_type(stt); }

  // Return whether this is an Output_section with the specified flag
  // set.
  bool
  is_section_flag_set(elfcpp::Elf_Xword shf) const
  { return this->do_is_section_flag_set(shf); }

  // Set the address and file offset of this data.
  void
  set_address(uint64_t addr, off_t off);

  // Write the data to the output file.
  void
  write(Output_file* file)
  { this->do_write(file); }

 protected:
  // Functions that child classes may or in some cases must implement.

  // Write the data to the output file.
  virtual void
  do_write(Output_file*) = 0;

  // Return the required alignment.
  virtual uint64_t
  do_addralign() const = 0;

  // Return whether this is an Output_section.
  virtual bool
  do_is_section() const
  { return false; }

  // Return whether this is an Output_section of the specified type.
  // This only needs to be implement by Output_section.
  virtual bool
  do_is_section_type(elfcpp::Elf_Word) const
  { return false; }

  // Return whether this is an Output_section with the specific flag
  // set.  This only needs to be implemented by Output_section.
  virtual bool
  do_is_section_flag_set(elfcpp::Elf_Xword) const
  { return false; }

  // Set the address and file offset of the data.  This only needs to
  // be implemented if the child needs to know.
  virtual void
  do_set_address(uint64_t, off_t)
  { }

  // Functions that child classes may call.

  // Set the size of the data.
  void
  set_data_size(off_t data_size)
  { this->data_size_ = data_size; }

  // Return default alignment for a size--32 or 64.
  static uint64_t
  default_alignment(int size);

 private:
  Output_data(const Output_data&);
  Output_data& operator=(const Output_data&);

  // Memory address in file (not always meaningful).
  uint64_t address_;
  // Size of data in file.
  off_t data_size_;
  // Offset within file.
  off_t offset_;
};

// A simple case of Output_data in which we have constant data to
// output.

class Output_data_const : public Output_data
{
 public:
  Output_data_const(const std::string& data, uint64_t addralign)
    : Output_data(data.size()), data_(data), addralign_(addralign)
  { }

  Output_data_const(const char* p, off_t len, uint64_t addralign)
    : Output_data(len), data_(p, len), addralign_(addralign)
  { }

  // Write the data to the file.
  void
  do_write(Output_file* output);

  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return this->addralign_; }

 private:
  std::string data_;
  uint64_t addralign_;
};

// Output the section headers.

class Output_section_headers : public Output_data
{
 public:
  Output_section_headers(int size,
			 bool big_endian,
			 const Layout::Segment_list&,
			 const Layout::Section_list&,
			 const Stringpool*);

  // Write the data to the file.
  void
  do_write(Output_file*);

  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return Output_data::default_alignment(this->size_); }

 private:
  // Write the data to the file with the right size and endianness.
  template<int size, bool big_endian>
  void
  do_sized_write(Output_file*);

  int size_;
  bool big_endian_;
  const Layout::Segment_list& segment_list_;
  const Layout::Section_list& section_list_;
  const Stringpool* secnamepool_;
};

// Output the segment headers.

class Output_segment_headers : public Output_data
{
 public:
  Output_segment_headers(int size, bool big_endian,
			 const Layout::Segment_list& segment_list);

  // Write the data to the file.
  void
  do_write(Output_file*);

  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return Output_data::default_alignment(this->size_); }

 private:
  // Write the data to the file with the right size and endianness.
  template<int size, bool big_endian>
  void
  do_sized_write(Output_file*);

  int size_;
  bool big_endian_;
  const Layout::Segment_list& segment_list_;
};

// Output the ELF file header.

class Output_file_header : public Output_data
{
 public:
  Output_file_header(int size,
		     bool big_endian,
		     const General_options&,
		     const Target*,
		     const Symbol_table*,
		     const Output_segment_headers*);

  // Add information about the section headers.  We lay out the ELF
  // file header before we create the section headers.
  void set_section_info(const Output_section_headers*,
			const Output_section* shstrtab);

  // Write the data to the file.
  void
  do_write(Output_file*);

  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return Output_data::default_alignment(this->size_); }

  // Set the address and offset--we only implement this for error
  // checking.
  void
  do_set_address(uint64_t, off_t off) const
  { assert(off == 0); }

 private:
  // Write the data to the file with the right size and endianness.
  template<int size, bool big_endian>
  void
  do_sized_write(Output_file*);

  int size_;
  bool big_endian_;
  const General_options& options_;
  const Target* target_;
  const Symbol_table* symtab_;
  const Output_segment_headers* segment_header_;
  const Output_section_headers* section_header_;
  const Output_section* shstrtab_;
};

// An output section.  We don't expect to have too many output
// sections, so we don't bother to do a template on the size.

class Output_section : public Output_data
{
 public:
  // Create an output section, giving the name, type, and flags.
  Output_section(const char* name, elfcpp::Elf_Word, elfcpp::Elf_Xword,
		 unsigned int shndx);
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

  // Return the address alignment.
  uint64_t
  addralign() const
  { return this->addralign_; }

  // Return the section index.
  unsigned int
  shndx() const
  { return this->shndx_; }

  // Set the entsize field.
  void
  set_entsize(uint64_t v)
  { this->entsize_ = v; }

  // Set the link field.
  void
  set_link(unsigned int v)
  { this->link_ = v; }

  // Set the info field.
  void
  set_info(unsigned int v)
  { this->info_ = v; }

  // Set the addralign field.
  void
  set_addralign(uint64_t v)
  { this->addralign_ = v; }

  // Write the data to the file.  For a typical Output_section, this
  // does nothing.  We write out the data by looping over all the
  // input sections.
  virtual void
  do_write(Output_file*)
  { }

  // Return the address alignment--function required by parent class.
  uint64_t
  do_addralign() const
  { return this->addralign_; }

  // Return whether this is an Output_section.
  bool
  do_is_section() const
  { return true; }

  // Return whether this is a section of the specified type.
  bool
  do_is_section_type(elfcpp::Elf_Word type) const
  { return this->type_ == type; }

  // Return whether the specified section flag is set.
  bool
  do_is_section_flag_set(elfcpp::Elf_Xword flag) const
  { return (this->flags_ & flag) != 0; }

  // Write the section header into *OPHDR.
  template<int size, bool big_endian>
  void
  write_header(const Stringpool*, elfcpp::Shdr_write<size, big_endian>*) const;

 private:
  // Most of these fields are only valid after layout.

  // The name of the section.  This will point into a Stringpool.
  const char* name_;
  // The section address is in the parent class.
  // The section alignment.
  uint64_t addralign_;
  // The section entry size.
  uint64_t entsize_;
  // The file offset is in the parent class.
  // The section link field.
  unsigned int link_;
  // The section info field.
  unsigned int info_;
  // The section type.
  elfcpp::Elf_Word type_;
  // The section flags.
  elfcpp::Elf_Xword flags_;
  // The section index.
  unsigned int shndx_;
};

// A special Output_section which represents the symbol table
// (SHT_SYMTAB).

class Output_section_symtab : public Output_section
{
 public:
  Output_section_symtab(const char* name, off_t size, unsigned int shndx);
};

// A special Output_section which holds a string table.

class Output_section_strtab : public Output_section
{
 public:
  Output_section_strtab(const char* name, Stringpool* contents,
			unsigned int shndx);

  // Write out the data.
  void
  do_write(Output_file*);

 private:
  Stringpool* contents_;
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

  // Return the maximum alignment of the Output_data.
  uint64_t
  max_data_align() const;

  // Add an Output_section to this segment.
  void
  add_output_section(Output_section*, elfcpp::Elf_Word seg_flags);

  // Add an Output_data (which is not an Output_section) to the start
  // of this segment.
  void
  add_initial_output_data(Output_data*);

  // Set the address of the segment to ADDR and the offset to *POFF
  // (aligned if necessary), and set the addresses and offsets of all
  // contained output sections accordingly.  Return the address of the
  // immediately following segment.  Update *POFF.  This should only
  // be called for a PT_LOAD segment.
  uint64_t
  set_section_addresses(uint64_t addr, off_t* poff);

  // Set the offset of this segment based on the section.  This should
  // only be called for a non-PT_LOAD segment.
  void
  set_offset();

  // Return the number of output sections.
  unsigned int
  output_section_count() const;

  // Write the segment header into *OPHDR.
  template<int size, bool big_endian>
  void
  write_header(elfcpp::Phdr_write<size, big_endian>*) const;

  // Write the section headers of associated sections into V.
  template<int size, bool big_endian>
  unsigned char*
  write_section_headers(const Stringpool*,
                        unsigned char* v ACCEPT_SIZE_ENDIAN) const;

 private:
  Output_segment(const Output_segment&);
  Output_segment& operator=(const Output_segment&);

  typedef std::list<Output_data*> Output_data_list;

  // Set the section addresses in an Output_data_list.
  uint64_t
  set_section_list_addresses(Output_data_list*, uint64_t addr, off_t* poff);

  // Return the number of Output_sections in an Output_data_list.
  unsigned int
  output_section_count_list(const Output_data_list*) const;

  // Write the section headers in the list into V.
  template<int size, bool big_endian>
  unsigned char*
  write_section_headers_list(const Stringpool*, const Output_data_list*,
			     unsigned char* v ACCEPT_SIZE_ENDIAN) const;

  // The list of output data with contents attached to this segment.
  Output_data_list output_data_;
  // The list of output data without contents attached to this segment.
  Output_data_list output_bss_;
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

// This class represents the output file.

class Output_file
{
 public:
  Output_file(const General_options& options);

  // Open the output file.  FILE_SIZE is the final size of the file.
  void
  open(off_t file_size);

  // Close the output file and make sure there are no error.
  void
  close();

  // We currently always use mmap which makes the view handling quite
  // simple.  In the future we may support other approaches.

  // Write data to the output file.
  void
  write(off_t offset, const void* data, off_t len)
  { memcpy(this->base_ + offset, data, len); }

  // Get a buffer to use to write to the file, given the offset into
  // the file and the size.
  unsigned char*
  get_output_view(off_t start, off_t size)
  {
    assert(start >= 0 && size >= 0 && start + size <= this->file_size_);
    return this->base_ + start;
  }

  // VIEW must have been returned by get_output_view.  Write the
  // buffer to the file, passing in the offset and the size.
  void
  write_output_view(off_t, off_t, unsigned char*)
  { }

 private:
  // General options.
  const General_options& options_;
  // File name.
  const char* name_;
  // File descriptor.
  int o_;
  // File size.
  off_t file_size_;
  // Base of file mapped into memory.
  unsigned char* base_;
};

} // End namespace gold.

#endif // !defined(GOLD_OUTPUT_H)
