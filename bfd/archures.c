/* BFD library support routines for architectures.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Hacked by John Gilmore of Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*doc*
@section Architectures
BFD's idea of an architecture is implimented in @code{archures.c}. BFD
keeps two atoms in a BFD describing the architecture of the data
attached to the BFD, the @code{enum bfd_architecture arch} field and
the @code{unsigned long machine} field.
*/

/*proto* bfd_architecture
This enum gives the object file's CPU
architecture, in a global sense.  E.g. what processor family does it
belong to?  There is another field, which indicates what processor
within the family is in use.  The machine gives a number which
distingushes different versions of the architecture, containing for
example 2 and 3 for Intel i960 KA and i960 KB, and 68020 and 68030 for
Motorola 68020 and 68030.

*+
enum bfd_architecture 
{
  bfd_arch_unknown,   {* File arch not known *}
  bfd_arch_obscure,   {* Arch known, not one of these *}
  bfd_arch_m68k,      {* Motorola 68xxx *}
  bfd_arch_vax,       {* DEC Vax *}   
  bfd_arch_i960,      {* Intel 960 *}
    {* The order of the following is important.
       lower number indicates a machine type that 
       only accepts a subset of the instructions
       available to machines with higher numbers.
       The exception is the "ca", which is
       incompatible with all other machines except 
       "core". *}

#define bfd_mach_i960_core      1
#define bfd_mach_i960_ka_sa     2
#define bfd_mach_i960_kb_sb     3
#define bfd_mach_i960_mc        4
#define bfd_mach_i960_xa        5
#define bfd_mach_i960_ca        6

  bfd_arch_a29k,      {* AMD 29000 *}
  bfd_arch_sparc,     {* SPARC *}
  bfd_arch_mips,      {* MIPS Rxxxx *}
  bfd_arch_i386,      {* Intel 386 *}
  bfd_arch_ns32k,     {* National Semiconductor 32xxx *}
  bfd_arch_tahoe,     {* CCI/Harris Tahoe *}
  bfd_arch_i860,      {* Intel 860 *}
  bfd_arch_romp,      {* IBM ROMP RS/6000 *}
  bfd_arch_alliant,   {* Alliant *}
  bfd_arch_convex,    {* Convex *}
  bfd_arch_m88k,      {* Motorola 88xxx *}
  bfd_arch_pyramid,   {* Pyramid Technology *}
  bfd_arch_h8_300,    {* Hitachi H8/300 *}
  bfd_arch_last
  };
*-

stuff

*/



/* $Id$ */

#include <sysdep.h>
#include "bfd.h"

static char *prt_num_mach ();
static boolean scan_num_mach ();
static char *prt_960_mach ();
static boolean scan_960_mach ();

struct arch_print {
        enum bfd_architecture arch;
        char *astr;
        char *(*mach_print)();
        boolean (*mach_scan)();
} arch_print[] = {

        {bfd_arch_unknown,      "unknown",      prt_num_mach,   scan_num_mach},
        {bfd_arch_obscure,      "obscure",      prt_num_mach,   scan_num_mach},
        {bfd_arch_m68k,         "m68k",         prt_num_mach,   scan_num_mach},
        {bfd_arch_vax,          "vax",          prt_num_mach,   scan_num_mach},
        {bfd_arch_i960,         "i960",         prt_960_mach,   scan_960_mach},
        {bfd_arch_a29k,         "a29k",         prt_num_mach,   scan_num_mach},
        {bfd_arch_sparc,        "sparc",        prt_num_mach,   scan_num_mach},
        {bfd_arch_mips,         "mips",         prt_num_mach,   scan_num_mach},
        {bfd_arch_i386,         "i386",         prt_num_mach,   scan_num_mach},
        {bfd_arch_ns32k,        "ns32k",        prt_num_mach,   scan_num_mach},
        {bfd_arch_tahoe,        "tahoe",        prt_num_mach,   scan_num_mach},
        {bfd_arch_i860,         "i860",         prt_num_mach,   scan_num_mach},
        {bfd_arch_romp,         "romp",         prt_num_mach,   scan_num_mach},
        {bfd_arch_alliant,      "alliant",      prt_num_mach,   scan_num_mach},
        {bfd_arch_convex,       "convex",       prt_num_mach,   scan_num_mach},
        {bfd_arch_m88k,         "m88k",         prt_num_mach,   scan_num_mach},
        {bfd_arch_pyramid,      "pyramid",      prt_num_mach,   scan_num_mach},
        {bfd_arch_h8_300,       "H8/300",       prt_num_mach,   scan_num_mach},
        {bfd_arch_unknown,      (char *)0,      prt_num_mach,   scan_num_mach},
};

/*proto* bfd_prinable_arch_mach
Return a printable string representing the architecture and machine
type. The result is only good until the next call to
@code{bfd_printable_arch_mach}.  
*; PROTO(CONST char *,bfd_printable_arch_mach,
    (enum bfd_architecture arch, unsigned long machine));
*/

