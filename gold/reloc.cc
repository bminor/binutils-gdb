// reloc.cc -- relocate input files for gold.

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

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

#include "gold.h"

#include "workqueue.h"
#include "object.h"
#include "symtab.h"
#include "output.h"
#include "reloc.h"

namespace gold
{

// Read_relocs methods.

// These tasks just read the relocation information from the file.
// After reading it, the start another task to process the
// information.  These tasks requires access to the file.

Task_token*
Read_relocs::is_runnable()
{
  return this->object_->is_locked() ? this->object_->token() : NULL;
}

// Lock the file.

void
Read_relocs::locks(Task_locker* tl)
{
  tl->add(this, this->object_->token());
}

// Read the relocations and then start a Scan_relocs_task.

void
Read_relocs::run(Workqueue* workqueue)
{
  Read_relocs_data *rd = new Read_relocs_data;
  this->object_->read_relocs(rd);
  this->object_->release();

  workqueue->queue_front(new Scan_relocs(this->options_, this->symtab_,
					 this->layout_, this->object_, rd,
					 this->symtab_lock_, this->blocker_));
}

// Return a debugging name for the task.

std::string
Read_relocs::get_name() const
{
  return "Read_relocs " + this->object_->name();
}

// Scan_relocs methods.

// These tasks scan the relocations read by Read_relocs and mark up
// the symbol table to indicate which relocations are required.  We
// use a lock on the symbol table to keep them from interfering with
// each other.

Task_token*
Scan_relocs::is_runnable()
{
  if (!this->symtab_lock_->is_writable())
    return this->symtab_lock_;
  if (this->object_->is_locked())
    return this->object_->token();
  return NULL;
}

// Return the locks we hold: one on the file, one on the symbol table
// and one blocker.

void
Scan_relocs::locks(Task_locker* tl)
{
  tl->add(this, this->object_->token());
  tl->add(this, this->symtab_lock_);
  tl->add(this, this->blocker_);
}

// Scan the relocs.

void
Scan_relocs::run(Workqueue*)
{
  this->object_->scan_relocs(this->options_, this->symtab_, this->layout_,
			     this->rd_);
  this->object_->release();
  delete this->rd_;
  this->rd_ = NULL;
}

// Return a debugging name for the task.

std::string
Scan_relocs::get_name() const
{
  return "Scan_relocs " + this->object_->name();
}

// Relocate_task methods.

// We may have to wait for the output sections to be written.

Task_token*
Relocate_task::is_runnable()
{
  if (this->object_->relocs_must_follow_section_writes()
      && this->output_sections_blocker_->is_blocked())
    return this->output_sections_blocker_;

  if (this->object_->is_locked())
    return this->object_->token();

  return NULL;
}

// We want to lock the file while we run.  We want to unblock
// INPUT_SECTIONS_BLOCKER and FINAL_BLOCKER when we are done.
// INPUT_SECTIONS_BLOCKER may be NULL.

void
Relocate_task::locks(Task_locker* tl)
{
  if (this->input_sections_blocker_ != NULL)
    tl->add(this, this->input_sections_blocker_);
  tl->add(this, this->final_blocker_);
  tl->add(this, this->object_->token());
}

// Run the task.

void
Relocate_task::run(Workqueue*)
{
  this->object_->relocate(this->options_, this->symtab_, this->layout_,
			  this->of_);
  this->object_->release();
}

// Return a debugging name for the task.

std::string
Relocate_task::get_name() const
{
  return "Relocate_task " + this->object_->name();
}

// Read the relocs and local symbols from the object file and store
// the information in RD.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::do_read_relocs(Read_relocs_data* rd)
{
  rd->relocs.clear();

  unsigned int shnum = this->shnum();
  if (shnum == 0)
    return;

  rd->relocs.reserve(shnum / 2);

  std::vector<Map_to_output>& map_sections(this->map_to_output());

  const unsigned char *pshdrs = this->get_view(this->elf_file_.shoff(),
					       shnum * This::shdr_size,
					       true);
  // Skip the first, dummy, section.
  const unsigned char *ps = pshdrs + This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, ps += This::shdr_size)
    {
      typename This::Shdr shdr(ps);

      unsigned int sh_type = shdr.get_sh_type();
      if (sh_type != elfcpp::SHT_REL && sh_type != elfcpp::SHT_RELA)
	continue;

      unsigned int shndx = shdr.get_sh_info();
      if (shndx >= shnum)
	{
	  this->error(_("relocation section %u has bad info %u"),
		      i, shndx);
	  continue;
	}

      Output_section* os = map_sections[shndx].output_section;
      if (os == NULL)
	continue;

      // We are scanning relocations in order to fill out the GOT and
      // PLT sections.  Relocations for sections which are not
      // allocated (typically debugging sections) should not add new
      // GOT and PLT entries.  So we skip them.
      typename This::Shdr secshdr(pshdrs + shndx * This::shdr_size);
      if ((secshdr.get_sh_flags() & elfcpp::SHF_ALLOC) == 0)
	continue;

      if (shdr.get_sh_link() != this->symtab_shndx_)
	{
	  this->error(_("relocation section %u uses unexpected "
			"symbol table %u"),
		      i, shdr.get_sh_link());
	  continue;
	}

      off_t sh_size = shdr.get_sh_size();

      unsigned int reloc_size;
      if (sh_type == elfcpp::SHT_REL)
	reloc_size = elfcpp::Elf_sizes<size>::rel_size;
      else
	reloc_size = elfcpp::Elf_sizes<size>::rela_size;
      if (reloc_size != shdr.get_sh_entsize())
	{
	  this->error(_("unexpected entsize for reloc section %u: %lu != %u"),
		      i, static_cast<unsigned long>(shdr.get_sh_entsize()),
		      reloc_size);
	  continue;
	}

      size_t reloc_count = sh_size / reloc_size;
      if (static_cast<off_t>(reloc_count * reloc_size) != sh_size)
	{
	  this->error(_("reloc section %u size %lu uneven"),
		      i, static_cast<unsigned long>(sh_size));
	  continue;
	}

      rd->relocs.push_back(Section_relocs());
      Section_relocs& sr(rd->relocs.back());
      sr.reloc_shndx = i;
      sr.data_shndx = shndx;
      sr.contents = this->get_lasting_view(shdr.get_sh_offset(), sh_size,
					   true);
      sr.sh_type = sh_type;
      sr.reloc_count = reloc_count;
      sr.output_section = os;
      sr.needs_special_offset_handling = map_sections[shndx].offset == -1;
    }

