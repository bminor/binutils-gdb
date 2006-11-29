// reloc.h -- relocate input files for gold   -*- C++ -*-

#ifndef GOLD_RELOC_H
#define GOLD_RELOC_H

#include <byteswap.h>

#include "workqueue.h"

namespace gold
{

class General_options;
class Relobj;
class Read_relocs_data;
class Stringpool;
class Symbol;
class Layout;

// A class to read the relocations for an object file, and then queue
// up a task to see if they require any GOT/PLT/COPY relocations in
// the symbol table.

class Read_relocs : public Task
{
 public:
  // SYMTAB_LOCK is used to lock the symbol table.  BLOCKER should be
  // unblocked when the Scan_relocs task completes.
  Read_relocs(const General_options& options, Symbol_table* symtab,
	      Layout* layout, Relobj* object, Task_token* symtab_lock,
	      Task_token* blocker)
    : options_(options), symtab_(symtab), layout_(layout), object_(object),
      symtab_lock_(symtab_lock), blocker_(blocker)
  { }

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
  Layout* layout_;
  Relobj* object_;
  Task_token* symtab_lock_;
  Task_token* blocker_;
};

// Scan the relocations for an object to see if they require any
// GOT/PLT/COPY relocations.

class Scan_relocs : public Task
{
 public:
  // SYMTAB_LOCK is used to lock the symbol table.  BLOCKER should be
  // unblocked when the task completes.
  Scan_relocs(const General_options& options, Symbol_table* symtab,
	      Layout* layout, Relobj* object, Read_relocs_data* rd,
	      Task_token* symtab_lock, Task_token* blocker)
    : options_(options), symtab_(symtab), layout_(layout), object_(object),
      rd_(rd), symtab_lock_(symtab_lock), blocker_(blocker)
  { }

  // The standard Task methods.

  Is_runnable_type
  is_runnable(Workqueue*);

  Task_locker*
  locks(Workqueue*);

  void
  run(Workqueue*);

 private:
  class Scan_relocs_locker;

  const General_options& options_;
  Symbol_table* symtab_;
  Layout* layout_;
  Relobj* object_;
  Read_relocs_data* rd_;
  Task_token* symtab_lock_;
  Task_token* blocker_;
};

// A class to perform all the relocations for an object file.

class Relocate_task : public Task
{
 public:
  Relocate_task(const General_options& options, const Symbol_table* symtab,
		const Layout* layout, Relobj* object, Output_file* of,
		Task_token* final_blocker)
    : options_(options), symtab_(symtab), layout_(layout), object_(object),
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
  const Layout* layout_;
  Relobj* object_;
  Output_file* of_;
  Task_token* final_blocker_;
};

// Standard relocation routines which are used on many targets.  Here
// SIZE and BIG_ENDIAN refer to the target, not the relocation type.

template<int size, bool big_endian>
class Relocate_functions
{
private:
  // Do a simple relocation with the addend in the section contents.
  // VALSIZE is the size of the value.
  template<int valsize>
  static inline void
  rel(unsigned char* view,
      typename elfcpp::Swap<valsize, big_endian>::Valtype value)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype x = elfcpp::Swap<valsize, big_endian>::readval(wv);
    elfcpp::Swap<valsize, big_endian>::writeval(wv, x + value);
  }

  // Do a simple PC relative relocation with the addend in the section
  // contents.  VALSIZE is the size of the value.
  template<int valsize>
  static inline void
  pcrel(unsigned char* view,
	typename elfcpp::Swap<valsize, big_endian>::Valtype value,
	typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype x = elfcpp::Swap<valsize, big_endian>::readval(wv);
    elfcpp::Swap<valsize, big_endian>::writeval(wv, x + value - address);
  }

  typedef Relocate_functions<size, big_endian> This;

public:
  // Do a simple 8-bit REL relocation with the addend in the object
  // file data.
  static inline void
  rel8(unsigned char* view, unsigned char value)
  {
    This::template rel<8>(view, value);
  }

  // Do a simple 8-bit PC relative relocation with the addend in the
  // object file data.
  static inline void
  pcrel8(unsigned char* view, unsigned char value,
	 typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    This::template pcrel<8>(view, value, address);
  }

  // Do a simple 16-bit REL relocation with the addend in the object
  // file data.
  static inline void
  rel16(unsigned char* view, elfcpp::Elf_Half value)
  {
    This::template rel<16>(view, value);
  }

  // Do a simple 32-bit PC relative REL relocation with the addend in
  // the object file data.
  static inline void
  pcrel16(unsigned char* view, elfcpp::Elf_Word value,
	  typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    This::template pcrel<16>(view, value, address);
  }

  // Do a simple 32-bit REL relocation with the addend in the section
  // contents.
  static inline void
  rel32(unsigned char* view, elfcpp::Elf_Word value)
  {
    This::template rel<32>(view, value);
  }

  // Do a simple 32-bit PC relative REL relocation with the addend in
  // the section contents.
  static inline void
  pcrel32(unsigned char* view, elfcpp::Elf_Word value,
	  typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    This::template pcrel<32>(view, value, address);
  }

  // Do a simple 64-bit REL relocation with the addend in the section
  // contents.
  static inline void
  rel64(unsigned char* view, elfcpp::Elf_Xword value)
  {
    This::template rel<64>(view, value);
  }

  // Do a simple 64-bit PC relative REL relocation with the addend in
  // the section contents.
  static inline void
  pcrel64(unsigned char* view, elfcpp::Elf_Xword value,
	  typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    This::template pcrel<64>(view, value, address);
  }

  // Return whether we need a COPY reloc for a reloc against GSYM,
  // which is being applied to section SHNDX in OBJECT.
  static bool
  need_copy_reloc(const General_options*, Relobj* object, unsigned int shndx,
		  Symbol* gsym);
};

} // End namespace gold.

#endif // !defined(GOLD_RELOC_H)
