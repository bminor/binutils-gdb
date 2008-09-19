/* test_plugin.c -- simple linker plugin test

   Copyright 2008 Free Software Foundation, Inc.
   Written by Cary Coutant <ccoutant@google.com>.

   This file is part of gold.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plugin-api.h"

struct claimed_file
{
  const char* name;
  void* handle;
  int nsyms;
  struct ld_plugin_symbol* syms;
  struct claimed_file* next;
};

static struct claimed_file* first_claimed_file = NULL;
static struct claimed_file* last_claimed_file = NULL;

static ld_plugin_register_claim_file register_claim_file_hook = NULL;
static ld_plugin_register_all_symbols_read register_all_symbols_read_hook = NULL;
static ld_plugin_register_cleanup register_cleanup_hook = NULL;
static ld_plugin_add_symbols add_symbols = NULL;
static ld_plugin_get_symbols get_symbols = NULL;
static ld_plugin_add_input_file add_input_file = NULL;
static ld_plugin_message message = NULL;

#define MAXOPTS 10

static const char *opts[MAXOPTS];
static int nopts = 0;

enum ld_plugin_status onload(struct ld_plugin_tv *tv);
enum ld_plugin_status claim_file_hook(const struct ld_plugin_input_file *file,
                                      int *claimed);
enum ld_plugin_status all_symbols_read_hook(void);
enum ld_plugin_status cleanup_hook(void);

enum ld_plugin_status
onload(struct ld_plugin_tv *tv)
{
  struct ld_plugin_tv *entry;
  int api_version = 0;
  int gold_version = 0;
  int i;

  for (entry = tv; entry->tv_tag != LDPT_NULL; ++entry)
    {
      switch (entry->tv_tag)
        {
        case LDPT_API_VERSION:
          api_version = entry->tv_u.tv_val;
          break;
        case LDPT_GOLD_VERSION:
          gold_version = entry->tv_u.tv_val;
          break;
        case LDPT_LINKER_OUTPUT:
          break;
        case LDPT_OPTION:
          if (nopts < MAXOPTS)
            opts[nopts++] = entry->tv_u.tv_string;
          break;
        case LDPT_REGISTER_CLAIM_FILE_HOOK:
          register_claim_file_hook = entry->tv_u.tv_register_claim_file;
          break;
        case LDPT_REGISTER_ALL_SYMBOLS_READ_HOOK:
          register_all_symbols_read_hook =
            entry->tv_u.tv_register_all_symbols_read;
          break;
        case LDPT_REGISTER_CLEANUP_HOOK:
          register_cleanup_hook = entry->tv_u.tv_register_cleanup;
          break;
        case LDPT_ADD_SYMBOLS:
          add_symbols = entry->tv_u.tv_add_symbols;
          break;
        case LDPT_GET_SYMBOLS:
          get_symbols = entry->tv_u.tv_get_symbols;
          break;
        case LDPT_ADD_INPUT_FILE:
          add_input_file = entry->tv_u.tv_add_input_file;
          break;
        case LDPT_MESSAGE:
          message = entry->tv_u.tv_message;
          break;
        default:
          break;
        }
    }

  if (message == NULL)
    {
      fprintf(stderr, "tv_message interface missing\n");
      return LDPS_ERR;
    }

  if (register_claim_file_hook == NULL)
    {
      fprintf(stderr, "tv_register_claim_file_hook interface missing\n");
      return LDPS_ERR;
    }

  if (register_all_symbols_read_hook == NULL)
    {
      fprintf(stderr, "tv_register_all_symbols_read_hook interface missing\n");
      return LDPS_ERR;
    }

  if (register_cleanup_hook == NULL)
    {
      fprintf(stderr, "tv_register_cleanup_hook interface missing\n");
      return LDPS_ERR;
    }

  (*message)(LDPL_INFO, "API version:   %d", api_version);
  (*message)(LDPL_INFO, "gold version:  %d", gold_version);

  for (i = 0; i < nopts; ++i)
    (*message)(LDPL_INFO, "option: %s", opts[i]);

  if ((*register_claim_file_hook)(claim_file_hook) != LDPS_OK)
    {
      (*message)(LDPL_ERROR, "error registering claim file hook");
      return LDPS_ERR;
    }

  if ((*register_all_symbols_read_hook)(all_symbols_read_hook) != LDPS_OK)
    {
      (*message)(LDPL_ERROR, "error registering all symbols read hook");
      return LDPS_ERR;
    }

  if ((*register_cleanup_hook)(cleanup_hook) != LDPS_OK)
    {
      (*message)(LDPL_ERROR, "error registering cleanup hook");
      return LDPS_ERR;
    }

  return LDPS_OK;
}

enum ld_plugin_status
claim_file_hook (const struct ld_plugin_input_file* file, int* claimed)
{
  int len;
  char buf[160];
  struct claimed_file* claimed_file;
  struct ld_plugin_symbol* syms;
  int nsyms = 0;
  int maxsyms = 0;
  FILE* irfile;
  char *p;
  char *pbind;
  char *pvis;
  char *psect;
  int weak;
  int def;
  int vis;
  int size;
  char* name;
  int is_comdat;
  int i;

  (*message)(LDPL_INFO,
             "%s: claim file hook called (offset = %ld, size = %ld)",
             file->name, (long)file->offset, (long)file->filesize);

  /* Look for the beginning of output from readelf -s.  */
  irfile = fdopen(file->fd, "r");
  (void)fseek(irfile, file->offset, SEEK_SET);
  len = fread(buf, 1, 13, irfile);
  if (len < 13 || strncmp(buf, "\nSymbol table", 13) != 0)
    return LDPS_OK;

  /* Skip the two header lines.  */
  (void) fgets(buf, sizeof(buf), irfile);
  (void) fgets(buf, sizeof(buf), irfile);

  if (add_symbols == NULL)
    {
      fprintf(stderr, "tv_add_symbols interface missing\n");
      return LDPS_ERR;
    }

  /* Parse the output from readelf. The columns are:
     Index Value Size Type Binding Visibility Section Name.  */
  syms = (struct ld_plugin_symbol*)malloc(sizeof(struct ld_plugin_symbol) * 8);
  if (syms == NULL)
    return LDPS_ERR;
  maxsyms = 8;
  while (fgets(buf, sizeof(buf), irfile) != NULL)
    {
      p = buf;
      p += strspn(p, " ");

      /* Index field.  */
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Value field.  */
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Size field.  */
      size = atoi(p);
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Type field.  */
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Binding field.  */
      pbind = p;
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Visibility field.  */
      pvis = p;
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Section field.  */
      psect = p;
      p += strcspn(p, " ");
      p += strspn(p, " ");

      /* Name field.  */
      /* FIXME:  Look for version.  */
      len = strlen(p);
      if (p[len-1] == '\n')
        p[--len] = '\0';
      name = malloc(len + 1);
      strncpy(name, p, len + 1);

      /* Ignore local symbols.  */
      if (strncmp(pbind, "LOCAL", 5) == 0)
        continue;

      weak = strncmp(pbind, "WEAK", 4) == 0;
      if (strncmp(psect, "UND", 3) == 0)
        def = weak ? LDPK_WEAKUNDEF : LDPK_UNDEF;
      else if (strncmp(psect, "COM", 3) == 0)
        def = LDPK_COMMON;
      else
        def = weak ? LDPK_WEAKDEF : LDPK_DEF;

      if (strncmp(pvis, "INTERNAL", 8) == 0)
        vis = LDPV_INTERNAL;
      else if (strncmp(pvis, "HIDDEN", 6) == 0)
        vis = LDPV_HIDDEN;
      else if (strncmp(pvis, "PROTECTED", 9) == 0)
        vis = LDPV_PROTECTED;
      else
        vis = LDPV_DEFAULT;

      /* If the symbol is listed in the options list, special-case
         it as a comdat symbol.  */
      is_comdat = 0;
      for (i = 0; i < nopts; ++i)
        {
          if (name != NULL && strcmp(name, opts[i]) == 0)
            {
              is_comdat = 1;
              break;
            }
        }

      if (nsyms >= maxsyms)
        {
          syms = (struct ld_plugin_symbol*)
            realloc(syms, sizeof(struct ld_plugin_symbol) * maxsyms * 2);
          if (syms == NULL)
            return LDPS_ERR;
          maxsyms *= 2;
        }

      syms[nsyms].name = name;
      syms[nsyms].version = NULL;
      syms[nsyms].def = def;
      syms[nsyms].visibility = vis;
      syms[nsyms].size = size;
      syms[nsyms].comdat_key = is_comdat ? name : NULL;
      syms[nsyms].resolution = LDPR_UNKNOWN;
      ++nsyms;
    }

  claimed_file = (struct claimed_file*) malloc(sizeof(struct claimed_file));
  if (claimed_file == NULL)
    return LDPS_ERR;

  claimed_file->name = file->name;
  claimed_file->handle = file->handle;
  claimed_file->nsyms = nsyms;
  claimed_file->syms = syms;
  claimed_file->next = NULL;
  if (last_claimed_file == NULL)
    first_claimed_file = claimed_file;
  else
    last_claimed_file->next = claimed_file;
  last_claimed_file = claimed_file;

  (*add_symbols)(file->handle, nsyms, syms);

  *claimed = 1;
  return LDPS_OK;
}

