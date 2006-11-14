// dynobj.cc -- dynamic object support for gold

#include "gold.h"

#include <vector>
#include <cstring>

#include "symtab.h"
#include "dynobj.h"

namespace gold
{

// Class Sized_dynobj.

template<int size, bool big_endian>
Sized_dynobj<size, big_endian>::Sized_dynobj(
    const std::string& name,
    Input_file* input_file,
    off_t offset,
    const elfcpp::Ehdr<size, big_endian>& ehdr)
  : Dynobj(name, input_file, offset),
    elf_file_(this, ehdr),
    soname_()
{
}

// Set up the object.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::setup(
    const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  this->set_target(ehdr.get_e_machine(), size, big_endian,
		   ehdr.get_e_ident()[elfcpp::EI_OSABI],
		   ehdr.get_e_ident()[elfcpp::EI_ABIVERSION]);

  const unsigned int shnum = this->elf_file_.shnum();
  this->set_shnum(shnum);
}

// Find the SHT_DYNSYM section and the various version sections, and
// the dynamic section, given the section headers.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::find_dynsym_sections(
    const unsigned char* pshdrs,
    unsigned int* pdynsym_shndx,
    unsigned int* pversym_shndx,
    unsigned int* pverdef_shndx,
    unsigned int* pverneed_shndx,
    unsigned int* pdynamic_shndx)
{
  *pdynsym_shndx = -1U;
  *pversym_shndx = -1U;
  *pverdef_shndx = -1U;
  *pverneed_shndx = -1U;
  *pdynamic_shndx = -1U;

  const unsigned int shnum = this->shnum();
  const unsigned char* p = pshdrs;
  for (unsigned int i = 0; i < shnum; ++i, p += This::shdr_size)
    {
      typename This::Shdr shdr(p);

      unsigned int* pi;
      switch (shdr.get_sh_type())
	{
	case elfcpp::SHT_DYNSYM:
	  pi = pdynsym_shndx;
	  break;
	case elfcpp::SHT_GNU_versym:
	  pi = pversym_shndx;
	  break;
	case elfcpp::SHT_GNU_verdef:
	  pi = pverdef_shndx;
	  break;
	case elfcpp::SHT_GNU_verneed:
	  pi = pverneed_shndx;
	  break;
	case elfcpp::SHT_DYNAMIC:
	  pi = pdynamic_shndx;
	  break;
	default:
	  pi = NULL;
	  break;
	}

      if (pi == NULL)
	continue;

      if (*pi != -1U)
	{
	  fprintf(stderr,
		  _("%s: %s: unexpected duplicate type %u section: %u, %u\n"),
		  program_name, this->name().c_str(), shdr.get_sh_type(),
		  *pi, i);
	  gold_exit(false);
	}

      *pi = i;
    }
}

// Read the contents of section SHNDX.  PSHDRS points to the section
// headers.  TYPE is the expected section type.  LINK is the expected
// section link.  Store the data in *VIEW and *VIEW_SIZE.  The
// section's sh_info field is stored in *VIEW_INFO.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::read_dynsym_section(
    const unsigned char* pshdrs,
    unsigned int shndx,
    elfcpp::SHT type,
    unsigned int link,
    File_view** view,
    off_t* view_size,
    unsigned int* view_info)
{
  if (shndx == -1U)
    {
      *view = NULL;
      *view_size = 0;
      *view_info = 0;
      return;
    }

  typename This::Shdr shdr(pshdrs + shndx * This::shdr_size);

  assert(shdr.get_sh_type() == type);

  if (shdr.get_sh_link() != link)
    {
      fprintf(stderr,
	      _("%s: %s: unexpected link in section %u header: %u != %u\n"),
	      program_name, this->name().c_str(), shndx,
	      shdr.get_sh_link(), link);
      gold_exit(false);
    }

  *view = this->get_lasting_view(shdr.get_sh_offset(), shdr.get_sh_size());
  *view_size = shdr.get_sh_size();
  *view_info = shdr.get_sh_info();
}

