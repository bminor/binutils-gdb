// reloc.h -- relocate input files for gold   -*- C++ -*-

#ifndef GOLD_RELOC_H
#define GOLD_RELOC_H

#include "workqueue.h"

namespace gold
{

class Relocate_task : public Task
{
 public:
  Relocate_task(const General_options& options, const Symbol_table* symtab,
		const Stringpool* sympool, Object* object, Output_file* of,
		Task_token* final_blocker)
    : options_(options), symtab_(symtab), sympool_(sympool), object_(object),
      of_(of), final_blocker_(final_blocker)
  { }

  // The standard Task methods.

  Is_runnable_type
  is_runnable(Workqueue*);

  Task_locker*
  locks(Workqueue*);

  void
  run(Workqueue*);

 private:
  class Relocate_locker;

  const General_options& options_;
  const Symbol_table* symtab_;
  const Stringpool* sympool_;
  Object* object_;
  Output_file* of_;
  Task_token* final_blocker_;
};

} // End namespace gold.

#endif // !defined(GOLD_RELOC_H)
