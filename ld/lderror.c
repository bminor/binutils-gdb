#include "bfd.h"
#include "sysdep.h"
#include "../bfd/seclet.h"
#include "ld.h"
#include "ldmisc.h"

#define MAX_ERRORS_IN_A_ROW 5

extern ld_config_type config;

extern bfd_error_vector_type bfd_error_vector;


/* BFD has failed to link something, give a better error message */

static void
ld_undefined_symbol (relent, seclet)
     CONST arelent *relent;
     CONST bfd_seclet_type *seclet;
{
  asymbol *s = *(relent->sym_ptr_ptr);
  static asymbol *error_symbol;
  static unsigned int error_count;
  if (seclet != (bfd_seclet_type *)NULL)
  {
    
    asection *section = seclet->u.indirect.section;
    bfd *abfd = section->owner;


    /* We remember the symbol, and never print more than
       a reasonable number of them in a row */
    if (s == error_symbol) {
	error_count++;
      }
    else {
	error_count = 0;
	error_symbol = s;
      }
    if (error_count < MAX_ERRORS_IN_A_ROW) {
	einfo("%X%C: undefined reference to `%T'\n",
	      abfd,section, seclet->u.indirect.symbols,
	      relent->address, s);
	config.make_executable = false;

      }
    else if (error_count == MAX_ERRORS_IN_A_ROW) {
	einfo("%C: more undefined references to `%T' follow\n",
	      abfd, section,
	      seclet->u.indirect.symbols, 
	      relent->address, s);
      }		    
    else {
	/* Don't print any more */
      }
  }
  else 
  {
    einfo("%Xundefined reference to %s\n", (*(relent->sym_ptr_ptr))->name);
  }
}
static void
ld_reloc_truncated (relent, seclet)
     CONST arelent *relent;
     bfd_seclet_type *seclet;
{
  asymbol *s = *(relent->sym_ptr_ptr);
  asection *section = seclet->u.indirect.section;
  bfd *abfd = section->owner;
  
  einfo("%X%C: relocation truncated to fit %R\n",
	abfd, section, seclet->u.indirect.symbols, relent->address, relent);

}
  

void
init_bfd_error_vector ()
{
  bfd_error_vector.undefined_symbol = ld_undefined_symbol;  
  bfd_error_vector.reloc_value_truncated = ld_reloc_truncated;
}
