/* Copyright (C) 2014-2015 Free Software Foundation, Inc.

   Contributed by Intel Corp. <markus.t.metzger@intel.com>

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

#include "common-defs.h"
#include "btrace-common.h"


/* See btrace-common.h.  */

const char *
btrace_format_string (enum btrace_format format)
{
  switch (format)
    {
    case BTRACE_FORMAT_NONE:
      return _("No or unknown format");

    case BTRACE_FORMAT_BTS:
      return _("Branch Trace Store");
    }

  internal_error (__FILE__, __LINE__, _("Unknown branch trace format"));
}

/* See btrace-common.h.  */

void
btrace_data_init (struct btrace_data *data)
{
  data->format = BTRACE_FORMAT_NONE;
}

/* See btrace-common.h.  */

void
btrace_data_fini (struct btrace_data *data)
{
  switch (data->format)
    {
    case BTRACE_FORMAT_NONE:
      /* Nothing to do.  */
      return;

    case BTRACE_FORMAT_BTS:
      VEC_free (btrace_block_s, data->variant.bts.blocks);
      return;
    }

  internal_error (__FILE__, __LINE__, _("Unkown branch trace format."));
}

/* See btrace-common.h.  */

int
btrace_data_empty (struct btrace_data *data)
{
  switch (data->format)
    {
    case BTRACE_FORMAT_NONE:
      return 1;

    case BTRACE_FORMAT_BTS:
      return VEC_empty (btrace_block_s, data->variant.bts.blocks);
    }

  internal_error (__FILE__, __LINE__, _("Unkown branch trace format."));
}
