// symtab.cc -- the gold symbol table

#include "gold.h"

#include <cassert>
#include <stdint.h>
#include <string>
#include <utility>

#include "object.h"
#include "symtab.h"

namespace gold
{

// Class Symbol.

// Initialize the fields in the base class Symbol.

template<int size, bool big_endian>
void
Symbol::init_base(const char* name, const char* version, Object* object,
		  const elfcpp::Sym<size, big_endian>& sym)
{
  this->name_ = name;
  this->version_ = version;
  this->object_ = object;
  this->shnum_ = sym.get_st_shndx(); // FIXME: Handle SHN_XINDEX.
  this->type_ = sym.get_st_type();
  this->binding_ = sym.get_st_bind();
  this->visibility_ = sym.get_st_visibility();
  this->other_ = sym.get_st_nonvis();
  this->is_special_ = false;
  this->is_def_ = false;
  this->is_forwarder_ = false;
  this->in_dyn_ = object->is_dynamic();
}

// Initialize the fields in Sized_symbol.

template<int size>
template<bool big_endian>
void
Sized_symbol<size>::init(const char* name, const char* version, Object* object,
			 const elfcpp::Sym<size, big_endian>& sym)
{
  this->init_base(name, version, object, sym);
  this->value_ = sym.get_st_value();
  this->size_ = sym.get_st_size();
}

// Class Symbol_table.

Symbol_table::Symbol_table()
  : size_(0), table_(), namepool_(), output_pool_(), forwarders_()
{
}

Symbol_table::~Symbol_table()
{
}

// The hash function.  The key is always canonicalized, so we use a
// simple combination of the pointers.

size_t
Symbol_table::Symbol_table_hash::operator()(const Symbol_table_key& key) const
{
  return (reinterpret_cast<size_t>(key.first)
	  ^ reinterpret_cast<size_t>(key.second));
}

// The symbol table key equality function.  This is only called with
// canonicalized name and version strings, so we can use pointer
// comparison.

bool
Symbol_table::Symbol_table_eq::operator()(const Symbol_table_key& k1,
					  const Symbol_table_key& k2) const
{
  return k1.first == k2.first && k1.second == k2.second;
}

// Make TO a symbol which forwards to FROM.  

void
Symbol_table::make_forwarder(Symbol* from, Symbol* to)
{
  assert(!from->is_forwarder() && !to->is_forwarder());
  this->forwarders_[from] = to;
  from->set_forwarder();
}

Symbol*
Symbol_table::resolve_forwards(Symbol* from) const
{
  assert(from->is_forwarder());
  Unordered_map<Symbol*, Symbol*>::const_iterator p =
    this->forwarders_.find(from);
  assert(p != this->forwarders_.end());
  return p->second;
}

// Resolve a Symbol with another Symbol.  This is only used in the
// unusual case where there are references to both an unversioned
// symbol and a symbol with a version, and we then discover that that
// version is the default version.  Because this is unusual, we do
// this the slow way, by converting back to an ELF symbol.

#ifdef HAVE_MEMBER_TEMPLATE_SPECIFICATIONS

template<int size, bool big_endian>
void
Symbol_table::resolve(Sized_symbol<size>* to, const Sized_symbol<size>* from)
{
  unsigned char buf[elfcpp::Elf_sizes<size>::sym_size];
  elfcpp::Sym_write<size, big_endian> esym(buf);
  // We don't bother to set the st_name field.
  esym.put_st_value(from->value());
  esym.put_st_size(from->symsize());
  esym.put_st_info(from->binding(), from->type());
  esym.put_st_other(from->visibility(), from->other());
  esym.put_st_shndx(from->shnum());
  Symbol_table::resolve(to, esym.sym(), from->object());
}

#else

template<int size>
void
Symbol_table::resolve(Sized_symbol<size>* to, const Sized_symbol<size>* from,
                      bool big_endian)
{
  unsigned char buf[elfcpp::Elf_sizes<size>::sym_size];
  if (big_endian)
    {
      elfcpp::Sym_write<size, true> esym(buf);
      // We don't bother to set the st_name field.
      esym.put_st_value(from->value());
      esym.put_st_size(from->symsize());
      esym.put_st_info(from->binding(), from->type());
      esym.put_st_other(from->visibility(), from->other());
      esym.put_st_shndx(from->shnum());
      Symbol_table::resolve(to, esym.sym(), from->object());
    }
  else
    {
      elfcpp::Sym_write<size, false> esym(buf);
      // We don't bother to set the st_name field.
      esym.put_st_value(from->value());
      esym.put_st_size(from->symsize());
      esym.put_st_info(from->binding(), from->type());
      esym.put_st_other(from->visibility(), from->other());
      esym.put_st_shndx(from->shnum());
      Symbol_table::resolve(to, esym.sym(), from->object());
    }
}

#endif

// Add one symbol from OBJECT to the symbol table.  NAME is symbol
// name and VERSION is the version; both are canonicalized.  DEF is
// whether this is the default version.

// If DEF is true, then this is the definition of a default version of
// a symbol.  That means that any lookup of NAME/NULL and any lookup
// of NAME/VERSION should always return the same symbol.  This is
// obvious for references, but in particular we want to do this for
// definitions: overriding NAME/NULL should also override
// NAME/VERSION.  If we don't do that, it would be very hard to
// override functions in a shared library which uses versioning.

// We implement this by simply making both entries in the hash table
// point to the same Symbol structure.  That is easy enough if this is
// the first time we see NAME/NULL or NAME/VERSION, but it is possible
// that we have seen both already, in which case they will both have
// independent entries in the symbol table.  We can't simply change
// the symbol table entry, because we have pointers to the entries
// attached to the object files.  So we mark the entry attached to the
// object file as a forwarder, and record it in the forwarders_ map.
// Note that entries in the hash table will never be marked as
// forwarders.

template<int size, bool big_endian>
Symbol*
Symbol_table::add_from_object(Sized_object<size, big_endian>* object,
			      const char *name,
			      const char *version, bool def,
			      const elfcpp::Sym<size, big_endian>& sym)
{
  Symbol* const snull = NULL;
  std::pair<typename Symbol_table_type::iterator, bool> ins =
    this->table_.insert(std::make_pair(std::make_pair(name, version), snull));

  std::pair<typename Symbol_table_type::iterator, bool> insdef =
    std::make_pair(this->table_.end(), false);
  if (def)
    {
      const char* const vnull = NULL;
      insdef = this->table_.insert(std::make_pair(std::make_pair(name, vnull),
						  snull));
    }

  // ins.first: an iterator, which is a pointer to a pair.
  // ins.first->first: the key (a pair of name and version).
  // ins.first->second: the value (Symbol*).
  // ins.second: true if new entry was inserted, false if not.

  Sized_symbol<size>* ret;
  if (!ins.second)
    {
      // We already have an entry for NAME/VERSION.
#ifdef HAVE_MEMBER_TEMPLATE_SPECIFICATIONS
      ret = this->get_sized_symbol<size>(ins.first->second);
#else
      assert(size == this->get_size());
      ret = static_cast<Sized_symbol<size>*>(ins.first->second);
#endif
      assert(ret != NULL);
      Symbol_table::resolve(ret, sym, object);

      if (def)
	{
	  if (insdef.second)
	    {
	      // This is the first time we have seen NAME/NULL.  Make
	      // NAME/NULL point to NAME/VERSION.
	      insdef.first->second = ret;
	    }
	  else
	    {
	      // This is the unfortunate case where we already have
	      // entries for both NAME/VERSION and NAME/NULL.
	      const Sized_symbol<size>* sym2;
#ifdef HAVE_MEMBER_TEMPLATE_SPECIFICATIONS
	      sym2 = this->get_sized_symbol<size>(insdef.first->second);
	      Symbol_table::resolve<size, big_endian>(ret, sym2);
#else
	      sym2 = static_cast<Sized_symbol<size>*>(insdef.first->second);
	      Symbol_table::resolve(ret, sym2, big_endian);
#endif
	      this->make_forwarder(insdef.first->second, ret);
	      insdef.first->second = ret;
	    }
	}
    }
  else
    {
      // This is the first time we have seen NAME/VERSION.
      assert(ins.first->second == NULL);
      if (def && !insdef.second)
	{
	  // We already have an entry for NAME/NULL.  Make
	  // NAME/VERSION point to it.
#ifdef HAVE_MEMBER_TEMPLATE_SPECIFICATIONS
	  ret = this->get_sized_symbol<size>(insdef.first->second);
#else
          ret = static_cast<Sized_symbol<size>*>(insdef.first->second);
#endif
	  Symbol_table::resolve(ret, sym, object);
	  ins.first->second = ret;
	}
      else
	{
	  Sized_target<size, big_endian>* target = object->sized_target();
	  if (!target->has_make_symbol())
	    ret = new Sized_symbol<size>();
	  else
	    {
	      ret = target->make_symbol();
	      if (ret == NULL)
		{
		  // This means that we don't want a symbol table
		  // entry after all.
		  if (!def)
		    this->table_.erase(ins.first);
		  else
		    {
		      this->table_.erase(insdef.first);
		      // Inserting insdef invalidated ins.
		      this->table_.erase(std::make_pair(name, version));
		    }
		  return NULL;
		}
	    }

	  ret->init(name, version, object, sym);

	  ins.first->second = ret;
	  if (def)
	    {
	      // This is the first time we have seen NAME/NULL.  Point
	      // it at the new entry for NAME/VERSION.
	      assert(insdef.second);
	      insdef.first->second = ret;
	    }
	}
    }

  return ret;
}

// Add all the symbols in an object to the hash table.

template<int size, bool big_endian>
void
Symbol_table::add_from_object(
    Sized_object<size, big_endian>* object,
    const elfcpp::Sym<size, big_endian>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size,
    Symbol** sympointers)
{
  // We take the size from the first object we see.
  if (this->get_size() == 0)
    this->set_size(size);

  if (size != this->get_size() || size != object->target()->get_size())
    {
      fprintf(stderr, _("%s: %s: mixing 32-bit and 64-bit ELF objects\n"),
	      program_name, object->name().c_str());
      gold_exit(false);
    }

  const unsigned char* p = reinterpret_cast<const unsigned char*>(syms);
  for (size_t i = 0; i < count; ++i)
    {
      elfcpp::Sym<size, big_endian> sym(p);

      unsigned int st_name = sym.get_st_name();
      if (st_name >= sym_name_size)
	{
	  fprintf(stderr,
		  _("%s: %s: bad global symbol name offset %u at %lu\n"),
		  program_name, object->name().c_str(), st_name,
		  static_cast<unsigned long>(i));
	  gold_exit(false);
	}

      const char* name = sym_names + st_name;

      // In an object file, an '@' in the name separates the symbol
      // name from the version name.  If there are two '@' characters,
      // this is the default version.
      const char* ver = strchr(name, '@');

      Symbol* res;
      if (ver == NULL)
	{
	  name = this->namepool_.add(name);
	  res = this->add_from_object(object, name, NULL, false, sym);
	}
      else
	{
	  name = this->namepool_.add(name, ver - name);
	  bool def = false;
	  ++ver;
	  if (*ver == '@')
	    {
	      def = true;
	      ++ver;
	    }
	  ver = this->namepool_.add(ver);
	  res = this->add_from_object(object, name, ver, def, sym);
	}

      *sympointers++ = res;

      p += elfcpp::Elf_sizes<size>::sym_size;
    }
}

// Record the names of the local symbols for an object.

template<int size, bool big_endian>
void
Symbol_table::add_local_symbol_names(Sized_object<size, big_endian>* object,
				     const elfcpp::Sym<size, big_endian>* syms,
				     size_t count, const char* sym_names,
				     size_t sym_name_size)
{
  const unsigned char* p = reinterpret_cast<const unsigned char*>(syms);
  for (size_t i = 0; i < count; ++i)
    {
      elfcpp::Sym<size, big_endian> sym(p);

      unsigned int st_name = sym.get_st_name();
      if (st_name >= sym_name_size)
	{
	  fprintf(stderr,
		  _("%s: %s: bad local symbol name offset %u at %lu\n"),
		  program_name, object->name().c_str(), st_name,
		  static_cast<unsigned long>(i));
	  gold_exit(false);
	}

      this->output_pool_.add(sym_names + st_name);
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones needed for implemented
// targets.

template
void
Symbol_table::add_from_object<32, true>(
    Sized_object<32, true>* object,
    const elfcpp::Sym<32, true>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size,
    Symbol** sympointers);

template
void
Symbol_table::add_from_object<32, false>(
    Sized_object<32, false>* object,
    const elfcpp::Sym<32, false>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size,
    Symbol** sympointers);

template
void
Symbol_table::add_from_object<64, true>(
    Sized_object<64, true>* object,
    const elfcpp::Sym<64, true>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size,
    Symbol** sympointers);

template
void
Symbol_table::add_from_object<64, false>(
    Sized_object<64, false>* object,
    const elfcpp::Sym<64, false>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size,
    Symbol** sympointers);

template
void
Symbol_table::add_local_symbol_names<32, true>(
    Sized_object<32, true>* object,
    const elfcpp::Sym<32, true>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size);

template
void
Symbol_table::add_local_symbol_names<32, false>(
    Sized_object<32, false>* object,
    const elfcpp::Sym<32, false>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size);

template
void
Symbol_table::add_local_symbol_names<64, true>(
    Sized_object<64, true>* object,
    const elfcpp::Sym<64, true>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size);

template
void
Symbol_table::add_local_symbol_names<64, false>(
    Sized_object<64, false>* object,
    const elfcpp::Sym<64, false>* syms,
    size_t count,
    const char* sym_names,
    size_t sym_name_size);

} // End namespace gold.
