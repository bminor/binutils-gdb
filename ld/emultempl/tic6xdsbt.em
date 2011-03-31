# This shell script emits a C file. -*- C -*-
#   Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.
#

# This file is sourced from elf32.em, and defines extra C6X DSBT specific
# features.
#
fragment <<EOF
#include "ldctor.h"
#include "elf32-tic6x.h"

static struct elf32_tic6x_params params =
{
  0, 64
};

static int
is_tic6x_target (void)
{
  extern const bfd_target bfd_elf32_tic6x_le_vec;
  extern const bfd_target bfd_elf32_tic6x_be_vec;

  return (link_info.output_bfd->xvec == &bfd_elf32_tic6x_le_vec
  	  || link_info.output_bfd->xvec == &bfd_elf32_tic6x_be_vec);
}

/* Pass params to backend.  */

static void
tic6x_after_open (void)
{
  if (is_tic6x_target ())
    {
      if (params.dsbt_index >= params.dsbt_size)
	{
	  einfo (_("%P%F: invalid --dsbt-index %d, outside DSBT size.\n"),
		 params.dsbt_index);
	}
      elf32_tic6x_setup (&link_info, &params);
    }

  gld${EMULATION_NAME}_after_open ();
}
EOF

# This code gets inserted into the generic elf32.sc linker script
# and allows us to define our own command line switches.
PARSE_AND_LIST_PROLOGUE='
#define OPTION_DSBT_INDEX		300
#define OPTION_DSBT_SIZE		301
'

PARSE_AND_LIST_LONGOPTS='
  {"dsbt-index", required_argument, NULL, OPTION_DSBT_INDEX},
  {"dsbt-size", required_argument, NULL, OPTION_DSBT_SIZE},
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("  --dsbt-index <index>\n"));
  fprintf (file, _("\t\t\tUse this as the DSBT index for the output object\n"));
  fprintf (file, _("  --dsbt-size <index>\n"));
  fprintf (file, _("\t\t\tUse this as the number of entries in the DSBT table\n"));
'

PARSE_AND_LIST_ARGS_CASES='
    case OPTION_DSBT_INDEX:
      {
	char *end;
	params.dsbt_index = strtol (optarg, &end, 0);
	if (*end == 0
	    && params.dsbt_index >= 0 && params.dsbt_index < 0x7fff)
	  break;
	einfo (_("%P%F: invalid --dsbt-index %s\n"), optarg);
      }
      break;
    case OPTION_DSBT_SIZE:
      {
	char *end;
	params.dsbt_size = strtol (optarg, &end, 0);
	if (*end == 0
	    && params.dsbt_size >= 0 && params.dsbt_size < 0x7fff)
	  break;
	einfo (_("%P%F: invalid --dsbt-size %s\n"), optarg);
      }
      break;
'

LDEMUL_AFTER_OPEN=tic6x_after_open