  // Read the local symbols.
  gold_assert(this->symtab_shndx_ != -1U);
  if (this->symtab_shndx_ == 0 || this->local_symbol_count_ == 0)
    rd->local_symbols = NULL;
  else
    {
      typename This::Shdr symtabshdr(pshdrs
				     + this->symtab_shndx_ * This::shdr_size);
      gold_assert(symtabshdr.get_sh_type() == elfcpp::SHT_SYMTAB);
      const int sym_size = This::sym_size;
      const unsigned int loccount = this->local_symbol_count_;
      gold_assert(loccount == symtabshdr.get_sh_info());
      off_t locsize = loccount * sym_size;
      rd->local_symbols = this->get_lasting_view(symtabshdr.get_sh_offset(),
						 locsize, true);
    }
}

// Scan the relocs and adjust the symbol table.  This looks for
// relocations which require GOT/PLT/COPY relocations.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::do_scan_relocs(const General_options& options,
					       Symbol_table* symtab,
					       Layout* layout,
					       Read_relocs_data* rd)
{
  Sized_target<size, big_endian>* target = this->sized_target();

  const unsigned char* local_symbols;
  if (rd->local_symbols == NULL)
    local_symbols = NULL;
  else
    local_symbols = rd->local_symbols->data();

  for (Read_relocs_data::Relocs_list::iterator p = rd->relocs.begin();
       p != rd->relocs.end();
       ++p)
    {
      target->scan_relocs(options, symtab, layout, this, p->data_shndx,
			  p->sh_type, p->contents->data(), p->reloc_count,
			  p->output_section, p->needs_special_offset_handling,
			  this->local_symbol_count_,
			  local_symbols);
      delete p->contents;
      p->contents = NULL;
    }

  if (rd->local_symbols != NULL)
    {
      delete rd->local_symbols;
      rd->local_symbols = NULL;
    }
}

