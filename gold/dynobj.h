// dynobj.h -- dynamic object support for gold   -*- C++ -*-

#ifndef GOLD_DYNOBJ_H
#define GOLD_DYNOBJ_H

#include "object.h"

namespace gold
{

// A dynamic object (ET_DYN).  This is an abstract base class itself.
// The implementations is the template class Sized_dynobj.

class Dynobj : public Object
{
 public:
  Dynobj(const std::string& name, Input_file* input_file, off_t offset = 0)
    : Object(name, input_file, true, offset)
  { }
};

// A dynamic object, size and endian specific version.

template<int size, bool big_endian>
class Sized_dynobj : public Dynobj
{
 public:
  Sized_dynobj(const std::string& name, Input_file* input_file, off_t offset,
	       const typename elfcpp::Ehdr<size, big_endian>&);

  // Read the symbols.
  void
  do_read_symbols(Read_symbols_data*);

  // Lay out the input sections.
  void
  do_layout(const General_options&, Symbol_table*, Layout*,
	    Read_symbols_data*);

  // Add the symbols to the symbol table.
  void
  do_add_symbols(Symbol_table*, Read_symbols_data*);

  // Return a view of the contents of a section.  Set *PLEN to the
  // size.
  const unsigned char*
  do_section_contents(unsigned int shnum, off_t* plen) = 0;

  // Get the name of a section.
  std::string
  do_section_name(unsigned int shnum);
};

} // End namespace gold.

#endif // !defined(GOLD_DYNOBJ_H)
