/* Support for dumping and reloading various pieces of GDB's internal state.
   Copyright 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support, using pieces from other GDB modules.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This file provides definitions used for reading and writing gdb
   state files.  State files have a fairly simple form which is intended
   to be easily extensible.  See state.c for further documentation. */

#if !defined (_STATE_H)
#define _STATE_H

/* State file-header */

typedef struct {
  unsigned char sf_mag0;	/* Magic number byte 0 */
  unsigned char sf_mag1;	/* Magic number byte 1 */
  unsigned char sf_mag2;	/* Magic number byte 2 */
  unsigned char sf_mag3;	/* Magic number byte 3 */
  unsigned long sf_ftoff;	/* File offset to start of form-tree */
  unsigned long sf_ftsize;	/* Size of the form-tree, in bytes */
} sf_hdr;

  
#define SF_MAG0	'g'		/* Magic number byte 0 value */
#define SF_MAG1	'd'		/* Magic number byte 1 value */
#define SF_MAG2	'b'		/* Magic number byte 2 value */
#define SF_MAG3	'\000'		/* Magic number byte 3 value */

#define SF_GOOD_MAGIC(asfd) ((asfd) -> hdr.sf_mag0 == SF_MAG0 && \
			     (asfd) -> hdr.sf_mag1 == SF_MAG1 && \
			     (asfd) -> hdr.sf_mag2 == SF_MAG2 && \
			     (asfd) -> hdr.sf_mag3 == SF_MAG3)

/* The internal form-tree is formed from nodes that contain pointers
   to the first sibling, the first child, a backpointer to the parent,
   and a pointer to the actual data for the node.  This allows all
   tree nodes to have the same sized structure, but support variable
   numbers of child nodes per parent node.  The backpointer for the
   parent is required for simplier tree walks. */

struct formnode
{
  struct formnode *sibling;	/* Pointer to first sibling */
  struct formnode *child;	/* Pointer to first child */
  struct formnode *parent;	/* Backpointer to parent */
  char *nodedata;		/* Pointer to the nodes data */
  unsigned long treesize;	/* Size of subtree rooted here */
};

/* A state file descriptor is defined by the following structure. */

typedef struct
{
  char *filename;		/* Full pathname of the state file */
  FILE *fp;			/* Open file pointer for the state file */
  sf_hdr hdr;			/* Copy of the state file-header */
  char *formtree;		/* Pointer to in-memory copy of form-tree */
} sfd;

extern sfd *
sfd_fopen PARAMS ((char *, char *));

extern void
sfd_fclose PARAMS ((sfd *));

extern struct objfile *
objfile_from_statefile PARAMS ((sfd *));

#endif	/* !defined (_STATE_H) */
