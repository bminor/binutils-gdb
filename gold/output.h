// output.h -- manage the output file for gold   -*- C++ -*-

#ifndef GOLD_OUTPUT_H
#define GOLD_OUTPUT_H

#include <cassert>
#include <list>
#include <vector>

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

  // Return the address.  This is only valid after Layout::finalize is
  // finished.
  uint64_t
  address() const
  { return this->address_; }

  // Return the size of the data.  This must be valid after
  // Layout::finalize calls set_address, but need not be valid before
  // then.
  off_t
  data_size() const
  { return this->data_size_; }

  // Return the file offset.  This is only valid after
  // Layout::finalize is finished.
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

  // Return the output section index, if there is an output section.
  unsigned int
  out_shndx() const
  { return this->do_out_shndx(); }

  // Set the output section index, if this is an output section.
  void
  set_out_shndx(unsigned int shndx)
  { this->do_set_out_shndx(shndx); }

  // Set the address and file offset of this data.  This is called
  // during Layout::finalize.
  void
  set_address(uint64_t addr, off_t off);

  // Write the data to the output file.  This is called after
  // Layout::finalize is complete.
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

  // Return the output section index, if there is an output section.
  virtual unsigned int
  do_out_shndx() const
  { abort(); }

  // Set the output section index, if this is an output section.
  virtual void
  do_set_out_shndx(unsigned int)
  { abort(); }

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

// Output sections are mainly comprised of input sections.  However,
// there are cases where we have data to write out which is not in an
// input section.  Output_section_data is used in such cases.  This is
// an abstract base class.

class Output_section_data : public Output_data
{
 public:
  Output_section_data(off_t data_size, uint64_t addralign)
    : Output_data(data_size), output_section_(NULL), addralign_(addralign)
  { }

  Output_section_data(uint64_t addralign)
    : Output_data(0), output_section_(NULL), addralign_(addralign)
  { }

  // Record the output section.
  void
  set_output_section(Output_section* os)
  {
    assert(this->output_section_ == NULL);
    this->output_section_ = os;
  }

 protected:
  // The child class must implement do_write.

  // Return the required alignment.
  uint64_t
  do_addralign() const
  { return this->addralign_; }

  // Return the section index of the output section.
  unsigned int
  do_out_shndx() const;

 private:
  // The output section for this section.
  const Output_section* output_section_;
  // The required alignment.
  uint64_t addralign_;
};

// Output_section_common is used to handle the common symbols.  This
// is quite simple.

class Output_section_common : public Output_section_data
{
 public:
  Output_section_common(uint64_t addralign)
    : Output_section_data(addralign)
  { }

  // Set the size.
  void
  set_common_size(off_t common_size)
  { this->set_data_size(common_size); }

  // Write out the data--there is nothing to do, as common symbols are
  // always zero and are stored in the BSS.
  void
  do_write(Output_file*)
  { }
};

// Output_section_got is used to manage a GOT.  Each entry in the GOT
// is for one symbol--either a global symbol or a local symbol in an
// object.  The target specific code adds entries to the GOT as
// needed.  The GOT code is then responsible for writing out the data
// and for generating relocs as required.

