/* Declarations of BFD back end for traditional Unix core files 
   Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.  */

#include "ansidecl.h"

/* forward declaration */
PROTO (bfd_target *, trad_unix_core_file_p, (bfd *abfd));
PROTO (char *, 	     trad_unix_core_file_failing_command, (bfd *abfd));
PROTO (int,	     trad_unix_core_file_failing_signal, (bfd *abfd));
PROTO (boolean,      trad_unix_core_file_matches_executable_p,
			 (bfd *core_bfd, bfd *exec_bfd));
