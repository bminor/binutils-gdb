/* Target description definitions for remote server for GDB.
   Copyright (C) 2012-2018 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef TDESC_H
#define TDESC_H

#include "arch/tdesc.h"

#include "regdef.h"
#include <vector>

struct tdesc_feature
{};

/* A target description.  Inherit from tdesc_feature so that target_desc
   can be used as tdesc_feature.  */

struct target_desc : tdesc_feature
{
  /* A vector of elements of register definitions that
     describe the inferior's register set.  */
  std::vector<struct reg *> reg_defs;

  /* The register cache size, in bytes.  */
  int registers_size;

#ifndef IN_PROCESS_AGENT
  /* An array of register names.  These are the "expedite" registers:
     registers whose values are sent along with stop replies.  */
  const char **expedite_regs = NULL;

  /* Defines what to return when looking for the "target.xml" file in
     response to qXfer:features:read.  Its contents can either be
     verbatim XML code (prefixed with a '@') or else the name of the
     actual XML file to be used in place of "target.xml".

     It can be NULL, then, its content is got from the following three
     fields features, arch, and osabi in tdesc_get_features_xml.  */
  const char *xmltarget = NULL;

  /* XML features in this target description.  */
  VEC (char_ptr) *features = NULL;

  /* The value of <architecture> element in the XML, replying GDB.  */
  const char *arch = NULL;

  /* The value of <osabi> element in the XML, replying GDB.  */
  const char *osabi = NULL;

public:
  target_desc ()
    : registers_size (0)
  {}

  ~target_desc ()
  {
    int i;

    for (reg *reg : reg_defs)
      xfree (reg);

    xfree ((char *) arch);
    xfree ((char *) osabi);

    char *f;

    for (i = 0; VEC_iterate (char_ptr, features, i, f); i++)
      xfree (f);
    VEC_free (char_ptr, features);
  }

  bool operator== (const target_desc &other) const
  {
    if (reg_defs.size () != other.reg_defs.size ())
      return false;

    for (int i = 0; i < reg_defs.size (); ++i)
      {
	struct reg *reg = reg_defs[i];
	struct reg *reg2 = other.reg_defs[i];

	if (reg != reg2 && *reg != *reg2)
	  return false;
      }

    /* Compare expedite_regs.  */
    int i = 0;
    for (; expedite_regs[i] != NULL; i++)
      {
	if (strcmp (expedite_regs[i], other.expedite_regs[i]) != 0)
	  return false;
      }
    if (other.expedite_regs[i] != NULL)
      return false;

    return true;
  }

  bool operator!= (const target_desc &other) const
  {
    return !(*this == other);
  }
#endif
};

/* Copy target description SRC to DEST.  */

void copy_target_description (struct target_desc *dest,
			      const struct target_desc *src);

/* Initialize TDESC.  */

void init_target_desc (struct target_desc *tdesc);

/* Return the current inferior's target description.  Never returns
   NULL.  */

const struct target_desc *current_target_desc (void);

#ifndef IN_PROCESS_AGENT
const char *tdesc_get_features_xml (struct target_desc *tdesc);
#endif

#endif /* TDESC_H */