// Set soname_ if this shared object has a DT_SONAME tag.  PSHDRS
// points to the section headers.  DYNAMIC_SHNDX is the section index
// of the SHT_DYNAMIC section.  STRTAB_SHNDX, STRTAB, and STRTAB_SIZE
// are the section index and contents of a string table which may be
// the one associated with the SHT_DYNAMIC section.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::set_soname(const unsigned char* pshdrs,
					   unsigned int dynamic_shndx,
					   unsigned int strtab_shndx,
					   const unsigned char* strtabu,
					   off_t strtab_size)
{
  typename This::Shdr dynamicshdr(pshdrs + dynamic_shndx * This::shdr_size);
  assert(dynamicshdr.get_sh_type() == elfcpp::SHT_DYNAMIC);

  const off_t dynamic_size = dynamicshdr.get_sh_size();
  const unsigned char* pdynamic = this->get_view(dynamicshdr.get_sh_offset(),
						 dynamic_size);

  const unsigned int link = dynamicshdr.get_sh_link();
  if (link != strtab_shndx)
    {
      if (link >= this->shnum())
	{
	  fprintf(stderr,
		  _("%s: %s: DYNAMIC section %u link out of range: %u\n"),
		  program_name, this->name().c_str(),
		  dynamic_shndx, link);
	  gold_exit(false);
	}

      typename This::Shdr strtabshdr(pshdrs + link * This::shdr_size);
      if (strtabshdr.get_sh_type() != elfcpp::SHT_STRTAB)
	{
	  fprintf(stderr,
		  _("%s: %s: DYNAMIC section %u link %u is not a strtab\n"),
		  program_name, this->name().c_str(),
		  dynamic_shndx, link);
	  gold_exit(false);
	}

      strtab_size = strtabshdr.get_sh_size();
      strtabu = this->get_view(strtabshdr.get_sh_offset(), strtab_size);
    }

  for (const unsigned char* p = pdynamic;
       p < pdynamic + dynamic_size;
       p += This::dyn_size)
    {
      typename This::Dyn dyn(p);

      if (dyn.get_d_tag() == elfcpp::DT_SONAME)
	{
	  off_t val = dyn.get_d_val();
	  if (val >= strtab_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: DT_SONAME value out of range: "
			"%lld >= %lld\n"),
		      program_name, this->name().c_str(),
		      static_cast<long long>(val),
		      static_cast<long long>(strtab_size));
	      gold_exit(false);
	    }

	  const char* strtab = reinterpret_cast<const char*>(strtabu);
	  this->soname_ = std::string(strtab + val);
	  return;
	}

      if (dyn.get_d_tag() == elfcpp::DT_NULL)
	return;
    }

  fprintf(stderr, _("%s: %s: missing DT_NULL in dynamic segment\n"),
	  program_name, this->name().c_str());
  gold_exit(false);
}