// Relocate the input sections and write out the local symbols.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::do_relocate(const General_options& options,
					    const Symbol_table* symtab,
					    const Layout* layout,
					    Output_file* of)
{
  unsigned int shnum = this->shnum();

  // Read the section headers.
  const unsigned char* pshdrs = this->get_view(this->elf_file_.shoff(),
					       shnum * This::shdr_size,
					       true);

  Views views;
  views.resize(shnum);

  // Make two passes over the sections.  The first one copies the
  // section data to the output file.  The second one applies
  // relocations.

  this->write_sections(pshdrs, of, &views);

  // Apply relocations.

  this->relocate_sections(options, symtab, layout, pshdrs, &views);

  // Write out the accumulated views.
  for (unsigned int i = 1; i < shnum; ++i)
    {
      if (views[i].view != NULL)
	{
	  if (!views[i].is_postprocessing_view)
	    {
	      if (views[i].is_input_output_view)
		of->write_input_output_view(views[i].offset,
					    views[i].view_size,
					    views[i].view);
	      else
		of->write_output_view(views[i].offset, views[i].view_size,
				      views[i].view);
	    }
	}
    }

  // Write out the local symbols.
  this->write_local_symbols(of, layout->sympool(), layout->dynpool());
}

// Write section data to the output file.  PSHDRS points to the
// section headers.  Record the views in *PVIEWS for use when
// relocating.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::write_sections(const unsigned char* pshdrs,
					       Output_file* of,
					       Views* pviews) const
{
  unsigned int shnum = this->shnum();
  const std::vector<Map_to_output>& map_sections(this->map_to_output());

  const unsigned char* p = pshdrs + This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, p += This::shdr_size)
    {
      View_size* pvs = &(*pviews)[i];

      pvs->view = NULL;

      const Output_section* os = map_sections[i].output_section;
      if (os == NULL)
	continue;
      off_t output_offset = map_sections[i].offset;

      typename This::Shdr shdr(p);

      if (shdr.get_sh_type() == elfcpp::SHT_NOBITS)
	continue;

      // In the normal case, this input section is simply mapped to
      // the output section at offset OUTPUT_OFFSET.

      // However, if OUTPUT_OFFSET == -1, then input data is handled
      // specially--e.g., a .eh_frame section.  The relocation
      // routines need to check for each reloc where it should be
      // applied.  For this case, we need an input/output view for the
      // entire contents of the section in the output file.  We don't
      // want to copy the contents of the input section to the output
      // section; the output section contents were already written,
      // and we waited for them in Relocate_task::is_runnable because
      // relocs_must_follow_section_writes is set for the object.

      // Regardless of which of the above cases is true, we have to
      // check requires_postprocessing of the output section.  If that
      // is false, then we work with views of the output file
      // directly.  If it is true, then we work with a separate
      // buffer, and the output section is responsible for writing the
      // final data to the output file.

      off_t output_section_offset;
      off_t output_section_size;
      if (!os->requires_postprocessing())
	{
	  output_section_offset = os->offset();
	  output_section_size = os->data_size();
	}
      else
	{
	  output_section_offset = 0;
	  output_section_size = os->postprocessing_buffer_size();
	}

      off_t view_start;
      off_t view_size;
      if (output_offset != -1)
	{
	  view_start = output_section_offset + output_offset;
	  view_size = shdr.get_sh_size();
	}
      else
	{
	  view_start = output_section_offset;
	  view_size = output_section_size;
	}

      if (view_size == 0)
	continue;

      gold_assert(output_offset == -1
		  || (output_offset >= 0
		      && output_offset + view_size <= output_section_size));

      unsigned char* view;
      if (os->requires_postprocessing())
	{
	  unsigned char* buffer = os->postprocessing_buffer();
	  view = buffer + view_start;
	  if (output_offset != -1)
	    this->read(shdr.get_sh_offset(), view_size, view);
	}
      else
	{
	  if (output_offset == -1)
	    view = of->get_input_output_view(view_start, view_size);
	  else
	    {
	      view = of->get_output_view(view_start, view_size);
	      this->read(shdr.get_sh_offset(), view_size, view);
	    }
	}

      pvs->view = view;
      pvs->address = os->address();
      if (output_offset != -1)
	pvs->address += output_offset;
      pvs->offset = view_start;
      pvs->view_size = view_size;
      pvs->is_input_output_view = output_offset == -1;
      pvs->is_postprocessing_view = os->requires_postprocessing();
    }
}

