/* Target description support for GDB.

   Copyright (C) 2006, 2007 Free Software Foundation, Inc.

   Contributed by CodeSourcery.

   This file is part of GDB.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#ifndef TARGET_DESCRIPTIONS_H
#define TARGET_DESCRIPTIONS_H 1

struct target_desc;

/* Fetch the current target's description, and switch the current
   architecture to one which incorporates that description.  */

void target_find_description (void);

/* Discard any description fetched from the current target, and switch
   the current architecture to one with no target description.  */

void target_clear_description (void);

/* Return the global current target description.  This should only be
   used by gdbarch initialization code; most access should be through
   an existing gdbarch.  */

const struct target_desc *target_current_description (void);

/* Accessors for target descriptions.  */

/* Return the BFD architecture associated with this target
   description, or NULL if no architecture was specified.  */

const struct bfd_arch_info *tdesc_architecture
  (const struct target_desc *);

/* Return the string value of a property named KEY, or NULL if the
   property was not specified.  */

const char *tdesc_property (const struct target_desc *,
			    const char *key);

/* Methods for constructing a target description.  */

struct target_desc *allocate_target_description (void);
struct cleanup *make_cleanup_free_target_description (struct target_desc *);
void set_tdesc_architecture (struct target_desc *,
			     const struct bfd_arch_info *);

void set_tdesc_property (struct target_desc *,
			 const char *key, const char *value);

#endif /* TARGET_DESCRIPTIONS_H */
