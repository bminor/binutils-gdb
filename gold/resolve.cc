// resolve.cc -- symbol resolution for gold

#include "gold.h"

#include "elfcpp.h"
#include "target.h"
#include "object.h"
#include "symtab.h"

namespace gold
{

// Resolve a symbol.  This is called the second and subsequent times
// we see a symbol.  TO is the pre-existing symbol.  SYM is the new
// symbol, seen in OBJECT.

template<int size, bool big_endian>
void
Symbol_table::resolve(Symbol* to,
		      const elfcpp::Sym<size, big_endian>& sym,
		      Object* object)
{
  if (object->target()->has_resolve())
    {
      object->sized_target<size, big_endian>()->resolve(to, sym, object);
      return;
    }

  // Build a little code for each symbol.
  // Bit 0: 0 for global, 1 for weak.
  // Bit 1: 0 for regular object, 1 for shared object
  // Bits 2-3: 0 for normal, 1 for undefined, 2 for common
  // This gives us values from 0 to 11:

  enum
  {
    DEF = 0,
    WEAK_DEF = 1,
    DYN_DEF = 2,
    DYN_WEAK_DEF = 3,
    UNDEF = 4,
    WEAK_UNDEF = 5,
    DYN_UNDEF = 6,
    DYN_WEAK_UNDEF = 7,
    COMMON = 8,
    WEAK_COMMON = 9,
    DYN_COMMON = 10,
    DYN_WEAK_COMMON = 11
  };

  int tobits;
  switch (to->binding())
    {
    case elfcpp::STB_GLOBAL:
      tobits = 0;
      break;

    case elfcpp::STB_WEAK:
      tobits = 1;
      break;

    case elfcpp::STB_LOCAL:
      // We should only see externally visible symbols in the symbol
      // table.
      abort();

    default:
      // Any target which wants to handle STB_LOOS, etc., needs to
      // define a resolve method.
      abort();
    }

  if (to->object() != NULL && to->object()->is_dynamic())
    tobits |= (1 << 1);

  switch (to->shnum())
    {
    case elfcpp::SHN_UNDEF:
      tobits |= (1 << 2);
      break;

    case elfcpp::SHN_COMMON:
      tobits |= (2 << 2);
      break;

    default:
      break;
    }

  int frombits;
  switch (sym.get_st_bind())
    {
    case elfcpp::STB_GLOBAL:
      frombits = 0;
      break;

    case elfcpp::STB_WEAK:
      frombits = 1;
      break;

    case elfcpp::STB_LOCAL:
      fprintf(stderr,
	      _("%s: %s: invalid STB_LOCAL symbol %s in external symbols\n"),
	      program_name, object->name().c_str(), to->name());
      gold_exit(false);

    default:
      fprintf(stderr,
	      _("%s: %s: unsupported symbol binding %d for symbol %s\n"),
	      program_name, object->name().c_str(),
	      static_cast<int>(sym.get_st_bind()), to->name());
      gold_exit(false);
    }

  if (object->is_dynamic())
    frombits |= (1 << 1);

  switch (sym.get_st_shndx())
    {
    case elfcpp::SHN_UNDEF:
      frombits |= (1 << 2);
      break;

    case elfcpp::SHN_COMMON:
      frombits |= (2 << 2);
      break;

    default:
      break;
    }

  // We use a giant switch table for symbol resolution.  This code is
  // unwieldy, but: 1) it is efficient; 2) we definitely handle all
  // cases; 3) it is easy to change the handling of a particular case.
  // The alternative would be a series of conditionals, but it is easy
  // to get the ordering wrong.  This could also be done as a table,
  // but that is no easier to understand than this large switch
  // statement.

  switch (tobits * 16 + frombits)
    {
    case DEF * 16 + DEF:
      // Two definitions of the same symbol.
      fprintf(stderr, "%s: %s: multiple definition of %s\n",
	      program_name, object->name().c_str(), to->name());
      // FIXME: Report locations.  Record that we have seen an error.
      return;

    case WEAK_DEF * 16 + DEF:
      // In the original SVR4 linker, a weak definition followed by a
      // regular definition was treated as a multiple definition
      // error.  In the Solaris linker and the GNU linker, a weak
      // definition followed by a regular definition causes the
      // regular definition to be ignored.  We are currently
      // compatible with the GNU linker.  In the future we should add
      // a target specific option to change this.  FIXME.
      return;

    case DYN_DEF * 16 + DEF:
    case DYN_WEAK_DEF * 16 + DEF:
    case UNDEF * 16 + DEF:
    case WEAK_UNDEF * 16 + DEF:
    case DYN_UNDEF * 16 + DEF:
    case DYN_WEAK_UNDEF * 16 + DEF:
    case COMMON * 16 + DEF:
    case WEAK_COMMON * 16 + DEF:
    case DYN_COMMON * 16 + DEF:
    case DYN_WEAK_COMMON * 16 + DEF:

    case DEF * 16 + WEAK_DEF:
    case WEAK_DEF * 16 + WEAK_DEF:
    case DYN_DEF * 16 + WEAK_DEF:
    case DYN_WEAK_DEF * 16 + WEAK_DEF:
    case UNDEF * 16 + WEAK_DEF:
    case WEAK_UNDEF * 16 + WEAK_DEF:
    case DYN_UNDEF * 16 + WEAK_DEF:
    case DYN_WEAK_UNDEF * 16 + WEAK_DEF:
    case COMMON * 16 + WEAK_DEF:
    case WEAK_COMMON * 16 + WEAK_DEF:
    case DYN_COMMON * 16 + WEAK_DEF:
    case DYN_WEAK_COMMON * 16 + WEAK_DEF:

    case DEF * 16 + DYN_DEF:
    case WEAK_DEF * 16 + DYN_DEF:
    case DYN_DEF * 16 + DYN_DEF:
    case DYN_WEAK_DEF * 16 + DYN_DEF:
    case UNDEF * 16 + DYN_DEF:
    case WEAK_UNDEF * 16 + DYN_DEF:
    case DYN_UNDEF * 16 + DYN_DEF:
    case DYN_WEAK_UNDEF * 16 + DYN_DEF:
    case COMMON * 16 + DYN_DEF:
    case WEAK_COMMON * 16 + DYN_DEF:
    case DYN_COMMON * 16 + DYN_DEF:
    case DYN_WEAK_COMMON * 16 + DYN_DEF:

    case DEF * 16 + DYN_WEAK_DEF:
    case WEAK_DEF * 16 + DYN_WEAK_DEF:
    case DYN_DEF * 16 + DYN_WEAK_DEF:
    case DYN_WEAK_DEF * 16 + DYN_WEAK_DEF:
    case UNDEF * 16 + DYN_WEAK_DEF:
    case WEAK_UNDEF * 16 + DYN_WEAK_DEF:
    case DYN_UNDEF * 16 + DYN_WEAK_DEF:
    case DYN_WEAK_UNDEF * 16 + DYN_WEAK_DEF:
    case COMMON * 16 + DYN_WEAK_DEF:
    case WEAK_COMMON * 16 + DYN_WEAK_DEF:
    case DYN_COMMON * 16 + DYN_WEAK_DEF:
    case DYN_WEAK_COMMON * 16 + DYN_WEAK_DEF:

    case DEF * 16 + UNDEF:
    case WEAK_DEF * 16 + UNDEF:
    case DYN_DEF * 16 + UNDEF:
    case DYN_WEAK_DEF * 16 + UNDEF:
    case UNDEF * 16 + UNDEF:
    case WEAK_UNDEF * 16 + UNDEF:
    case DYN_UNDEF * 16 + UNDEF:
    case DYN_WEAK_UNDEF * 16 + UNDEF:
    case COMMON * 16 + UNDEF:
    case WEAK_COMMON * 16 + UNDEF:
    case DYN_COMMON * 16 + UNDEF:
    case DYN_WEAK_COMMON * 16 + UNDEF:

    case DEF * 16 + WEAK_UNDEF:
    case WEAK_DEF * 16 + WEAK_UNDEF:
    case DYN_DEF * 16 + WEAK_UNDEF:
    case DYN_WEAK_DEF * 16 + WEAK_UNDEF:
    case UNDEF * 16 + WEAK_UNDEF:
    case WEAK_UNDEF * 16 + WEAK_UNDEF:
    case DYN_UNDEF * 16 + WEAK_UNDEF:
    case DYN_WEAK_UNDEF * 16 + WEAK_UNDEF:
    case COMMON * 16 + WEAK_UNDEF:
    case WEAK_COMMON * 16 + WEAK_UNDEF:
    case DYN_COMMON * 16 + WEAK_UNDEF:
    case DYN_WEAK_COMMON * 16 + WEAK_UNDEF:

    case DEF * 16 + DYN_UNDEF:
    case WEAK_DEF * 16 + DYN_UNDEF:
    case DYN_DEF * 16 + DYN_UNDEF:
    case DYN_WEAK_DEF * 16 + DYN_UNDEF:
    case UNDEF * 16 + DYN_UNDEF:
    case WEAK_UNDEF * 16 + DYN_UNDEF:
    case DYN_UNDEF * 16 + DYN_UNDEF:
    case DYN_WEAK_UNDEF * 16 + DYN_UNDEF:
    case COMMON * 16 + DYN_UNDEF:
    case WEAK_COMMON * 16 + DYN_UNDEF:
    case DYN_COMMON * 16 + DYN_UNDEF:
    case DYN_WEAK_COMMON * 16 + DYN_UNDEF:

    case DEF * 16 + DYN_WEAK_UNDEF:
    case WEAK_DEF * 16 + DYN_WEAK_UNDEF:
    case DYN_DEF * 16 + DYN_WEAK_UNDEF:
    case DYN_WEAK_DEF * 16 + DYN_WEAK_UNDEF:
    case UNDEF * 16 + DYN_WEAK_UNDEF:
    case WEAK_UNDEF * 16 + DYN_WEAK_UNDEF:
    case DYN_UNDEF * 16 + DYN_WEAK_UNDEF:
    case DYN_WEAK_UNDEF * 16 + DYN_WEAK_UNDEF:
    case COMMON * 16 + DYN_WEAK_UNDEF:
    case WEAK_COMMON * 16 + DYN_WEAK_UNDEF:
    case DYN_COMMON * 16 + DYN_WEAK_UNDEF:
    case DYN_WEAK_COMMON * 16 + DYN_WEAK_UNDEF:

    case DEF * 16 + COMMON:
    case WEAK_DEF * 16 + COMMON:
    case DYN_DEF * 16 + COMMON:
    case DYN_WEAK_DEF * 16 + COMMON:
    case UNDEF * 16 + COMMON:
    case WEAK_UNDEF * 16 + COMMON:
    case DYN_UNDEF * 16 + COMMON:
    case DYN_WEAK_UNDEF * 16 + COMMON:
    case COMMON * 16 + COMMON:
    case WEAK_COMMON * 16 + COMMON:
    case DYN_COMMON * 16 + COMMON:
    case DYN_WEAK_COMMON * 16 + COMMON:

    case DEF * 16 + WEAK_COMMON:
    case WEAK_DEF * 16 + WEAK_COMMON:
    case DYN_DEF * 16 + WEAK_COMMON:
    case DYN_WEAK_DEF * 16 + WEAK_COMMON:
    case UNDEF * 16 + WEAK_COMMON:
    case WEAK_UNDEF * 16 + WEAK_COMMON:
    case DYN_UNDEF * 16 + WEAK_COMMON:
    case DYN_WEAK_UNDEF * 16 + WEAK_COMMON:
    case COMMON * 16 + WEAK_COMMON:
    case WEAK_COMMON * 16 + WEAK_COMMON:
    case DYN_COMMON * 16 + WEAK_COMMON:
    case DYN_WEAK_COMMON * 16 + WEAK_COMMON:

    case DEF * 16 + DYN_COMMON:
    case WEAK_DEF * 16 + DYN_COMMON:
    case DYN_DEF * 16 + DYN_COMMON:
    case DYN_WEAK_DEF * 16 + DYN_COMMON:
    case UNDEF * 16 + DYN_COMMON:
    case WEAK_UNDEF * 16 + DYN_COMMON:
    case DYN_UNDEF * 16 + DYN_COMMON:
    case DYN_WEAK_UNDEF * 16 + DYN_COMMON:
    case COMMON * 16 + DYN_COMMON:
    case WEAK_COMMON * 16 + DYN_COMMON:
    case DYN_COMMON * 16 + DYN_COMMON:
    case DYN_WEAK_COMMON * 16 + DYN_COMMON:

    case DEF * 16 + DYN_WEAK_COMMON:
    case WEAK_DEF * 16 + DYN_WEAK_COMMON:
    case DYN_DEF * 16 + DYN_WEAK_COMMON:
    case DYN_WEAK_DEF * 16 + DYN_WEAK_COMMON:
    case UNDEF * 16 + DYN_WEAK_COMMON:
    case WEAK_UNDEF * 16 + DYN_WEAK_COMMON:
    case DYN_UNDEF * 16 + DYN_WEAK_COMMON:
    case DYN_WEAK_UNDEF * 16 + DYN_WEAK_COMMON:
    case COMMON * 16 + DYN_WEAK_COMMON:
    case WEAK_COMMON * 16 + DYN_WEAK_COMMON:
    case DYN_COMMON * 16 + DYN_WEAK_COMMON:
    case DYN_WEAK_COMMON * 16 + DYN_WEAK_COMMON:

      break;
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones needed for implemented
// targets.

template
void
Symbol_table::resolve<32, true>(
    Symbol* to,
    const elfcpp::Sym<32, true>& sym,
    Object* object);

template
void
Symbol_table::resolve<32, false>(
    Symbol* to,
    const elfcpp::Sym<32, false>& sym,
    Object* object);

template
void
Symbol_table::resolve<64, true>(
    Symbol* to,
    const elfcpp::Sym<64, true>& sym,
    Object* object);

template
void
Symbol_table::resolve<64, false>(
    Symbol* to,
    const elfcpp::Sym<64, false>& sym,
    Object* object);

} // End namespace gold.