enum ld_plugin_status
all_symbols_read_hook(void)
{
  int i;
  const char* res;
  struct claimed_file* claimed_file;
  char buf[160];
  char *p;

  (*message)(LDPL_INFO, "all symbols read hook called");

  if (get_symbols == NULL)
    {
      fprintf(stderr, "tv_get_symbols interface missing\n");
      return LDPS_ERR;
    }

  for (claimed_file = first_claimed_file;
       claimed_file != NULL;
       claimed_file = claimed_file->next)
    {
      (*get_symbols)(claimed_file->handle, claimed_file->nsyms,
                     claimed_file->syms);
      for (i = 0; i < claimed_file->nsyms; ++i)
        {
          switch (claimed_file->syms[i].resolution)
            {
            case LDPR_UNKNOWN:
              res = "UNKNOWN";
              break;
            case LDPR_UNDEF:
              res = "UNDEF";
              break;
            case LDPR_PREVAILING_DEF:
              res = "PREVAILING_DEF_REG";
              break;
            case LDPR_PREVAILING_DEF_IRONLY:
              res = "PREVAILING_DEF_IRONLY";
              break;
            case LDPR_PREEMPTED_REG:
              res = "PREEMPTED_REG";
              break;
            case LDPR_PREEMPTED_IR:
              res = "PREEMPTED_IR";
              break;
            case LDPR_RESOLVED_IR:
              res = "RESOLVED_IR";
              break;
            case LDPR_RESOLVED_EXEC:
              res = "RESOLVED_EXEC";
              break;
            case LDPR_RESOLVED_DYN:
              res = "RESOLVED_DYN";
              break;
            default:
              res = "?";
              break;
            }
          (*message)(LDPL_INFO, "%s: %s: %s", claimed_file->name,
                     claimed_file->syms[i].name, res);
        }
    }

  if (add_input_file == NULL)
    {
      fprintf(stderr, "tv_add_input_file interface missing\n");
      return LDPS_ERR;
    }

  for (claimed_file = first_claimed_file;
       claimed_file != NULL;
       claimed_file = claimed_file->next)
    {
      if (strlen(claimed_file->name) >= sizeof(buf))
        {
          (*message)(LDPL_FATAL, "%s: filename too long", claimed_file->name);
          return LDPS_ERR;
        }
      strcpy(buf, claimed_file->name);
      p = strrchr(buf, '.');
      if (p == NULL || strcmp(p, ".syms") != 0)
        {
          (*message)(LDPL_FATAL, "%s: filename must have '.syms' suffix",
                     claimed_file->name);
          return LDPS_ERR;
        }
      p[1] = 'o';
      p[2] = '\0';
      (*add_input_file)(buf);
    }

  return LDPS_OK;
}

enum ld_plugin_status
cleanup_hook(void)
{
  (*message)(LDPL_INFO, "cleanup hook called");
  return LDPS_OK;
}
