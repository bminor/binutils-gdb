/* mri.h -- header file for MRI scripting functions
   Copyright 1993 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

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

#ifndef MRI_H
#define MRI_H

extern int symbol_truncate;

extern void mri_output_section PARAMS ((const char *name, etree_type *vma));
extern void mri_only_load PARAMS ((const char *name));
extern void mri_base PARAMS ((etree_type *exp));
extern void mri_load PARAMS ((const char *name));
extern void mri_order PARAMS ((const char *name));
extern void mri_alias PARAMS ((const char *want, const char *is, int isn));
extern void mri_name PARAMS ((const char *name));
extern void mri_format PARAMS ((const char *name));
extern void mri_public PARAMS ((const char *name, etree_type *exp));
extern void mri_align PARAMS ((const char *name, etree_type *exp));
extern void mri_alignmod PARAMS ((const char *name, etree_type *exp));
extern void mri_truncate PARAMS ((int exp));

#endif
