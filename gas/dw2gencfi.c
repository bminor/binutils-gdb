/* dw2gencfi.c - Support for generating Dwarf2 CFI information.
   Copyright 2003 Free Software Foundation, Inc.
   Contributed by Michal Ludvig <mludvig@suse.cz>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include <errno.h>
#include "as.h"
#include "dw2gencfi.h"

struct cie_entry
{
  unsigned long offset;
  size_t size;
  void *data;
  struct cie_entry *next;
};

struct cfi_data
{
  enum cfi_insn insn;
  long param[2];
  struct cfi_data *next;
};

struct cfi_info
{
  addressT start_address;
  addressT end_address;
  addressT last_address;
  const char *labelname;
  struct cfi_data *data;
  struct cfi_info *next;
};

/* Current open CFI entry.  */
static struct cfi_info *cfi_info;

/* List of CIEs so that they could be reused.  */
static struct cie_entry *cie_root;

/* Current target config.  */
static struct cfi_config current_config;

/* This is the main entry point to the CFI machinery.  */
static void dot_cfi (int arg);

const pseudo_typeS cfi_pseudo_table[] =
  {
    { "cfi_verbose", dot_cfi, CFI_verbose },
    { "cfi_startproc", dot_cfi, CFI_startproc },
    { "cfi_endproc", dot_cfi, CFI_endproc },
    { "cfi_def_cfa", dot_cfi, CFA_def_cfa },
    { "cfi_def_cfa_register", dot_cfi, CFA_def_cfa_register },
    { "cfi_def_cfa_offset", dot_cfi, CFA_def_cfa_offset },
    { "cfi_adjust_cfa_offset", dot_cfi, CFI_adjust_cfa_offset },
    { "cfi_offset", dot_cfi, CFA_offset },
    { "cfi_register", dot_cfi, CFA_register },
    { NULL, NULL, 0 }
  };

static const char *
cfi_insn_str (enum cfi_insn insn)
{
  switch (insn)
    {
    case CFA_nop:
      return "CFA_nop";
    case CFA_set_loc:
      return "CFA_set_loc";
    case CFA_advance_loc1:
      return "CFA_advance_loc1";
    case CFA_advance_loc2:
      return "CFA_advance_loc2";
    case CFA_advance_loc4:
      return "CFA_advance_loc4";
    case CFA_offset_extended:
      return "CFA_offset_extended";
    case CFA_resotre_extended:
      return "CFA_resotre_extended";
    case CFA_undefined:
      return "CFA_undefined";
    case CFA_same_value:
      return "CFA_same_value";
    case CFA_register:
      return "CFA_register";
    case CFA_remember_state:
      return "CFA_remember_state";
    case CFA_restore_state:
      return "CFA_restore_state";
    case CFA_def_cfa:
      return "CFA_def_cfa";
    case CFA_def_cfa_register:
      return "CFA_def_cfa_register";
    case CFA_def_cfa_offset:
      return "CFA_def_cfa_offset";
    case CFA_advance_loc:
      return "CFA_advance_loc";
    case CFA_offset:
      return "CFA_offset";
    case CFA_restore:
      return "CFA_restore";
    default:
      break;
    }

  return "CFA_unknown";
}

static struct cfi_data *
alloc_cfi_data (void)
{
  return (struct cfi_data *) xcalloc (sizeof (struct cfi_info), 1);
}

static struct cfi_info *
alloc_cfi_info (void)
{
  return (struct cfi_info *) xcalloc (sizeof (struct cfi_info), 1);
}