// Read the symbols and sections from a dynamic object.  We read the
// dynamic symbols, not the normal symbols.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  this->read_section_data(&this->elf_file_, sd);

  const unsigned char* const pshdrs = sd->section_headers->data();

  unsigned int dynsym_shndx;
  unsigned int versym_shndx;
  unsigned int verdef_shndx;
  unsigned int verneed_shndx;
  unsigned int dynamic_shndx;
  this->find_dynsym_sections(pshdrs, &dynsym_shndx, &versym_shndx,
			     &verdef_shndx, &verneed_shndx, &dynamic_shndx);

  unsigned int strtab_shndx = -1U;

  if (dynsym_shndx == -1U)
    {
      sd->symbols = NULL;
      sd->symbols_size = 0;
      sd->symbol_names = NULL;
      sd->symbol_names_size = 0;
    }
  else
    {
      // Get the dynamic symbols.
      typename This::Shdr dynsymshdr(pshdrs + dynsym_shndx * This::shdr_size);
      assert(dynsymshdr.get_sh_type() == elfcpp::SHT_DYNSYM);

      sd->symbols = this->get_lasting_view(dynsymshdr.get_sh_offset(),
					   dynsymshdr.get_sh_size());
      sd->symbols_size = dynsymshdr.get_sh_size();

      // Get the symbol names.
      strtab_shndx = dynsymshdr.get_sh_link();
      if (strtab_shndx >= this->shnum())
	{
	  fprintf(stderr,
		  _("%s: %s: invalid dynamic symbol table name index: %u\n"),
		  program_name, this->name().c_str(), strtab_shndx);
	  gold_exit(false);
	}
      typename This::Shdr strtabshdr(pshdrs + strtab_shndx * This::shdr_size);
      if (strtabshdr.get_sh_type() != elfcpp::SHT_STRTAB)
	{
	  fprintf(stderr,
		  _("%s: %s: dynamic symbol table name section "
		    "has wrong type: %u\n"),
		  program_name, this->name().c_str(),
		  static_cast<unsigned int>(strtabshdr.get_sh_type()));
	  gold_exit(false);
	}

      sd->symbol_names = this->get_lasting_view(strtabshdr.get_sh_offset(),
						strtabshdr.get_sh_size());
      sd->symbol_names_size = strtabshdr.get_sh_size();

      // Get the version information.

      unsigned int dummy;
      this->read_dynsym_section(pshdrs, versym_shndx, elfcpp::SHT_GNU_versym,
				dynsym_shndx, &sd->versym, &sd->versym_size,
				&dummy);

      // We require that the version definition and need section link
      // to the same string table as the dynamic symbol table.  This
      // is not a technical requirement, but it always happens in
      // practice.  We could change this if necessary.

      this->read_dynsym_section(pshdrs, verdef_shndx, elfcpp::SHT_GNU_verdef,
				strtab_shndx, &sd->verdef, &sd->verdef_size,
				&sd->verdef_info);

      this->read_dynsym_section(pshdrs, verneed_shndx, elfcpp::SHT_GNU_verneed,
				strtab_shndx, &sd->verneed, &sd->verneed_size,
				&sd->verneed_info);
    }

  // Read the SHT_DYNAMIC section to find whether this shared object
  // has a DT_SONAME tag.  This doesn't really have anything to do
  // with reading the symbols, but this is a convenient place to do
  // it.
  if (dynamic_shndx != -1U)
    this->set_soname(pshdrs, dynamic_shndx, strtab_shndx,
		     (sd->symbol_names == NULL
		      ? NULL
		      : sd->symbol_names->data()),
		     sd->symbol_names_size);
}

// Lay out the input sections for a dynamic object.  We don't want to
// include sections from a dynamic object, so all that we actually do
// here is check for .gnu.warning sections.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::do_layout(const General_options&,
					  Symbol_table* symtab,
					  Layout*,
					  Read_symbols_data* sd)
{
  const unsigned int shnum = this->shnum();
  if (shnum == 0)
    return;

  // Get the section headers.
  const unsigned char* pshdrs = sd->section_headers->data();

  // Get the section names.
  const unsigned char* pnamesu = sd->section_names->data();
  const char* pnames = reinterpret_cast<const char*>(pnamesu);

  // Skip the first, dummy, section.
  pshdrs += This::shdr_size;
  for (unsigned int i = 1; i < shnum; ++i, pshdrs += This::shdr_size)
    {
      typename This::Shdr shdr(pshdrs);

      if (shdr.get_sh_name() >= sd->section_names_size)
	{
	  fprintf(stderr,
		  _("%s: %s: bad section name offset for section %u: %lu\n"),
		  program_name, this->name().c_str(), i,
		  static_cast<unsigned long>(shdr.get_sh_name()));
	  gold_exit(false);
	}

      const char* name = pnames + shdr.get_sh_name();

      this->handle_gnu_warning_section(name, i, symtab);
    }

  delete sd->section_headers;
  sd->section_headers = NULL;
  delete sd->section_names;
  sd->section_names = NULL;
}

// Add an entry to the vector mapping version numbers to version
// strings.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::set_version_map(
    Version_map* version_map,
    unsigned int ndx,
    const char* name) const
{
  assert(ndx < version_map->size());
  if ((*version_map)[ndx] != NULL)
    {
      fprintf(stderr, _("%s: %s: duplicate definition for version %u\n"),
	      program_name, this->name().c_str(), ndx);
      gold_exit(false);
    }
  (*version_map)[ndx] = name;
}