// Relocate section data.  VIEWS points to the section data as views
// in the output file.

template<int size, bool big_endian>
void
Sized_relobj<size, big_endian>::relocate_sections(
    const General_options& options,
    const Symbol_table* symtab,
    const Layout* layout,
    const unsigned char* pshdrs,
    Views* pviews)
{
  unsigned int shnum = this->shnum();
  Sized_target<size, big_endian>* target = this->sized_target();

  const std::vector<Map_to_output>& map_sections(this->map_to_output());

  Relocate_info<size, big_endian> relinfo;
  relinfo.options = &options;
  relinfo.symtab = symtab;
  relinfo.layout = layout;
  relinfo.object = this;

  const unsigned char* p = pshdrs + This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, p += This::shdr_size)
    {
      typename This::Shdr shdr(p);

      unsigned int sh_type = shdr.get_sh_type();
      if (sh_type != elfcpp::SHT_REL && sh_type != elfcpp::SHT_RELA)
	continue;

      unsigned int index = shdr.get_sh_info();
      if (index >= this->shnum())
	{
	  this->error(_("relocation section %u has bad info %u"),
		      i, index);
	  continue;
	}

      Output_section* os = map_sections[index].output_section;
      if (os == NULL)
	{
	  // This relocation section is against a section which we
	  // discarded.
	  continue;
	}
      off_t output_offset = map_sections[index].offset;

      gold_assert((*pviews)[index].view != NULL);

      if (shdr.get_sh_link() != this->symtab_shndx_)
	{
	  gold_error(_("relocation section %u uses unexpected "
		       "symbol table %u"),
		     i, shdr.get_sh_link());
	  continue;
	}

      off_t sh_size = shdr.get_sh_size();
      const unsigned char* prelocs = this->get_view(shdr.get_sh_offset(),
						    sh_size, false);

      unsigned int reloc_size;
      if (sh_type == elfcpp::SHT_REL)
	reloc_size = elfcpp::Elf_sizes<size>::rel_size;
      else
	reloc_size = elfcpp::Elf_sizes<size>::rela_size;

      if (reloc_size != shdr.get_sh_entsize())
	{
	  gold_error(_("unexpected entsize for reloc section %u: %lu != %u"),
		     i, static_cast<unsigned long>(shdr.get_sh_entsize()),
		     reloc_size);
	  continue;
	}

      size_t reloc_count = sh_size / reloc_size;
      if (static_cast<off_t>(reloc_count * reloc_size) != sh_size)
	{
	  gold_error(_("reloc section %u size %lu uneven"),
		     i, static_cast<unsigned long>(sh_size));
	  continue;
	}

      gold_assert(output_offset != -1
		  || this->relocs_must_follow_section_writes());

      relinfo.reloc_shndx = i;
      relinfo.data_shndx = index;
      target->relocate_section(&relinfo,
			       sh_type,
			       prelocs,
			       reloc_count,
			       os,
			       output_offset == -1,
			       (*pviews)[index].view,
			       (*pviews)[index].address,
			       (*pviews)[index].view_size);
    }
}

