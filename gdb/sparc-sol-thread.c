/* Copyright (C) 2012-2013 Free Software Foundation, Inc.

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

#include "defs.h"
#include <proc_service.h>

/* Get size of extra register set.  Currently a noop.  */

ps_err_e
ps_lgetxregsize (struct ps_prochandle *ph, lwpid_t lwpid, int *xregsize)
{
  return PS_OK;
}

/* Get extra register set.  Currently a noop.  */

ps_err_e
ps_lgetxregs (struct ps_prochandle *ph, lwpid_t lwpid, caddr_t xregset)
{
  return PS_OK;
}

/* Set extra register set.  Currently a noop.  */

ps_err_e
ps_lsetxregs (struct ps_prochandle *ph, lwpid_t lwpid, caddr_t xregset)
{
  return PS_OK;
}
