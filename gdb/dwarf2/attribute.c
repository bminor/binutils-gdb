/* DWARF attributes

   Copyright (C) 1994-2020 Free Software Foundation, Inc.

   Adapted by Gary Funck (gary@intrepid.com), Intrepid Technology,
   Inc.  with support from Florida State University (under contract
   with the Ada Joint Program Office), and Silicon Graphics, Inc.
   Initial contribution by Brent Benson, Harris Computer Systems, Inc.,
   based on Fred Fish's (Cygnus Support) implementation of DWARF 1
   support.

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
#include "dwarf2/attribute.h"

/* See attribute.h.  */

CORE_ADDR
attr_value_as_address (struct attribute *attr)
{
  CORE_ADDR addr;

  if (attr->form != DW_FORM_addr && attr->form != DW_FORM_addrx
      && attr->form != DW_FORM_GNU_addr_index)
    {
      /* Aside from a few clearly defined exceptions, attributes that
	 contain an address must always be in DW_FORM_addr form.
	 Unfortunately, some compilers happen to be violating this
	 requirement by encoding addresses using other forms, such
	 as DW_FORM_data4 for example.  For those broken compilers,
	 we try to do our best, without any guarantee of success,
	 to interpret the address correctly.  It would also be nice
	 to generate a complaint, but that would require us to maintain
	 a list of legitimate cases where a non-address form is allowed,
	 as well as update callers to pass in at least the CU's DWARF
	 version.  This is more overhead than what we're willing to
	 expand for a pretty rare case.  */
      addr = DW_UNSND (attr);
    }
  else
    addr = DW_ADDR (attr);

  return addr;
}

/* See attribute.h.  */

int
attr_form_is_block (const struct attribute *attr)
{
  return (attr == NULL ? 0 :
      attr->form == DW_FORM_block1
      || attr->form == DW_FORM_block2
      || attr->form == DW_FORM_block4
      || attr->form == DW_FORM_block
      || attr->form == DW_FORM_exprloc);
}

/* See attribute.h.  */

int
attr_form_is_section_offset (const struct attribute *attr)
{
  return (attr->form == DW_FORM_data4
          || attr->form == DW_FORM_data8
	  || attr->form == DW_FORM_sec_offset);
}

/* See attribute.h.  */

int
attr_form_is_constant (const struct attribute *attr)
{
  switch (attr->form)
    {
    case DW_FORM_sdata:
    case DW_FORM_udata:
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
    case DW_FORM_implicit_const:
      return 1;
    default:
      return 0;
    }
}

/* DW_ADDR is always stored already as sect_offset; despite for the forms
   besides DW_FORM_ref_addr it is stored as cu_offset in the DWARF file.  */

int
attr_form_is_ref (const struct attribute *attr)
{
  switch (attr->form)
    {
    case DW_FORM_ref_addr:
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
    case DW_FORM_GNU_ref_alt:
      return 1;
    default:
      return 0;
    }
}