/* Parse arguments.  */
static int
cfi_parse_arg (long *param, int resolvereg)
{
  long value;
  int retval = -1;
  int nchars;

  assert (param != NULL);
  SKIP_WHITESPACE ();

  if (sscanf (input_line_pointer, "%li%n", &value, &nchars) >= 1)
    {
      input_line_pointer += nchars;
      retval = 1;
    }
#ifdef tc_regname_to_dw2regnum
  else if (resolvereg && ((is_name_beginner (*input_line_pointer))
			   || (*input_line_pointer == '%'
			       && is_name_beginner (*(++input_line_pointer)))))
    {
      char *name, c, *p;

      name = input_line_pointer;
      c = get_symbol_end ();
      p = input_line_pointer;

      if ((value = tc_regname_to_dw2regnum (name)) >= 0)
	retval = 1;

      *p = c;
    }
#endif
  else
    as_bad (resolvereg ?
	    _("can't convert argument to a register number") :
	    _("can't convert argument to an integer"));

  if (retval > 0)
    *param = value;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      SKIP_WHITESPACE ();
    }

  return retval;
}

static int
cfi_parse_reg (long *param)
{
  return cfi_parse_arg (param, 1);
}

static int
cfi_parse_const (long *param)
{
  return cfi_parse_arg (param, 0);
}

void
cfi_add_insn (enum cfi_insn insn, long param0, long param1)
{
  struct cfi_data *data_ptr;

  if (!cfi_info->data)
    {
      cfi_info->data = alloc_cfi_data ();
      data_ptr = cfi_info->data;
    }
  else
    {
      data_ptr = cfi_info->data;

      while (data_ptr && data_ptr->next)
	data_ptr = data_ptr->next;

      data_ptr->next = alloc_cfi_data ();

      data_ptr = data_ptr->next;
    }

  data_ptr->insn = insn;
  data_ptr->param[0] = param0;
  data_ptr->param[1] = param1;
}

static void
cfi_advance_loc (void)
{
  addressT curr_address = frag_now_fix ();
  if (cfi_info->last_address == curr_address)
    return;
  cfi_add_insn (CFA_advance_loc,
		(long) (curr_address - cfi_info->last_address), 0);
  cfi_info->last_address = curr_address;
}

static long
get_current_offset (struct cfi_info *info)
{
  long current_offset = 0;
  struct cfi_data *data = info->data;

  current_offset = 0;
  while (data)
    {
      if (data->insn == CFA_def_cfa)
	current_offset = data->param[1];
      else if (data->insn == CFA_def_cfa_offset)
	current_offset = data->param[0];
      data = data->next;
    }

  return current_offset;
}

static void
cfi_make_insn (int arg)
{
  long param[2] = { 0, 0 };

  if (!cfi_info)
    {
      as_bad (_("CFI instruction used without previous .cfi_startproc"));
      return;
    }

  cfi_advance_loc ();

  switch (arg)
    {
      /* Instructions that take two arguments (register, integer). */
    case CFA_offset:
    case CFA_def_cfa:
      if (cfi_parse_reg (&param[0]) < 0)
	{
	  as_bad (_("first argument to %s is not a register"),
		  cfi_insn_str (arg));
	  return;
	}
      if (cfi_parse_const (&param[1]) < 0)
	{
	  as_bad (_("second argument to %s is not a number"),
		  cfi_insn_str (arg));
	  return;
	}
      break;

    case CFA_register:
      if (cfi_parse_reg (&param[0]) < 0)
	{
	  as_bad (_("first argument to %s is not a register"),
		  cfi_insn_str (arg));
	  return;
	}
      if (cfi_parse_reg (&param[1]) < 0)
	{
	  as_bad (_("second argument to %s is not a register"),
		  cfi_insn_str (arg));
	  return;
	}
      break;

      /* Instructions that take one register argument.  */
    case CFA_def_cfa_register:
      if (cfi_parse_reg (&param[0]) < 0)
	{
	  as_bad (_("argument to %s is not a register"), cfi_insn_str (arg));
	  return;
	}
      break;

      /* Instructions that take one integer argument.  */
    case CFA_def_cfa_offset:
      if (cfi_parse_const (&param[0]) < 0)
	{
	  as_bad (_("argument to %s is not a number"), cfi_insn_str (arg));
	  return;
	}
      break;

      /* Special handling for pseudo-instruction.  */
    case CFI_adjust_cfa_offset:
      if (cfi_parse_const (&param[0]) < 0)
	{
	  as_bad (_("argument to %s is not a number"),
		    ".cfi_adjust_cfa_offset");
	  return;
	}
      param[0] += get_current_offset (cfi_info);
      arg = CFA_def_cfa_offset;
      break;

    default:
      as_bad (_("unknown CFI instruction %d (%s)"), arg, cfi_insn_str (arg));
      return;
    }
  cfi_add_insn (arg, param[0], param[1]);
}