// Copy_relocs::Copy_reloc_entry methods.

// Return whether we should emit this reloc.  We should emit it if the
// symbol is still defined in a dynamic object.  If we should not emit
// it, we clear it, to save ourselves the test next time.

template<int size, bool big_endian>
bool
Copy_relocs<size, big_endian>::Copy_reloc_entry::should_emit()
{
  if (this->sym_ == NULL)
    return false;
  if (this->sym_->is_from_dynobj())
    return true;
  this->sym_ = NULL;
  return false;
}

// Emit a reloc into a SHT_REL section.

template<int size, bool big_endian>
void
Copy_relocs<size, big_endian>::Copy_reloc_entry::emit(
    Output_data_reloc<elfcpp::SHT_REL, true, size, big_endian>* reloc_data)
{
  this->sym_->set_needs_dynsym_entry();
  reloc_data->add_global(this->sym_, this->reloc_type_, this->output_section_,
                         this->relobj_, this->shndx_, this->address_);
}

// Emit a reloc into a SHT_RELA section.

template<int size, bool big_endian>
void
Copy_relocs<size, big_endian>::Copy_reloc_entry::emit(
    Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian>* reloc_data)
{
  this->sym_->set_needs_dynsym_entry();
  reloc_data->add_global(this->sym_, this->reloc_type_, this->output_section_,
                         this->relobj_, this->shndx_, this->address_,
			 this->addend_);
}

// Copy_relocs methods.

// Return whether we need a COPY reloc for a relocation against GSYM.
// The relocation is being applied to section SHNDX in OBJECT.

template<int size, bool big_endian>
bool
Copy_relocs<size, big_endian>::need_copy_reloc(
    const General_options*,
    Relobj* object,
    unsigned int shndx,
    Sized_symbol<size>* sym)
{
  // FIXME: Handle -z nocopyrelocs.

  if (sym->symsize() == 0)
    return false;

  // If this is a readonly section, then we need a COPY reloc.
  // Otherwise we can use a dynamic reloc.
  if ((object->section_flags(shndx) & elfcpp::SHF_WRITE) == 0)
    return true;

  return false;
}

// Save a Rel reloc.

template<int size, bool big_endian>
void
Copy_relocs<size, big_endian>::save(
    Symbol* sym,
    Relobj* relobj,
    unsigned int shndx,
    Output_section* output_section,
    const elfcpp::Rel<size, big_endian>& rel)
{
  unsigned int reloc_type = elfcpp::elf_r_type<size>(rel.get_r_info());
  this->entries_.push_back(Copy_reloc_entry(sym, reloc_type, relobj, shndx,
                                            output_section,
                                            rel.get_r_offset(), 0));
}

// Save a Rela reloc.

template<int size, bool big_endian>
void
Copy_relocs<size, big_endian>::save(
    Symbol* sym,
    Relobj* relobj,
    unsigned int shndx,
    Output_section* output_section,
    const elfcpp::Rela<size, big_endian>& rela)
{
  unsigned int reloc_type = elfcpp::elf_r_type<size>(rela.get_r_info());
  this->entries_.push_back(Copy_reloc_entry(sym, reloc_type, relobj, shndx,
                                            output_section,
					    rela.get_r_offset(),
					    rela.get_r_addend()));
}

// Return whether there are any relocs to emit.  We don't want to emit
// a reloc if the symbol is no longer defined in a dynamic object.

