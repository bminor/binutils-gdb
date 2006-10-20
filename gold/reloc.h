// reloc.h -- relocate input files for gold   -*- C++ -*-

#ifndef GOLD_RELOC_H
#define GOLD_RELOC_H

#include <byteswap.h>

#include "workqueue.h"

namespace gold
{

class Object;
class Read_relocs_data;
class Stringpool;

// A class to read the relocations for an object file, and then queue
// up a task to see if they require any GOT/PLT/COPY relocations in
// the symbol table.

class Read_relocs : public Task
{
 public:
  // SYMTAB_LOCK is used to lock the symbol table.  BLOCKER should be
  // unblocked when the Scan_relocs task completes.
  Read_relocs(const General_options& options, Symbol_table* symtab,
	      Object* object, Task_token* symtab_lock,
	      Task_token* blocker)
    : options_(options), symtab_(symtab), object_(object),
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
  Object* object_;
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
	      Object* object, Read_relocs_data* rd, Task_token* symtab_lock,
	      Task_token* blocker)
    : options_(options), symtab_(symtab), object_(object), rd_(rd),
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
  class Scan_relocs_locker;

  const General_options& options_;
  Symbol_table* symtab_;
  Object* object_;
  Read_relocs_data* rd_;
  Task_token* symtab_lock_;
  Task_token* blocker_;
};

// A class to perform all the relocations for an object file.

class Relocate_task : public Task
{
 public:
  Relocate_task(const General_options& options, const Symbol_table* symtab,
		const Layout* layout, Object* object, Output_file* of,
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
  Object* object_;
  Output_file* of_;
  Task_token* final_blocker_;
};

// Integer swapping routines used by relocation functions.  FIXME:
// Maybe these should be more general, and/or shared with elfcpp.

// Endian simply indicates whether the host is big endian or not,
// based on the results of the configure script.

struct Endian
{
 public:
  // Used for template specializations.
#ifdef WORDS_BIGENDIAN
  static const bool host_big_endian = true;
#else
  static const bool host_big_endian = false;
#endif
};

// Valtype_base is a template based on size (8, 16, 32, 64) which
// defines a typedef Valtype for the unsigned integer of the specified
// size.

template<int size>
struct Valtype_base;

template<>
struct Valtype_base<8>
{
  typedef unsigned char Valtype;
};

template<>
struct Valtype_base<16>
{
  typedef uint16_t Valtype;
};

template<>
struct Valtype_base<32>
{
  typedef uint32_t Valtype;
};

template<>
struct Valtype_base<64>
{
  typedef uint64_t Valtype;
};

// Convert_host is a template based on size and on whether the host
// and target have the same endianness.  It defines the type Valtype,
// and defines a function convert_host which takes an argument of type
// Valtype and swaps it if the host and target have different
// endianness.

template<int size, bool same_endian>
struct Convert_host;

template<int size>
struct Convert_host<size, true>
{
  typedef typename Valtype_base<size>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return v; }
};

template<>
struct Convert_host<8, false>
{
  typedef Valtype_base<8>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return v; }
};

template<>
struct Convert_host<16, false>
{
  typedef Valtype_base<16>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return bswap_16(v); }
};

template<>
struct Convert_host<32, false>
{
  typedef Valtype_base<32>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return bswap_32(v); }
};

template<>
struct Convert_host<64, false>
{
  typedef Valtype_base<64>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return bswap_64(v); }
};

// Convert is a template based on size and on whether we have a big
// endian target.  It defines Valtype and convert_host like
// Convert_host.  That is, it is just like Convert_host except in the
// meaning of the second template parameter.

template<int size, bool big_endian>
struct Convert
{
  typedef typename Valtype_base<size>::Valtype Valtype;

  static inline Valtype
  convert_host(Valtype v)
  { return Convert_host<size, big_endian == Endian::host_big_endian>
      ::convert_host(v); }
};

// Swap is a template based on size and on whether the target is big
// endian.  It defines the type Valtype and the functions readval and
// writeval.  The functions read and write values of the appropriate
// size out of buffers, swapping them if necessary.

template<int size, bool big_endian>
struct Swap
{
  typedef typename Valtype_base<size>::Valtype Valtype;

  static inline Valtype
  readval(const Valtype* wv)
  { return Convert<size, big_endian>::convert_host(*wv); }

  static inline void
  writeval(Valtype* wv, Valtype v)
  { *wv = Convert<size, big_endian>::convert_host(v); }
};

// Swap_unaligned is a template based on size and on whether the
// target is big endian.  It defines the type Valtype and the
// functions readval_unaligned and writeval_unaligned.  The functions
// read and write values of the appropriate size out of buffers which
// may be misaligned.

