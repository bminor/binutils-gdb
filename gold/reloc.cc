// reloc.cc -- relocate input files for gold.

#include "gold.h"

#include "workqueue.h"
#include "object.h"
#include "output.h"
#include "reloc.h"

namespace gold
{

// Relocate_task methods.

// These tasks are always runnable.

Task::Is_runnable_type
Relocate_task::is_runnable(Workqueue*)
{
  return IS_RUNNABLE;
}

// We want to lock the file while we run.  We want to unblock
// FINAL_BLOCKER when we are done.

class Relocate_task::Relocate_locker : public Task_locker
{
 public:
  Relocate_locker(Task_token& token, Workqueue* workqueue,
		  Object* object)
    : blocker_(token, workqueue), objlock_(*object)
  { }

 private:
  Task_locker_block blocker_;
  Task_locker_obj<Object> objlock_;
};

Task_locker*
Relocate_task::locks(Workqueue* workqueue)
{
  return new Relocate_locker(*this->final_blocker_, workqueue,
			     this->object_);
}

// Run the task.

void
Relocate_task::run(Workqueue*)
{
  this->object_->relocate(this->options_, this->symtab_, this->sympool_,
			  this->of_);
}

// Relocate the input sections and write out the local symbols.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::do_relocate(const General_options&,
					    const Symbol_table* symtab,
					    const Stringpool* sympool,
					    Output_file* of)
{
  unsigned int shnum = this->shnum();

  // Read the section headers.
  const unsigned char* pshdrs = this->get_view(this->shoff_,
					       shnum * This::shdr_size);

  Views views;
  views.resize(shnum);

  // Make two passes over the sections.  The first one copies the
  // section data to the output file.  The second one applies
  // relocations.

  this->write_sections(pshdrs, of, &views);

  // Apply relocations.

  this->relocate_sections(symtab, pshdrs, &views);

  // Write out the accumulated views.
  for (unsigned int i = 1; i < shnum; ++i)
    {
      if (views[i].view != NULL)
	of->write_output_view(views[i].offset, views[i].view_size,
			      views[i].view);
    }

  // Write out the local symbols.
  this->write_local_symbols(of, sympool);
}

// Write section data to the output file.  PSHDRS points to the
// section headers.  Record the views in *PVIEWS for use when
// relocating.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::write_sections(const unsigned char* pshdrs,
					       Output_file* of,
					       Views* pviews)
{
  unsigned int shnum = this->shnum();
  std::vector<Map_to_output>& map_sections(this->map_to_output());

  const unsigned char* p = pshdrs + This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, p += This::shdr_size)
    {
      View_size* pvs = &(*pviews)[i];

      pvs->view = NULL;

      const Output_section* os = map_sections[i].output_section;
      if (os == NULL)
	continue;

      typename This::Shdr shdr(p);

      if (shdr.get_sh_type() == elfcpp::SHT_NOBITS)
	continue;

      assert(map_sections[i].offset >= 0
	     && map_sections[i].offset < os->data_size());
      off_t start = os->offset() + map_sections[i].offset;
      off_t sh_size = shdr.get_sh_size();

      unsigned char* view = of->get_output_view(start, sh_size);
      this->input_file()->file().read(shdr.get_sh_offset(),
				      sh_size,
				      view);
      pvs->view = view;
      pvs->address = os->address() + map_sections[i].offset;
      pvs->offset = start;
      pvs->view_size = sh_size;
    }
}

// Relocate section data.  VIEWS points to the section data as views
// in the output file.

template<int size, bool big_endian>
void
Sized_object<size, big_endian>::relocate_sections(const Symbol_table* symtab,
						  const unsigned char* pshdrs,
						  Views* pviews)
{
  unsigned int shnum = this->shnum();
  std::vector<Map_to_output>& map_sections(this->map_to_output());
  Sized_target<size, big_endian>* target = this->sized_target();

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
	  fprintf(stderr, _("%s: %s: relocation section %u has bad info %u\n"),
		  program_name, this->name().c_str(), i, index);
	  gold_exit(false);
	}

      if (map_sections[index].output_section == NULL)
	{
	  // This relocation section is against a section which we
	  // discarded.
	  continue;
	}

      assert((*pviews)[index].view != NULL);

      if (shdr.get_sh_link() != this->symtab_shnum_)
	{
	  fprintf(stderr,
		  _("%s: %s: relocation section %u uses unexpected "
		    "symbol table %u\n"),
		  program_name, this->name().c_str(), i, shdr.get_sh_link());
	  gold_exit(false);
	}

      off_t sh_size = shdr.get_sh_size();
      const unsigned char* prelocs = this->get_view(shdr.get_sh_offset(),
						    sh_size);

      unsigned int reloc_size;
      if (sh_type == elfcpp::SHT_REL)
	reloc_size = elfcpp::Elf_sizes<size>::rel_size;
      else
	reloc_size = elfcpp::Elf_sizes<size>::rela_size;

      if (reloc_size != shdr.get_sh_entsize())
	{
	  fprintf(stderr,
		  _("%s: %s: unexpected entsize for reloc section %u: "
		    "%lu != %u"),
		  program_name, this->name().c_str(), i,
		  static_cast<unsigned long>(shdr.get_sh_entsize()),
		  reloc_size);
	  gold_exit(false);
	}

      size_t reloc_count = sh_size / reloc_size;
      if (reloc_count * reloc_size != sh_size)
	{
	  fprintf(stderr, _("%s: %s: reloc section %u size %lu uneven"),
		  program_name, this->name().c_str(), i,
		  static_cast<unsigned long>(sh_size));
	  gold_exit(false);
	}

      target->relocate_section(symtab, this, sh_type, prelocs, reloc_count,
			       this->local_symbol_count_,
			       this->values_,
			       this->symbols_,
			       (*pviews)[index].view,
			       (*pviews)[index].address,
			       (*pviews)[index].view_size);
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

template
void
Sized_object<32, false>::do_relocate(const General_options& options,
				     const Symbol_table* symtab,
				     const Stringpool* sympool,
				     Output_file* of);

template
void
Sized_object<32, true>::do_relocate(const General_options& options,
				    const Symbol_table* symtab,
				    const Stringpool* sympool,
				    Output_file* of);

template
void
Sized_object<64, false>::do_relocate(const General_options& options,
				     const Symbol_table* symtab,
				     const Stringpool* sympool,
				     Output_file* of);

template
void
Sized_object<64, true>::do_relocate(const General_options& options,
				    const Symbol_table* symtab,
				    const Stringpool* sympool,
				    Output_file* of);


} // End namespace gold.
