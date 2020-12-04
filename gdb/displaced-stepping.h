/* Displaced stepping related things.

   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef DISPLACED_STEPPING_H
#define DISPLACED_STEPPING_H

enum displaced_step_prepare_status
{
  /* A displaced stepping buffer was successfully allocated and prepared.  */
  DISPLACED_STEP_PREPARE_STATUS_OK,

  /* This particular instruction can't be displaced stepped, GDB should fall
     back on in-line stepping.  */
  DISPLACED_STEP_PREPARE_STATUS_CANT,

  /* Not enough resources are available at this time, try again later.  */
  DISPLACED_STEP_PREPARE_STATUS_UNAVAILABLE,
};

enum displaced_step_finish_status
{
  /* Either the instruction was stepped and fixed up, or the specified thread
     wasn't executing a displaced step (in which case there's nothing to
     finish). */
  DISPLACED_STEP_FINISH_STATUS_OK,

  /* The thread started a displaced step, but didn't complete it.  */
  DISPLACED_STEP_FINISH_STATUS_NOT_EXECUTED,
};

#endif /* DISPLACED_STEPPING_H */