template<int size, bool big_endian>
class Swap_unaligned;

template<bool big_endian>
class Swap_unaligned<8, big_endian>
{
public:
  typedef typename Valtype_base<8>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  { return *wv; }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  { *wv = v; }
};

template<>
class Swap_unaligned<16, false>
{
public:
  typedef Valtype_base<16>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return (wv[1] << 8) | wv[0];
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[1] = v >> 8;
    wv[0] = v;
  }
};

template<>
class Swap_unaligned<16, true>
{
public:
  typedef Valtype_base<16>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return (wv[0] << 8) | wv[1];
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[0] = v >> 8;
    wv[1] = v;
  }
};

template<>
class Swap_unaligned<32, false>
{
public:
  typedef Valtype_base<32>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return (wv[3] << 24) | (wv[2] << 16) | (wv[1] << 8) | wv[0];
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[3] = v >> 24;
    wv[2] = v >> 16;
    wv[1] = v >> 8;
    wv[0] = v;
  }
};

template<>
class Swap_unaligned<32, true>
{
public:
  typedef Valtype_base<32>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return (wv[0] << 24) | (wv[1] << 16) | (wv[2] << 8) | wv[3];
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[0] = v >> 24;
    wv[1] = v >> 16;
    wv[2] = v >> 8;
    wv[3] = v;
  }
};

template<>
class Swap_unaligned<64, false>
{
public:
  typedef Valtype_base<64>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return ((static_cast<Valtype>(wv[7]) << 56)
	    | (static_cast<Valtype>(wv[6]) << 48)
	    | (static_cast<Valtype>(wv[5]) << 40)
	    | (static_cast<Valtype>(wv[4]) << 32)
	    | (static_cast<Valtype>(wv[3]) << 24)
	    | (static_cast<Valtype>(wv[2]) << 16)
	    | (static_cast<Valtype>(wv[1]) << 8)
	    | static_cast<Valtype>(wv[0]));
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[7] = v >> 56;
    wv[6] = v >> 48;
    wv[5] = v >> 40;
    wv[4] = v >> 32;
    wv[3] = v >> 24;
    wv[2] = v >> 16;
    wv[1] = v >> 8;
    wv[0] = v;
  }
};

template<>
class Swap_unaligned<64, true>
{
public:
  typedef Valtype_base<64>::Valtype Valtype;

  static inline Valtype
  readval_unaligned(const unsigned char* wv)
  {
    return ((static_cast<Valtype>(wv[0]) << 56)
	    | (static_cast<Valtype>(wv[1]) << 48)
	    | (static_cast<Valtype>(wv[2]) << 40)
	    | (static_cast<Valtype>(wv[3]) << 32)
	    | (static_cast<Valtype>(wv[4]) << 24)
	    | (static_cast<Valtype>(wv[5]) << 16)
	    | (static_cast<Valtype>(wv[6]) << 8)
	    | static_cast<Valtype>(wv[7]));
  }

  static inline void
  writeval_unaligned(unsigned char* wv, Valtype v)
  {
    wv[7] = v >> 56;
    wv[6] = v >> 48;
    wv[5] = v >> 40;
    wv[4] = v >> 32;
    wv[3] = v >> 24;
    wv[2] = v >> 16;
    wv[1] = v >> 8;
    wv[0] = v;
  }
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
  rel(unsigned char* view, typename Swap<valsize, big_endian>::Valtype value)
  {
    typedef typename Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype x = Swap<valsize, big_endian>::readval(wv);
    Swap<valsize, big_endian>::writeval(wv, x + value);
  }

  // Do a simple PC relative relocation with the addend in the section
  // contents.  VALSIZE is the size of the value.
  template<int valsize>
  static inline void
  pcrel(unsigned char* view, typename Swap<valsize, big_endian>::Valtype value,
	typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    typedef typename Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype x = Swap<valsize, big_endian>::readval(wv);
    Swap<valsize, big_endian>::writeval(wv, x + value - address);
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
  rel64(unsigned char* view, elfcpp::Elf_Word value)
  {
    This::template rel<64>(view, value);
  }

  // Do a simple 64-bit PC relative REL relocation with the addend in
  // the section contents.
  static inline void
  pcrel64(unsigned char* view, elfcpp::Elf_Word value,
	  typename elfcpp::Elf_types<size>::Elf_Addr address)
  {
    This::template pcrel<64>(view, value, address);
  }
};

} // End namespace gold.

#endif // !defined(GOLD_RELOC_H)