CONST char *
DEFUN(bfd_printable_arch_mach,(arch, machine),
      enum bfd_architecture arch AND
      unsigned long machine)
{
  struct arch_print *ap;

  for (ap = arch_print; ap->astr; ap++) {
    if (ap->arch == arch) {
      if (machine == 0)
        return ap->astr;
      return (*ap->mach_print)(ap, machine);
    }
  }
  return "UNKNOWN!";
}

static char *
prt_num_mach (ap, machine)
     struct arch_print *ap;
     unsigned long machine;
{
  static char result[20];

  sprintf(result, "%s:%ld", ap->astr, (long) machine);
  return result;
}

/*proto*
*i bfd_scan_arch_mach
Scan a string and attempt to turn it into an archive and machine type combination.  
*; PROTO(boolean, bfd_scan_arch_mach,
    (CONST char *, enum bfd_architecture *, unsigned long *));
*/

boolean
DEFUN(bfd_scan_arch_mach,(string, archp, machinep),
      CONST char *string AND
      enum bfd_architecture *archp AND
      unsigned long *machinep)
{
  struct arch_print *ap;
  int len;

  /* First look for an architecture, possibly followed by machtype. */
  for (ap = arch_print; ap->astr; ap++) {
    if (ap->astr[0] != string[0])
      continue;
    len = strlen (ap->astr);
    if (!strncmp (ap->astr, string, len)) {
      /* We found the architecture, now see about the machine type */
      if (archp)
        *archp = ap->arch;
      if (string[len] != '\0') {
        if (ap->mach_scan (string+len, ap, archp, machinep, 1))
          return true;
      }
      if (machinep)
        *machinep = 0;
      return true;
    }
  }

  /* Couldn't find an architecture -- try for just a machine type */
  for (ap = arch_print; ap->astr; ap++) {
    if (ap->mach_scan (string, ap, archp, machinep, 0))
      return true;
  }

  return false;
}

static boolean
scan_num_mach (string, ap, archp, machinep, archspec)
     char *string;
     struct arch_print *ap;
     enum bfd_architecture *archp;
     unsigned long *machinep;
     int archspec;
{
  enum bfd_architecture arch;
  unsigned long machine;
  char achar;

  if (archspec) {

    /* Architecture already specified, now go for machine type.  */
    if (string[0] != ':')
      return false;
    /* We'll take any valid number that occupies the entire string */
    if (1 != sscanf (string+1, "%lu%c", &machine, &achar))
      return false;
    arch = ap->arch;

  } else {

    /* We couldn't identify an architecture prefix.  Perhaps the entire
       thing is a machine type.  Be a lot picker.  */
    if (1 != sscanf (string, "%lu%c", &machine, &achar))
      return false;
    switch (machine) {
    case 68010:
    case 68020:
    case 68030:
    case 68040:
    case 68332:
    case 68050:         arch = bfd_arch_m68k; break;
    case 68000:         arch = bfd_arch_m68k; machine = 0; break;

    case 80960:
    case 960:           arch = bfd_arch_i960; machine = 0; break;

    case 386:
    case 80386:         arch = bfd_arch_i386; machine = 0; break;
    case 486:           arch = bfd_arch_i386; break;

    case 29000:         arch = bfd_arch_a29k; machine = 0; break;

    case 32016:
    case 32032:
    case 32132:
    case 32232:
    case 32332:
    case 32432:
    case 32532:         arch = bfd_arch_ns32k; break;
    case 32000:         arch = bfd_arch_ns32k; machine = 0; break;

    case 860:
    case 80860:         arch = bfd_arch_i860; machine = 0; break;

    default:            return false;
    }
  }

  if (archp)
    *archp = arch;
  if (machinep)
    *machinep = machine;
  return true;
}

/* Intel 960 machine variants.  */

static char *
prt_960_mach (ap, machine)
     struct arch_print *ap;
     unsigned long machine;
{
  static char result[20];
  char *str;

  switch (machine) {
  case bfd_mach_i960_core:      str = "core"; break;
  case bfd_mach_i960_kb_sb:     str = "kb"; break;
  case bfd_mach_i960_mc:        str = "mc"; break;
  case bfd_mach_i960_xa:        str = "xa"; break;
  case bfd_mach_i960_ca:        str = "ca"; break;
  case bfd_mach_i960_ka_sa:     str = "ka"; break;
  default:
        return prt_num_mach (ap, machine);
  }
  sprintf (result, "%s:%s", ap->astr, str);
  return result;
}