template<int size, bool big_endian>
bool
Copy_relocs<size, big_endian>::any_to_emit()
{
  for (typename Copy_reloc_entries::iterator p = this->entries_.begin();
       p != this->entries_.end();
       ++p)
    {
      if (p->should_emit())
	return true;
    }
  return false;
}

// Emit relocs.

template<int size, bool big_endian>
template<int sh_type>
void
Copy_relocs<size, big_endian>::emit(
    Output_data_reloc<sh_type, true, size, big_endian>* reloc_data)
{
  for (typename Copy_reloc_entries::iterator p = this->entries_.begin();
       p != this->entries_.end();
       ++p)
    {
      if (p->should_emit())
	p->emit(reloc_data);
    }
}

// Track_relocs methods.

// Initialize the class to track the relocs.  This gets the object,
// the reloc section index, and the type of the relocs.  This returns
// false if something goes wrong.

template<int size, bool big_endian>
bool
Track_relocs<size, big_endian>::initialize(
    Object* object,
    unsigned int reloc_shndx,
    unsigned int reloc_type)
{
  // If RELOC_SHNDX is -1U, it means there is more than one reloc
  // section for the .eh_frame section.  We can't handle that case.
  if (reloc_shndx == -1U)
    return false;

  // If RELOC_SHNDX is 0, there is no reloc section.
  if (reloc_shndx == 0)
    return true;

  // Get the contents of the reloc section.
  this->prelocs_ = object->section_contents(reloc_shndx, &this->len_, false);

  if (reloc_type == elfcpp::SHT_REL)
    this->reloc_size_ = elfcpp::Elf_sizes<size>::rel_size;
  else if (reloc_type == elfcpp::SHT_RELA)
    this->reloc_size_ = elfcpp::Elf_sizes<size>::rela_size;
  else
    gold_unreachable();

  if (this->len_ % this->reloc_size_ != 0)
    {
      object->error(_("reloc section size %zu is not a multiple of "
		      "reloc size %d\n"),
		    static_cast<size_t>(this->len_),
		    this->reloc_size_);
      return false;
    }

  return true;
}

// Return the offset of the next reloc, or -1 if there isn't one.

template<int size, bool big_endian>
off_t
Track_relocs<size, big_endian>::next_offset() const
{
  if (this->pos_ >= this->len_)
    return -1;

  // Rel and Rela start out the same, so we can always use Rel to find
  // the r_offset value.
  elfcpp::Rel<size, big_endian> rel(this->prelocs_ + this->pos_);
  return rel.get_r_offset();
}

// Return the index of the symbol referenced by the next reloc, or -1U
// if there aren't any more relocs.

template<int size, bool big_endian>
unsigned int
Track_relocs<size, big_endian>::next_symndx() const
{
  if (this->pos_ >= this->len_)
    return -1U;

  // Rel and Rela start out the same, so we can use Rel to find the
  // symbol index.
  elfcpp::Rel<size, big_endian> rel(this->prelocs_ + this->pos_);
  return elfcpp::elf_r_sym<size>(rel.get_r_info());
}

// Advance to the next reloc whose r_offset is greater than or equal
// to OFFSET.  Return the number of relocs we skip.

