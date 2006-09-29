// readsyms.cc -- read input file symbols for gold

#include "gold.h"

#include <cstring>

#include "elfcpp.h"
#include "options.h"
#include "dirsearch.h"
#include "object.h"
#include "archive.h"
#include "readsyms.h"

namespace gold
{

// Class read_symbols.

Read_symbols::~Read_symbols()
{
  // The this_blocker_ and next_blocker_ pointers are passed on to the
  // Add_symbols task.
}

// Return whether a Read_symbols task is runnable.  We need write
// access to the symbol table.  We can read an ordinary input file
// immediately.  For an archive specified using -l, we have to wait
// until the search path is complete.

Task::Is_runnable_type
Read_symbols::is_runnable(Workqueue*)
{
  if (this->input_.is_lib() && this->dirpath_.token().is_blocked())
    return IS_BLOCKED;

  return IS_RUNNABLE;
}

// Return a Task_locker for a Read_symbols task.  We don't need any
// locks here.

Task_locker*
Read_symbols::locks(Workqueue*)
{
  return NULL;
}

// Run a Read_symbols task.  This is where we actually read the
// symbols and relocations.

void
Read_symbols::run(Workqueue* workqueue)
{
  Input_file* input_file = new Input_file(this->input_);
  input_file->open(this->options_, this->dirpath_);

  // Read enough of the file to pick up the entire ELF header.

  int ehdr_size = elfcpp::Elf_sizes<64>::ehdr_size;
  off_t bytes;
  const unsigned char* p = input_file->file().get_view(0, ehdr_size, &bytes);
  if (bytes >= 4)
    {
      static unsigned char elfmagic[4] =
	{
	  elfcpp::ELFMAG0, elfcpp::ELFMAG1,
	  elfcpp::ELFMAG2, elfcpp::ELFMAG3
	};
      if (memcmp(p, elfmagic, 4) == 0)
	{
	  // This is an ELF object.
	  Object* obj = make_elf_object(this->input_.name(), input_file, 0,
					p, bytes);

	  this->input_objects_->add_object(obj);

	  Read_symbols_data sd = obj->read_symbols();
	  workqueue->queue(new Add_symbols(this->symtab_, obj, sd,
					   this->this_blocker_,
					   this->next_blocker_));

	  // Opening the file locked it, so now we need to unlock it.
	  input_file->file().unlock();

	  return;
	}
    }

  if (bytes >= Archive::sarmag)
    {
      if (memcmp(p, Archive::armag, Archive::sarmag) == 0)
	{
	  // This is an archive.
	  Archive* arch = new Archive(this->input_.name(), input_file);
	  arch->setup();
	  workqueue->queue(new Add_archive_symbols(this->symtab_,
						   this->input_objects_,
						   arch,
						   this->this_blocker_,
						   this->next_blocker_));
	  return;
	}
    }

  // Here we have to handle archives and any other input file
  // types we need.
  fprintf(stderr, _("%s: %s: not an object or archive\n"),
	  program_name, input_file->file().filename().c_str());
  gold_exit(false);
}

// Class Add_symbols.

Add_symbols::~Add_symbols()
{
  if (this->this_blocker_ != NULL)
    delete this->this_blocker_;
  // next_blocker_ is deleted by the task associated with the next
  // input file.
}

// We are blocked by this_blocker_.  We block next_blocker_.  We also
// lock the file.

Task::Is_runnable_type
Add_symbols::is_runnable(Workqueue*)
{
  if (this->this_blocker_ != NULL && this->this_blocker_->is_blocked())
    return IS_BLOCKED;
  if (this->object_->is_locked())
    return IS_LOCKED;
  return IS_RUNNABLE;
}

class Add_symbols::Add_symbols_locker : public Task_locker
{
 public:
  Add_symbols_locker(Task_token& token, Workqueue* workqueue,
		     Object* object)
    : blocker_(token, workqueue), objlock_(*object)
  { }

 private:
  Task_locker_block blocker_;
  Task_locker_obj<Object> objlock_;
};

Task_locker*
Add_symbols::locks(Workqueue* workqueue)
{
  return new Add_symbols_locker(*this->next_blocker_, workqueue,
				this->object_);
}

void
Add_symbols::run(Workqueue*)
{
  this->object_->add_symbols(this->symtab_, this->sd_);
}

} // End namespace gold.
