/* Helper routines for parsing XML using Expat.

   Copyright (C) 2006
   Free Software Foundation, Inc.

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


#ifndef XML_SUPPORT_H
#define XML_SUPPORT_H

#include <expat.h>

/* Helper functions for parsing XML documents.  See xml-support.c
   for more information about these functions.  */

const XML_Char *xml_get_required_attribute (const XML_Char **attrs,
					    const XML_Char *attr);

ULONGEST xml_get_integer_attribute (const XML_Char **attrs,
				    const XML_Char *attr);

int xml_get_enum_value (const XML_Char **attrs,
			const XML_Char *attr,
			const XML_Char **xml_names,
			int *values);

void make_cleanup_free_xml_parser (XML_Parser parser);

#endif
