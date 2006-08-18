// readsyms.h -- read input file symbols for gold   -*- C++ -*-

#ifndef GOLD_READSYMS_H
#define GOLD_READSYMS_H

#include "targetsize.h"
#include "workqueue.h"
#include "object.h"

namespace gold
{

// This Task is responsible for reading the symbols from an input
// file.  This also includes reading the relocations so that we can
// check for any that require a PLT and/or a GOT.  After the data has
// been read, this queues up another task to actually add the symbols
// to the symbol table.  The tasks are separated because the file
// reading can occur in parallel but adding the symbols must be done
// in the order of the input files.

class Read_symbols : public Task
{
 public:
  // DIRPATH is the list of directories to search for libraries.
  // INPUT is the file to read.  THIS_BLOCKER is used to prevent the
  // associated Add_symbols task from running before the previous one
  // has completed; it will be NULL for the first task.  NEXT_BLOCKER
  // is used to block the next input file from adding symbols.
  Read_symbols(const General_options& options, Symbol_table* symtab,
	       const Dirsearch& dirpath, const Input_argument& input,
	       Task_token* this_blocker, Task_token* next_blocker)
    : options_(options), symtab_(symtab), dirpath_(dirpath), input_(input),
      this_blocker_(this_blocker), next_blocker_(next_blocker)
  { }

  ~Read_symbols();

  // The standard Task methods.

  Is_runnable_type
  is_runnable(Workqueue*);

  Task_locker*
  locks(Workqueue*);

  void
  run(Workqueue*);

 private:
  const General_options& options_;
  Symbol_table* symtab_;
  const Dirsearch& dirpath_;
  const Input_argument& input_;
  Task_token* this_blocker_;
  Task_token* next_blocker_;
};

// This Task handles adding the symbols to the symbol table.  These
// tasks must be run in the same order as the arguments appear on the
// command line.

class Add_symbols : public Task
{
 public:
  // THIS_BLOCKER is used to prevent this task from running before the
  // one for the previous input file.  NEXT_BLOCKER is used to prevent
  // the next task from running.
  Add_symbols(Symbol_table* symtab, Object* object, Read_symbols_data sd,
	      Task_token* this_blocker, Task_token* next_blocker)
    : symtab_(symtab), object_(object), sd_(sd), this_blocker_(this_blocker),
      next_blocker_(next_blocker)
  { }

  ~Add_symbols();

  // The standard Task methods.

  Is_runnable_type
  is_runnable(Workqueue*);

  Task_locker*
  locks(Workqueue*);

  void
  run(Workqueue*);

private:
  Symbol_table* symtab_;
  Object* object_;
  Read_symbols_data sd_;
  Task_token* this_blocker_;
  Task_token* next_blocker_;
};

} // end namespace gold

#endif // !defined(GOLD_READSYMS_H)