// Create a vector mapping version numbers to version strings.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::make_version_map(
    Read_symbols_data* sd,
    Version_map* version_map) const
{
  if (sd->verdef == NULL && sd->verneed == NULL)
    return;

  // First find the largest version index.
  unsigned int maxver = 0;

  if (sd->verdef != NULL)
    {
      const unsigned char* pverdef = sd->verdef->data();
      off_t verdef_size = sd->verdef_size;
      const unsigned int count = sd->verdef_info;

      const unsigned char* p = pverdef;
      for (unsigned int i = 0; i < count; ++i)
	{
	  elfcpp::Verdef<size, big_endian> verdef(p);

	  const unsigned int vd_ndx = verdef.get_vd_ndx();

	  // The GNU linker clears the VERSYM_HIDDEN bit.  I'm not
	  // sure why.

	  if (vd_ndx > maxver)
	    maxver = vd_ndx;

	  const unsigned int vd_next = verdef.get_vd_next();
	  if ((p - pverdef) + vd_next >= verdef_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verdef vd_next field out of range: %u\n"),
		      program_name, this->name().c_str(), vd_next);
	      gold_exit(false);
	    }

	  p += vd_next;
	}
    }

  if (sd->verneed != NULL)
    {
      const unsigned char* pverneed = sd->verneed->data();
      off_t verneed_size = sd->verneed_size;
      const unsigned int count = sd->verneed_info;

      const unsigned char* p = pverneed;
      for (unsigned int i = 0; i < count; ++i)
	{
	  elfcpp::Verneed<size, big_endian> verneed(p);

	  const unsigned int vn_aux = verneed.get_vn_aux();
	  if ((p - pverneed) + vn_aux >= verneed_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verneed vn_aux field out of range: %u\n"),
		      program_name, this->name().c_str(), vn_aux);
	      gold_exit(false);
	    }

	  const unsigned int vn_cnt = verneed.get_vn_cnt();
	  const unsigned char* pvna = p + vn_aux;
	  for (unsigned int j = 0; j < vn_cnt; ++j)
	    {
	      elfcpp::Vernaux<size, big_endian> vernaux(pvna);

	      const unsigned int vna_other = vernaux.get_vna_other();
	      if (vna_other > maxver)
		maxver = vna_other;

	      const unsigned int vna_next = vernaux.get_vna_next();
	      if ((pvna - pverneed) + vna_next >= verneed_size)
		{
		  fprintf(stderr,
			  _("%s: %s: verneed vna_next field "
			    "out of range: %u\n"),
			  program_name, this->name().c_str(), vna_next);
		  gold_exit(false);
		}

	      pvna += vna_next;
	    }

	  const unsigned int vn_next = verneed.get_vn_next();
	  if ((p - pverneed) + vn_next >= verneed_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verneed vn_next field out of range: %u\n"),
		      program_name, this->name().c_str(), vn_next);
	      gold_exit(false);
	    }

	  p += vn_next;
	}
    }

  // Now MAXVER is the largest version index we have seen.

  version_map->resize(maxver + 1);

  const char* names = reinterpret_cast<const char*>(sd->symbol_names->data());
  off_t names_size = sd->symbol_names_size;

  if (sd->verdef != NULL)
    {
      const unsigned char* pverdef = sd->verdef->data();
      off_t verdef_size = sd->verdef_size;
      const unsigned int count = sd->verdef_info;

      const unsigned char* p = pverdef;
      for (unsigned int i = 0; i < count; ++i)
	{
	  elfcpp::Verdef<size, big_endian> verdef(p);

	  const unsigned int vd_cnt = verdef.get_vd_cnt();
	  if (vd_cnt < 1)
	    {
	      fprintf(stderr, _("%s: %s: verdef vd_cnt field too small: %u\n"),
		      program_name, this->name().c_str(), vd_cnt);
	      gold_exit(false);
	    }

	  const unsigned int vd_aux = verdef.get_vd_aux();
	  if ((p - pverdef) + vd_aux >= verdef_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verdef vd_aux field out of range: %u\n"),
		      program_name, this->name().c_str(), vd_aux);
	      gold_exit(false);
	    }

	  const unsigned char* pvda = p + vd_aux;
	  elfcpp::Verdaux<size, big_endian> verdaux(pvda);

	  const unsigned int vda_name = verdaux.get_vda_name();
	  if (vda_name >= names_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verdaux vda_name field out of range: %u\n"),
		      program_name, this->name().c_str(), vda_name);
	      gold_exit(false);
	    }

	  this->set_version_map(version_map, verdef.get_vd_ndx(),
				names + vda_name);

	  const unsigned int vd_next = verdef.get_vd_next();
	  if ((p - pverdef) + vd_next >= verdef_size)
	    {
	      fprintf(stderr,
		      _("%s: %s: verdef vd_next field out of range: %u\n"),
		      program_name, this->name().c_str(), vd_next);
	      gold_exit(false);
	    }

	  p += vd_next;
	}
    }

  if (sd->verneed != NULL)
    {
      const unsigned char* pverneed = sd->verneed->data();
      const unsigned int count = sd->verneed_info;

      const unsigned char* p = pverneed;
      for (unsigned int i = 0; i < count; ++i)
	{
	  elfcpp::Verneed<size, big_endian> verneed(p);

	  const unsigned int vn_aux = verneed.get_vn_aux();
	  const unsigned int vn_cnt = verneed.get_vn_cnt();
	  const unsigned char* pvna = p + vn_aux;
	  for (unsigned int j = 0; j < vn_cnt; ++j)
	    {
	      elfcpp::Vernaux<size, big_endian> vernaux(pvna);

	      const unsigned int vna_name = vernaux.get_vna_name();
	      if (vna_name >= names_size)
		{
		  fprintf(stderr,
			  _("%s: %s: vernaux vna_name field "
			    "out of range: %u\n"),
			  program_name, this->name().c_str(), vna_name);
		  gold_exit(false);
		}

	      this->set_version_map(version_map, vernaux.get_vna_other(),
				    names + vna_name);

	      pvna += vernaux.get_vna_next();
	    }

	  p += verneed.get_vn_next();
	}
    }
}