template<int size, bool big_endian>
class Output_section_got : public Output_section_data
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Valtype;

  Output_section_got()
    : Output_section_data(Output_data::default_alignment(size)),
      entries_()
  { }

  // Add an entry for a global symbol to the GOT.  This returns the
  // offset of the new entry from the start of the GOT.
  unsigned int
  add_global(Symbol* gsym)
  {
    this->entries_.push_back(Got_entry(gsym));
    this->set_got_size();
    return this->last_got_offset();
  }

  // Add an entry for a local symbol to the GOT.  This returns the
  // offset of the new entry from the start of the GOT.
  unsigned int
  add_local(Object* object, unsigned int sym_index)
  {
    this->entries_.push_back(Got_entry(object, sym_index));
    this->set_got_size();
    return this->last_got_offset();
  }

  // Add a constant to the GOT.  This returns the offset of the new
  // entry from the start of the GOT.
  unsigned int
  add_constant(Valtype constant)
  {
    this->entries_.push_back(Got_entry(constant));
    this->set_got_size();
    return this->last_got_offset();
  }

  // Write out the GOT table.
  void
  do_write(Output_file*);

 private:
  // This POD class holds a single GOT entry.
  class Got_entry
  {
   public:
    // Create a zero entry.
    Got_entry()
      : local_sym_index_(CONSTANT_CODE)
    { this->u_.constant = 0; }

    // Create a global symbol entry.
    Got_entry(Symbol* gsym)
      : local_sym_index_(GSYM_CODE)
    { this->u_.gsym = gsym; }

    // Create a local symbol entry.
    Got_entry(Object* object, unsigned int local_sym_index)
      : local_sym_index_(local_sym_index)
    {
      assert(local_sym_index != GSYM_CODE
	     && local_sym_index != CONSTANT_CODE);
      this->u_.object = object;
    }

    // Create a constant entry.  The constant is a host value--it will
    // be swapped, if necessary, when it is written out.
    Got_entry(Valtype constant)
      : local_sym_index_(CONSTANT_CODE)
    { this->u_.constant = constant; }

    // Write the GOT entry to an output view.
    void
    write(unsigned char* pov) const;

   private:
    enum
    {
      GSYM_CODE = -1U,
      CONSTANT_CODE = -2U
    };

    union
    {
      // For a local symbol, the object.
      Object* object;
      // For a global symbol, the symbol.
      Symbol* gsym;
      // For a constant, the constant.
      Valtype constant;
    } u_;
    // For a local symbol, the local symbol index.  This is -1U for a
    // global symbol, or -2U for a constant.
    unsigned int local_sym_index_;
  };

  typedef std::vector<Got_entry> Got_entries;

  // Return the offset into the GOT of GOT entry I.
  unsigned int
  got_offset(unsigned int i) const
  { return i * (size / 8); }

  // Return the offset into the GOT of the last entry added.
  unsigned int
  last_got_offset() const
  { return this->got_offset(this->entries_.size() - 1); }

  // Set the size of the section.
  void
  set_got_size()
  { this->set_data_size(this->got_offset(this->entries_.size())); }

  // The list of GOT entries.
  Got_entries entries_;
};

// An output section.  We don't expect to have too many output
// sections, so we don't bother to do a template on the size.

class Output_section : public Output_data
{
 public:
  // Create an output section, giving the name, type, and flags.
  Output_section(const char* name, elfcpp::Elf_Word, elfcpp::Elf_Xword,
		 bool may_add_data);
  virtual ~Output_section();

  // Add a new input section SHNDX, named NAME, with header SHDR, from
  // object OBJECT.  Return the offset within the output section.
  template<int size, bool big_endian>
  off_t
  add_input_section(Relobj* object, unsigned int shndx, const char *name,
		    const elfcpp::Shdr<size, big_endian>& shdr);

  // Add generated data ODATA to this output section.
  virtual void
  add_output_section_data(Output_section_data* posd);

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

  // Return the section index in the output file.
  unsigned int
  do_out_shndx() const
  { return this->out_shndx_; }

  // Set the output section index.
  void
  do_set_out_shndx(unsigned int shndx)
  { this->out_shndx_ = shndx; }

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

  // Set the address of the Output_section.  For a typical
  // Output_section, there is nothing to do, but if there are any
  // Output_section_data objects we need to set the final addresses
  // here.
  void
  do_set_address(uint64_t, off_t);

  // Write the data to the file.  For a typical Output_section, this
  // does nothing: the data is written out by calling Object::Relocate
  // on each input object.  But if there are any Output_section_data
  // objects we do need to write them out here.
  virtual void
  do_write(Output_file*);

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
  // In some cases we need to keep a list of the input sections
  // associated with this output section.  We only need the list if we
  // might have to change the offsets of the input section within the
  // output section after we add the input section.  The ordinary
  // input sections will be written out when we process the object
  // file, and as such we don't need to track them here.  We do need
  // to track Output_section_data objects here.  We store instances of
  // this structure in a std::vector, so it must be a POD.  There can
  // be many instances of this structure, so we use a union to save
  // some space.
  class Input_section
  {
   public:
    Input_section()
      : shndx_(0), p2align_(0), data_size_(0)
    { this->u_.object = NULL; }

    Input_section(Relobj* object, unsigned int shndx, off_t data_size,
		  uint64_t addralign)
      : shndx_(shndx),
	p2align_(ffsll(static_cast<long long>(addralign))),
	data_size_(data_size)
    {
      assert(shndx != -1U);
      this->u_.object = object;
    }

    Input_section(Output_section_data* posd)
      : shndx_(-1U),
	p2align_(ffsll(static_cast<long long>(posd->addralign()))),
	data_size_(0)
    { this->u_.posd = posd; }