static symbolS *
cfi_get_label (void)
{
  char symname[40], *symbase=".Llbl_cfi";
  symbolS *symbolP;
  unsigned int i = 0;

  snprintf (symname, sizeof (symname), "%s_0x%lx",
	    symbase, (long) frag_now_fix ());
  while ((symbolP = symbol_find (symname)))
    {
      if ((S_GET_VALUE (symbolP) == frag_now_fix ())
	  && (S_GET_SEGMENT (symbolP) == now_seg))
	return symbolP;

      snprintf (symname, sizeof (symname), "%s_0x%lx_%u",
		symbase, (long) frag_now_fix (), i++);
    }
#ifdef BFD_ASSEMBLER
  symbolP = (symbolS *) local_symbol_make (symname, now_seg,
					   (valueT) frag_now_fix (),
					   frag_now);
#else
  symbolP = symbol_make (symname);
#endif
  return symbolP;
}

static void
dot_cfi_startproc (void)
{
#ifdef tc_cfi_frame_initial_instructions
  const char *simple = "simple";
#endif

  if (cfi_info)
    {
      as_bad (_("previous CFI entry not closed (missing .cfi_endproc)"));
      return;
    }

#if defined(TARGET_USE_CFIPOP)
  /* Because this file is linked even for architectures that 
     don't use CFI, we must wrap this call.  */
  if (current_config.addr_length == 0)
    tc_cfi_init ();
#endif

  cfi_info = alloc_cfi_info ();

  cfi_info->start_address = frag_now_fix ();
  cfi_info->last_address = cfi_info->start_address;
  cfi_info->labelname = S_GET_NAME (cfi_get_label ());

  SKIP_WHITESPACE ();
#ifdef tc_cfi_frame_initial_instructions
  if (strncmp (simple, input_line_pointer, strlen (simple)) != 0)
    tc_cfi_frame_initial_instructions ();
  else
    input_line_pointer += strlen (simple);
#endif
}

#define cfi_is_advance_insn(insn)				\
  ((insn >= CFA_set_loc && insn <= CFA_advance_loc4)		\
   || insn == CFA_advance_loc)

/* Output CFI instructions to the file.  */

enum data_types
  {
    t_ascii = 0,
    t_byte = 1,
    t_half = 2,
    t_long = 4,
    t_quad = 8,
    t_uleb128 = 0x10,
    t_sleb128 = 0x11
  };

static int
output_data (char **p, unsigned long *size, enum data_types type, long value)
{
  char *ptr = *p;
  unsigned int ret_size;

  switch (type)
    {
    case t_byte:
      ret_size = 1;
      break;
    case t_half:
      ret_size = 2;
      break;
    case t_long:
      ret_size = 4;
      break;
    case t_quad:
    case t_uleb128:
    case t_sleb128:
      ret_size = 8;
      break;
    default:
      /* This should never happen - throw an internal error.  */
      as_fatal (_("unknown type %d"), type);
      return 0;
    }

  if (*size < ret_size)
    {
      as_bad (_("output_data buffer is too small"));
      return 0;
    }

  switch (type)
    {
    case t_byte:
      *ptr = (char) value;
      if (verbose)
	printf ("\t.byte\t0x%x\n", (unsigned char) *ptr);
      break;
    case t_half:
      *(short *) ptr = (short) value & 0xFFFF;
      if (verbose)
	printf ("\t.half\t0x%x\n", (unsigned short) *ptr);
      break;
    case t_long:
      *(int *) ptr = (int) value & 0xFFFFFFFF;
      if (verbose)
	printf ("\t.long\t0x%x\n", (unsigned int) *ptr);
      break;
    case t_quad:
      *(long long *) ptr = (long long) value & 0xFFFFFFFF;
      if (verbose)
	printf ("\t.quad\t0x%x\n", (unsigned int) *ptr);
      break;
    case t_uleb128:
    case t_sleb128:
      ret_size = output_leb128 (ptr, value, type == t_sleb128);
      if (verbose)
	printf ("\t.%s\t0x%lx\n",
		type == t_sleb128 ? "sleb128" : "uleb128",
		value);
      break;
    default:
      as_fatal (_("unknown type %d"), type);
      return 0;
    }

  *size -= ret_size;
  *p += ret_size;

  return ret_size;
}