template<int size, bool big_endian>
int
Track_relocs<size, big_endian>::advance(off_t offset)
{
  int ret = 0;
  while (this->pos_ < this->len_)
    {
      // Rel and Rela start out the same, so we can always use Rel to
      // find the r_offset value.
      elfcpp::Rel<size, big_endian> rel(this->prelocs_ + this->pos_);
      if (static_cast<off_t>(rel.get_r_offset()) >= offset)
	break;
      ++ret;
      this->pos_ += this->reloc_size_;
    }
  return ret;
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

#ifdef HAVE_TARGET_32_LITTLE
template
void
Sized_relobj<32, false>::do_read_relocs(Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_32_BIG
template
void
Sized_relobj<32, true>::do_read_relocs(Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
void
Sized_relobj<64, false>::do_read_relocs(Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_64_BIG
template
void
Sized_relobj<64, true>::do_read_relocs(Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
void
Sized_relobj<32, false>::do_scan_relocs(const General_options& options,
					Symbol_table* symtab,
					Layout* layout,
					Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_32_BIG
template
void
Sized_relobj<32, true>::do_scan_relocs(const General_options& options,
				       Symbol_table* symtab,
				       Layout* layout,
				       Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
void
Sized_relobj<64, false>::do_scan_relocs(const General_options& options,
					Symbol_table* symtab,
					Layout* layout,
					Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_64_BIG
template
void
Sized_relobj<64, true>::do_scan_relocs(const General_options& options,
				       Symbol_table* symtab,
				       Layout* layout,
				       Read_relocs_data* rd);
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
void
Sized_relobj<32, false>::do_relocate(const General_options& options,
				     const Symbol_table* symtab,
				     const Layout* layout,
				     Output_file* of);
#endif

#ifdef HAVE_TARGET_32_BIG
template
void
Sized_relobj<32, true>::do_relocate(const General_options& options,
				    const Symbol_table* symtab,
				    const Layout* layout,
				    Output_file* of);
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
void
Sized_relobj<64, false>::do_relocate(const General_options& options,
				     const Symbol_table* symtab,
				     const Layout* layout,
				     Output_file* of);
#endif

#ifdef HAVE_TARGET_64_BIG
template
void
Sized_relobj<64, true>::do_relocate(const General_options& options,
				    const Symbol_table* symtab,
				    const Layout* layout,
				    Output_file* of);
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
class Copy_relocs<32, false>;
#endif

#ifdef HAVE_TARGET_32_BIG
template
class Copy_relocs<32, true>;
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
class Copy_relocs<64, false>;
#endif

#ifdef HAVE_TARGET_64_BIG
template
class Copy_relocs<64, true>;
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
void
Copy_relocs<32, false>::emit<elfcpp::SHT_REL>(
    Output_data_reloc<elfcpp::SHT_REL, true, 32, false>*);
#endif

#ifdef HAVE_TARGET_32_BIG
template
void
Copy_relocs<32, true>::emit<elfcpp::SHT_REL>(
    Output_data_reloc<elfcpp::SHT_REL, true, 32, true>*);
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
void
Copy_relocs<64, false>::emit<elfcpp::SHT_REL>(
    Output_data_reloc<elfcpp::SHT_REL, true, 64, false>*);
#endif

#ifdef HAVE_TARGET_64_BIG
template
void
Copy_relocs<64, true>::emit<elfcpp::SHT_REL>(
    Output_data_reloc<elfcpp::SHT_REL, true, 64, true>*);
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
void
Copy_relocs<32, false>::emit<elfcpp::SHT_RELA>(
    Output_data_reloc<elfcpp::SHT_RELA , true, 32, false>*);
#endif

#ifdef HAVE_TARGET_32_BIG
template
void
Copy_relocs<32, true>::emit<elfcpp::SHT_RELA>(
    Output_data_reloc<elfcpp::SHT_RELA, true, 32, true>*);
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
void
Copy_relocs<64, false>::emit<elfcpp::SHT_RELA>(
    Output_data_reloc<elfcpp::SHT_RELA, true, 64, false>*);
#endif

#ifdef HAVE_TARGET_64_BIG
template
void
Copy_relocs<64, true>::emit<elfcpp::SHT_RELA>(
    Output_data_reloc<elfcpp::SHT_RELA, true, 64, true>*);
#endif

#ifdef HAVE_TARGET_32_LITTLE
template
class Track_relocs<32, false>;
#endif

#ifdef HAVE_TARGET_32_BIG
template
class Track_relocs<32, true>;
#endif

#ifdef HAVE_TARGET_64_LITTLE
template
class Track_relocs<64, false>;
#endif

#ifdef HAVE_TARGET_64_BIG
template
class Track_relocs<64, true>;
#endif

} // End namespace gold.
