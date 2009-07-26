/* External interface for checkpoint module.
   Copyright (C) 2009
   Free Software Foundation, Inc.

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

#if !defined (CHECKPOINT_H)
#define CHECKPOINT_H 1

/*
 * The data structure for the checkpoint list.
 */
struct checkpoint_info 
{
  int checkpoint_id;
  void *client_data;
  struct checkpoint_info *next;
};

/*
 * Public functions for managing the checkpoint list.
 */

/* Insert a checkpoint into the list.  */
extern struct checkpoint_info *checkpoint_insert (void *client_data);

/* Unlink a checkpoint from the list.  */
extern void checkpoint_unlink (struct checkpoint_info *cp);

/* Find a checkpoint (by id) in the list.  */
extern struct checkpoint_info *checkpoint_find_id (int id);

/* Traverse the checkpoint list.  */
extern struct checkpoint_info *checkpoint_first (void);
extern struct checkpoint_info *checkpoint_next (void);

extern void checkpoint_init (void);

#endif	/* CHECKPOINT_H */