static int
cfi_output_insn (struct cfi_data *data, char **buf, unsigned long *buf_size)
{
  char **pbuf = buf, *orig_buf = *buf;
  unsigned long size;

  if (!data || !buf)
    as_fatal (_("cfi_output_insn called with NULL pointer"));

  switch (data->insn)
    {
    case CFA_advance_loc:
      if (verbose)
	printf ("\t# %s(%ld)\n", cfi_insn_str (data->insn),
		data->param[0]);
      if (data->param[0] <= 0x3F)
	{
	  output_data (pbuf, buf_size, t_byte, CFA_advance_loc +
		       (data->param[0] / current_config.code_align));
	}
      else if (data->param[0] <= 0xFF)
	{
	  output_data (pbuf, buf_size, t_byte, CFA_advance_loc1);
	  output_data (pbuf, buf_size, t_byte,
		       data->param[0] / current_config.code_align);
	}
      else if (data->param[0] <= 0xFFFF)
	{
	  output_data (pbuf, buf_size, t_byte, CFA_advance_loc2);
	  output_data (pbuf, buf_size, t_half,
		       data->param[0] / current_config.code_align);
	}
      else
	{
	  output_data (pbuf, buf_size, t_byte, CFA_advance_loc4);
	  output_data (pbuf, buf_size, t_long,
		       data->param[0] / current_config.code_align);
	}
      break;

    case CFA_def_cfa:
      if (verbose)
	printf ("\t# CFA_def_cfa(%ld,%ld)\n",
		data->param[0], data->param[1]);
      output_data (pbuf, buf_size, t_byte, CFA_def_cfa);
      output_data (pbuf, buf_size, t_uleb128, data->param[0]);
      output_data (pbuf, buf_size, t_uleb128, data->param[1]);
      break;

    case CFA_def_cfa_register:
    case CFA_def_cfa_offset:
      if (verbose)
	printf ("\t# %s(%ld)\n", cfi_insn_str (data->insn),
		data->param[0]);
      output_data (pbuf, buf_size, t_byte, data->insn);
      output_data (pbuf, buf_size, t_uleb128, data->param[0]);
      break;

    case CFA_offset:
      if (verbose)
	printf ("\t# %s(%ld,%ld)\n", cfi_insn_str (data->insn),
		data->param[0], data->param[1]);

      /* Check whether to use CFA_offset or CFA_offset_extended.  */
      if (data->param[0] <= 0x3F)
	output_data (pbuf, buf_size, t_byte, CFA_offset + data->param[0]);
      else
	{
	  output_data (pbuf, buf_size, t_byte, CFA_offset_extended);
	  output_data (pbuf, buf_size, t_uleb128, data->param[0]);
	}
      output_data (pbuf, buf_size, t_uleb128,
		   data->param[1] / current_config.data_align);
      break;

    case CFA_register:
      if (verbose)
	printf ("\t# %s(%ld,%ld)\n", cfi_insn_str (data->insn),
		data->param[0], data->param[1]);
      output_data (pbuf, buf_size, t_byte, CFA_register);
      output_data (pbuf, buf_size, t_uleb128, data->param[0]);
      output_data (pbuf, buf_size, t_uleb128, data->param[1]);
      break;

    case CFA_nop:
      if (verbose)
	printf ("\t# CFA_nop\n");
      output_data (pbuf, buf_size, t_byte, CFA_nop);
      break;

    default:
      as_warn ("CFA_unknown[%d](%ld,%ld)", data->insn,
	       data->param[0], data->param[1]);
    }
  size = *pbuf - orig_buf;
  *buf = *pbuf;
  *buf_size -= size;
  return size;
}