// Add the dynamic symbols to the symbol table.

template<int size, bool big_endian>
void
Sized_dynobj<size, big_endian>::do_add_symbols(Symbol_table* symtab,
					       Read_symbols_data* sd)
{
  if (sd->symbols == NULL)
    {
      assert(sd->symbol_names == NULL);
      assert(sd->versym == NULL && sd->verdef == NULL && sd->verneed == NULL);
      return;
    }

  const int sym_size = This::sym_size;
  const size_t symcount = sd->symbols_size / sym_size;
  if (symcount * sym_size != sd->symbols_size)
    {
      fprintf(stderr,
	      _("%s: %s: size of dynamic symbols is not "
		"multiple of symbol size\n"),
	      program_name, this->name().c_str());
      gold_exit(false);
    }

  Version_map version_map;
  this->make_version_map(sd, &version_map);

  const char* sym_names =
    reinterpret_cast<const char*>(sd->symbol_names->data());
  symtab->add_from_dynobj(this, sd->symbols->data(), symcount,
			  sym_names, sd->symbol_names_size,
			  (sd->versym == NULL
			   ? NULL
			   : sd->versym->data()),
			  sd->versym_size,
			  &version_map);

  delete sd->symbols;
  sd->symbols = NULL;
  delete sd->symbol_names;
  sd->symbol_names = NULL;
  if (sd->versym != NULL)
    {
      delete sd->versym;
      sd->versym = NULL;
    }
  if (sd->verdef != NULL)
    {
      delete sd->verdef;
      sd->verdef = NULL;
    }
  if (sd->verneed != NULL)
    {
      delete sd->verneed;
      sd->verneed = NULL;
    }
}

// Instantiate the templates we need.  We could use the configure
// script to restrict this to only the ones for implemented targets.

template
class Sized_dynobj<32, false>;

template
class Sized_dynobj<32, true>;

template
class Sized_dynobj<64, false>;

template
class Sized_dynobj<64, true>;

} // End namespace gold.