static boolean
scan_960_mach (string, ap, archp, machinep, archspec)
     char *string;
     struct arch_print *ap;
     enum bfd_architecture *archp;
     unsigned long *machinep;
     int archspec;
{
  unsigned long machine;

  if (!archspec)
    return false;
  if (string[0] != ':')
    return false;
  string++;
  if (string[0] == '\0')
    return false;
  if (string[0] == 'c' && string[1] == 'o' && string[2] == 'r' &&
      string[3] == 'e' && string[4] == '\0')
    machine = bfd_mach_i960_core;
  else if (string[1] == '\0' || string[2] != '\0')      /* rest are 2-char */
    return false;
  else if (string[0] == 'k' && string[1] == 'b')
    machine = bfd_mach_i960_kb_sb;
  else if (string[0] == 's' && string[1] == 'b')
    machine = bfd_mach_i960_kb_sb;
  else if (string[0] == 'm' && string[1] == 'c')
    machine = bfd_mach_i960_mc;
  else if (string[0] == 'x' && string[1] == 'a')
    machine = bfd_mach_i960_xa;
  else if (string[0] == 'c' && string[1] == 'a')
    machine = bfd_mach_i960_ca;
  else if (string[0] == 'k' && string[1] == 'a')
    machine = bfd_mach_i960_ka_sa;
  else if (string[0] == 's' && string[1] == 'a')
    machine = bfd_mach_i960_ka_sa;
  else
    return false;

  if (archp)
    *archp = ap->arch;
  if (machinep)
    *machinep = machine;
  return true;
}



/*proto*
*i bfd_arch_compatible
This routine is used to determine whether two BFDs' architectures and machine types are
compatible.  It calculates the lowest common denominator between the
two architectures and machine types implied by the BFDs and sets the
objects pointed at by @var{archp} and @var{machine} if non NULL. 

This routine returns @code{true} if the BFDs are of compatible type,
otherwise @code{false}.
*; PROTO(boolean, bfd_arch_compatible,
     (bfd *abfd,
     bfd *bbfd,
     enum bfd_architecture *archp,
     unsigned long *machinep));
*-*/

boolean
DEFUN(bfd_arch_compatible,(abfd, bbfd, archp, machinep),
    bfd *abfd AND
    bfd *bbfd AND
    enum bfd_architecture *archp AND
    unsigned long *machinep)
{
  enum bfd_architecture archa, archb;
  unsigned long macha, machb;
  int pick_a;

  archa = bfd_get_architecture (abfd);
  archb = bfd_get_architecture (bbfd);
  macha = bfd_get_machine (abfd);
  machb = bfd_get_machine (bbfd);

  if (archb == bfd_arch_unknown)
        pick_a = 1;
  else if (archa == bfd_arch_unknown)
        pick_a = 0;
  else if (archa != archb)
        return false;                   /* Not compatible */
  else {
    /* Architectures are the same.  Check machine types.  */
    if (macha == machb)         /* Same machine type */
      pick_a = 1;
    else if (machb == 0)        /* B is default */
      pick_a = 1;
    else if (macha == 0)        /* A is default */
      pick_a = 0;
    else switch (archa) {
      /* If particular machine types of one architecture are not
         compatible with each other, this is the place to put those tests
         (returning false if incompatible).  */

      case bfd_arch_i960:
        /* The i960 has two distinct subspecies which may not interbreed:
           CORE CA          
           CORE KA KB MC
           Any architecture on the same line is compatible, the one on
           the right is the least restrictive.  */
        /* So, if either is a ca then the other must be a be core or ca */
        if (macha == bfd_mach_i960_ca) {
          if (machb != bfd_mach_i960_ca &&
              machb != bfd_mach_i960_core) {
            return false;
          }
          pick_a = 1;
        }
        else      if (machb == bfd_mach_i960_ca) {
          if (macha != bfd_mach_i960_ca &&
              macha != bfd_mach_i960_core) {
            return false;
          }
          pick_a = 0;
        }
        else {
          /* This must be from the bottom row, so take the higest */
          pick_a = (macha > machb);
        }
        break;

      /* For these chips, as far as we care, "lower" numbers are included
         by "higher" numbers, e.g. merge 68010 and 68020 into 68020,
         386 and 486 into 486, etc.  This will need to change
         if&when we care about things like 68332.  */
    case bfd_arch_m68k:
    case bfd_arch_ns32k:
    case bfd_arch_i386:
      pick_a = (macha > machb);
      break;
    
      /* By default, pick first file's type, for lack of something better.  */
    default:
      pick_a = 1;
    }
  }

  /* Set result based on our pick */
  if (!pick_a) {
    archa = archb;
    macha = machb;
  }
  if (archp)
    *archp = archa;
  if (machinep)
    *machinep = macha;

  return true;
}


/*proto* bfd_set_arch_mach
Set atch mach
*+
#define bfd_set_arch_mach(abfd, arch, mach) \
     BFD_SEND (abfd, _bfd_set_arch_mach,\
                    (abfd, arch, mach))
*-
*/


foo() { }



