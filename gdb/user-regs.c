/* User visible, per-frame registers, for GDB, the GNU debugger.

   Copyright 2002 Free Software Foundation, Inc.

   Contributed by Red Hat.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "user-regs.h"
#include "gdbtypes.h"
#include "gdb_string.h"
#include "gdb_assert.h"
#include "frame.h"

/* A table of user registers.

   User registers have regnum's that live above of the range [0
   .. NUM_REGS + NUM_PSEUDO_REGS) (which is controlled by the target).
   The target should never see a user register's regnum value.

   Always append, never delete.  By doing this, the relative regnum
   (offset from NUM_REGS + NUM_PSEUDO_REGS) assigned to each user
   register never changes.  */

struct user_reg
{
  const char *name;
  struct value *(*read) (struct frame_info * frame);
};

struct user_regs
{
  struct user_reg *user;
  int nr;
};

static void
append_user_reg (struct user_regs *regs,
		    const char *name, user_reg_read_ftype *read)
{
  regs->nr++;
  regs->user = xrealloc (regs->user,
			    regs->nr * sizeof (struct user_reg));
  regs->user[regs->nr - 1].name = name;
  regs->user[regs->nr - 1].read = read;
}

/* An array of the builtin user registers.  */

static struct user_regs builtin_user_regs;

void
user_reg_add_builtin (const char *name, user_reg_read_ftype *read)
{
  append_user_reg (&builtin_user_regs, name, read);
}

/* Per-architecture user registers.  Start with the builtin user
   registers and then, again, append.  */

static struct gdbarch_data *user_regs_data;

static void *
user_regs_init (struct gdbarch *gdbarch)
{
  int i;
  struct user_regs *regs = XMALLOC (struct user_regs);
  memset (regs, 0, sizeof (struct user_regs));
  for (i = 0; i < builtin_user_regs.nr; i++)
    append_user_reg (regs, builtin_user_regs.user[i].name,
		     builtin_user_regs.user[i].read);
  return regs;
}

static void
user_regs_free (struct gdbarch *gdbarch, void *data)
{
  struct user_regs *regs = data;
  xfree (regs->user);
  xfree (regs);
}

void
user_reg_add (struct gdbarch *gdbarch, const char *name,
		 user_reg_read_ftype *read)
{
  struct user_regs *regs = gdbarch_data (gdbarch, user_regs_data);
  if (regs == NULL)
    {
      /* ULGH, called during architecture initialization.  Patch
         things up.  */
      regs = user_regs_init (gdbarch);
      set_gdbarch_data (gdbarch, user_regs_data, regs);
    }
  append_user_reg (regs, name, read);
}

int
user_reg_map_name_to_regnum (struct gdbarch *gdbarch, const char *name,
			     int len)
{
  /* Make life easy, set the len to something reasonable.  */
  if (len < 0)
    len = strlen (name);

  /* Search register name space first - always let an architecture
     specific register override the user registers.  */
  {
    int i;
    int maxregs = (gdbarch_num_regs (gdbarch)
		   + gdbarch_num_pseudo_regs (gdbarch));
    for (i = 0; i < maxregs; i++)
      {
	const char *regname = gdbarch_register_name (gdbarch, i);
	if (regname != NULL && len == strlen (regname)
	    && strncmp (regname, name, len) == 0)
	  {
	    return i;
	  }
      }
  }

  /* Search the user name space.  */
  {
    struct user_regs *regs = gdbarch_data (gdbarch, user_regs_data);
    int reg;
    for (reg = 0; reg < regs->nr; reg++)
      {
	if ((len < 0 && strcmp (regs->user[reg].name, name))
	    || (len == strlen (regs->user[reg].name)
		&& strncmp (regs->user[reg].name, name, len) == 0))
	  return NUM_REGS + NUM_PSEUDO_REGS + reg;
      }
  }

  return -1;
}

const char *
user_reg_map_regnum_to_name (struct gdbarch *gdbarch, int regnum)
{
  int maxregs = (gdbarch_num_regs (gdbarch)
		 + gdbarch_num_pseudo_regs (gdbarch));
  struct user_regs *regs = gdbarch_data (gdbarch, user_regs_data);
  if (regnum < 0)
    return NULL;
  if (regnum < maxregs)
    return gdbarch_register_name (gdbarch, regnum);
  if (regnum < (maxregs + regs->nr))
    return regs->user[regnum - maxregs].name;
  return NULL;
}

struct value *
value_of_user_reg (int regnum, struct frame_info *frame)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);
  struct user_regs *regs = gdbarch_data (gdbarch, user_regs_data);
  int reg = regnum - (NUM_REGS + NUM_PSEUDO_REGS);
  gdb_assert (reg >= 0 && reg < regs->nr);
  return regs->user[reg].read (frame);
}

extern initialize_file_ftype _initialize_user_regs; /* -Wmissing-prototypes */

void
_initialize_user_regs (void)
{
  user_regs_data = register_gdbarch_data (user_regs_init, user_regs_free);
}