static void
dot_cfi_endproc (void)
{
  struct cfi_data *data_ptr;
  struct cie_entry *cie_ptr;
  char *cie_buf, *fde_buf, *pbuf, *where;
  unsigned long buf_size, cie_size, fde_size, last_cie_offset;
  unsigned long fde_initloc_offset, fde_len_offset, fde_offset;
  segT saved_seg, cfi_seg;
  expressionS exp;

  if (! cfi_info)
    {
      as_bad (_(".cfi_endproc without corresponding .cfi_startproc"));
      return;
    }
  cfi_info->end_address = frag_now_fix ();

  /* Open .eh_frame section.  */
  saved_seg = now_seg;
  cfi_seg = subseg_new (".eh_frame", 0);
#ifdef BFD_ASSEMBLER
  bfd_set_section_flags (stdoutput, cfi_seg,
			 SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA);
#endif
  subseg_set (cfi_seg, 0);

  /* Build CIE.  */
  cie_buf = xcalloc (1024, 1);
  /* Skip space for CIE length.  */
  pbuf = cie_buf + 4;
  buf_size = 1020;

  if (verbose)
    printf ("# CIE *****\n");

  /* CIE id.  */
  output_data (&pbuf, &buf_size, t_long, 0x0);
  /* Version.  */
  output_data (&pbuf, &buf_size, t_byte, 1);
  /* Augmentation.  */
  output_data (&pbuf, &buf_size, t_byte, 0);
  /* Code alignment.  */
  output_data (&pbuf, &buf_size, t_uleb128, current_config.code_align);
  /* Data alignment.  */
  output_data (&pbuf, &buf_size, t_sleb128, current_config.data_align);
  /* Return address column.  */
  output_data (&pbuf, &buf_size, t_byte, current_config.ra_column);

  /* Build CFI instructions.  */
  data_ptr = cfi_info->data;
  while (data_ptr && !cfi_is_advance_insn (data_ptr->insn))
    {
      cfi_output_insn (data_ptr, &pbuf, &buf_size);
      data_ptr = data_ptr->next;
    }

  /* Align the whole data to current_config.eh_align.  */
  cie_size = pbuf - cie_buf;
  cie_size += current_config.eh_align - cie_size % current_config.eh_align;

  /* CIE length.  */
  pbuf = cie_buf;
  output_data (&pbuf, &buf_size, t_long, cie_size - 4);

  /* OK, we built the CIE. Let's write it to the file...  */
  last_cie_offset = frag_now_fix ();

  /* Check if we have already emitted the exactly same CIE. 
     If yes then use its offset instead and don't put out 
     the new one.  */
  cie_ptr = cie_root;
  while (cie_ptr)
    {
      if (cie_ptr->size == cie_size - 4
	  && memcmp (cie_ptr->data, cie_buf + 4, cie_ptr->size) == 0)
	break;
      cie_ptr = cie_ptr->next;
    }

  /* If we have found the same CIE, use it...  */
  if (cie_ptr)
    {
      if (verbose)
	printf ("# Duplicate CIE found. Previous is at offset %lu\n",
		cie_ptr->offset);
      last_cie_offset = cie_ptr->offset;
    }
  else
    {
      /* Otherwise join this CIE to the list.  */
      where = (unsigned char *) frag_more (cie_size);
      memcpy (where, cie_buf, cie_size);
      if (cie_root)
	{
	  cie_ptr = cie_root;
	  while (cie_ptr->next)
	    cie_ptr = cie_ptr->next;
	  cie_ptr->next = calloc (sizeof (struct cie_entry), 1);
	  cie_ptr = cie_ptr->next;
	}
      else
	{
	  cie_root = calloc (sizeof (struct cie_entry), 1);
	  cie_ptr = cie_root;
	}

      cie_ptr->size = cie_size - 4;
      cie_ptr->data = calloc (cie_ptr->size, 1);
      cie_ptr->offset = last_cie_offset;
      memcpy (cie_ptr->data, cie_buf + 4, cie_ptr->size);
    }

  /* Clean up.  */
  free (cie_buf);

  /* Build the FDE...  */
  fde_buf = xcalloc (1024, 1);
  pbuf = fde_buf;
  buf_size = 1024;

  /* Offset of this FDE in current fragment.  */
  fde_offset = frag_now_fix ();

  if (verbose)
    {
      printf ("# FDE: start=0x%lx, end=0x%lx, delta=%d\n",
	      (long) cfi_info->start_address,
	      (long) cfi_info->end_address,
	      (int) (cfi_info->end_address - cfi_info->start_address));
    }

  /* FDE length (t_long, 4 bytes) - will be set later.  */
  fde_len_offset = pbuf - fde_buf;
  pbuf += 4;
  buf_size -= 4;

  /* CIE pointer - offset from here.  */
  output_data (&pbuf, &buf_size, t_long, fde_offset - last_cie_offset + 4);

  /* FDE initial location - this must be set relocatable!  */
  fde_initloc_offset = pbuf - fde_buf + fde_offset;
  output_data (&pbuf, &buf_size, current_config.addr_length,
	       cfi_info->start_address);

  /* FDE address range.  */
  output_data (&pbuf, &buf_size, current_config.addr_length,
	       cfi_info->end_address - cfi_info->start_address);

  while (data_ptr)
    {
      cfi_output_insn (data_ptr, &pbuf, &buf_size);
      data_ptr = data_ptr->next;
    }

  fde_size = pbuf - fde_buf;
  fde_size += current_config.eh_align - fde_size % current_config.eh_align;

  /* Now we can set FDE length.  */
  pbuf = fde_buf + fde_len_offset;
  buf_size = 4;
  output_data (&pbuf, &buf_size, t_long, fde_size - 4);

  /* Copy FDE to objfile.  */
  where = (unsigned char *) frag_more (fde_size);
  memcpy (where, fde_buf, fde_size);

  /* Set relocation for initial address.  */
  buf_size = current_config.addr_length;
  memset (&exp, 0, sizeof (exp));
  exp.X_op = O_symbol;
  exp.X_add_symbol = symbol_find (cfi_info->labelname);
  fix_new_exp (frag_now, fde_initloc_offset,
	       current_config.addr_length,
	       &exp, 0, current_config.reloc_type);

  /* Clean up.  */
  free (fde_buf);

  free (cfi_info);
  cfi_info = NULL;

  /* Restore previous segment.  */
  subseg_set (saved_seg, 0);
}

void
dot_cfi (int arg)
{
  long param;

  switch (arg)
    {
    case CFI_startproc:
      dot_cfi_startproc ();
      break;
    case CFI_endproc:
      dot_cfi_endproc ();
      break;
    case CFA_def_cfa:
    case CFA_def_cfa_register:
    case CFA_def_cfa_offset:
    case CFA_offset:
    case CFA_register:
    case CFI_adjust_cfa_offset:
      cfi_make_insn (arg);
      break;
    case CFI_verbose:
      if (cfi_parse_const (&param) >= 0)
	verbose = (int) param;
      else
	verbose = 1;
      break;
    default:
      as_bad (_("unknown CFI code 0x%x (%s)"), arg, cfi_insn_str (arg));
      break;
    }
  ignore_rest_of_line ();
}

void
cfi_set_config (struct cfi_config *cfg)
{
  assert (cfg != NULL);
  assert (cfg->addr_length > 0);

  current_config = *cfg;
}

void
cfi_finish (void)
{
  if (cfi_info)
    as_bad (_("open CFI at the end of file; missing .cfi_endproc directive"));
}