    // The required alignment.
    uint64_t
    addralign() const
    { return static_cast<uint64_t>(1) << this->p2align_; }

    // Return the required size.
    off_t
    data_size() const;

    // Set the address and file offset.  This is called during
    // Layout::finalize.  SECOFF is the file offset of the enclosing
    // section.
    void
    set_address(uint64_t addr, off_t off, off_t secoff);

    // Write out the data.  This does nothing for an input section.
    void
    write(Output_file*);

   private:
    // Whether this is an input section.
    bool
    is_input_section() const
    { return this->shndx_ != -1U; }

    // For an ordinary input section, this is the section index in
    // the input file.  For an Output_section_data, this is -1U.
    unsigned int shndx_;
    // The required alignment, stored as a power of 2.
    unsigned int p2align_;
    // For an ordinary input section, the section size.
    off_t data_size_;
    union
    {
      // If shndx_ != -1U, this points to the object which holds the
      // input section.
      Relobj* object;
      // If shndx_ == -1U, this is the data to write out.
      Output_section_data* posd;
    } u_;
  };

  typedef std::vector<Input_section> Input_section_list;

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
  unsigned int out_shndx_;
  // The input sections.  This will be empty in cases where we don't
  // need to keep track of them.
  Input_section_list input_sections_;
  // The offset of the first entry in input_sections_.
  off_t first_input_offset_;
  // Whether we permit adding data.
  bool may_add_data_;
};

// A special Output_section which represents the symbol table
// (SHT_SYMTAB).  The actual data is written out by
// Symbol_table::write_globals.

class Output_section_symtab : public Output_section
{
 public:
  Output_section_symtab(const char* name, off_t size);

  // The data is written out by Symbol_table::write_globals.  We don't
  // do anything here.
  void
  do_write(Output_file*)
  { }

  // We don't expect to see any input sections or data here.
  void
  add_output_section_data(Output_section_data*)
  { abort(); }
};

// A special Output_section which holds a string table.

class Output_section_strtab : public Output_section
{
 public:
  Output_section_strtab(const char* name, Stringpool* contents);

  // Write out the data.
  void
  do_write(Output_file*);

  // We don't expect to see any input sections or data here.
  void
  add_output_section_data(Output_section_data*)
  { abort(); }

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

  // Return the memory size.
  uint64_t
  memsz() const
  { return this->memsz_; }

  // Return the file size.
  off_t
  filesz() const
  { return this->filesz_; }

  // Return the maximum alignment of the Output_data.
  uint64_t
  addralign();

  // Add an Output_section to this segment.
  void
  add_output_section(Output_section*, elfcpp::Elf_Word seg_flags);

  // Add an Output_data (which is not an Output_section) to the start
  // of this segment.
  void
  add_initial_output_data(Output_data*);

  // Set the address of the segment to ADDR and the offset to *POFF
  // (aligned if necessary), and set the addresses and offsets of all
  // contained output sections accordingly.  Set the section indexes
  // of all contained output sections starting with *PSHNDX.  Return
  // the address of the immediately following segment.  Update *POFF
  // and *PSHNDX.  This should only be called for a PT_LOAD segment.
  uint64_t
  set_section_addresses(uint64_t addr, off_t* poff, unsigned int* pshndx);

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
  write_header(elfcpp::Phdr_write<size, big_endian>*);

  // Write the section headers of associated sections into V.
  template<int size, bool big_endian>
  unsigned char*
  write_section_headers(const Stringpool*,
                        unsigned char* v,
			unsigned int* pshndx ACCEPT_SIZE_ENDIAN) const;

 private:
  Output_segment(const Output_segment&);
  Output_segment& operator=(const Output_segment&);

  typedef std::list<Output_data*> Output_data_list;

  // Find the maximum alignment in an Output_data_list.
  static uint64_t
  maximum_alignment(const Output_data_list*);

  // Set the section addresses in an Output_data_list.
  uint64_t
  set_section_list_addresses(Output_data_list*, uint64_t addr, off_t* poff,
			     unsigned int* pshndx);

  // Return the number of Output_sections in an Output_data_list.
  unsigned int
  output_section_count_list(const Output_data_list*) const;

  // Write the section headers in the list into V.
  template<int size, bool big_endian>
  unsigned char*
  write_section_headers_list(const Stringpool*, const Output_data_list*,
			     unsigned char* v,
			     unsigned int* pshdx ACCEPT_SIZE_ENDIAN) const;

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
  // Whether we have set align_.
  bool is_align_known_;
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
