/* Module support.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"
#include "sim-io.h"
#include "sim-options.h"
#include "sim-assert.h"

#include "libiberty.h"

/* List of all modules.  */
static MODULE_INSTALL_FN * const modules[] = {
  standard_install,
  sim_events_install,
#if WITH_ENGINE
  sim_engine_install,
#endif
#if WITH_TRACE
  trace_install,
#endif
#if WITH_PROFILE
  profile_install,
#endif
  sim_core_install,
#ifndef SIM_HAVE_FLATMEM
  /* FIXME: should handle flatmem as well FLATMEM */
  sim_memopt_install,
#endif
#if WITH_WATCHPOINTS
  sim_watchpoint_install,
#endif
#if WITH_SCACHE
  scache_install,
#endif
#ifdef SIM_HAVE_MODEL
  model_install,
#endif
#ifdef SIM_HAVE_BREAKPOINTS
  sim_break_install,
#endif
  /* Configured in [simulator specific] additional modules.  */
#ifdef MODULE_LIST
  MODULE_LIST
#endif
  0
};

/* Functions called from sim_open.  */

/* Initialize common parts before argument processing.  */

SIM_RC
sim_pre_argv_init (SIM_DESC sd, const char *myname)
{
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  STATE_MY_NAME (sd) = myname + strlen (myname);
  while (STATE_MY_NAME (sd) > myname && STATE_MY_NAME (sd)[-1] != '/')
    --STATE_MY_NAME (sd);

  /* Set the cpu names to default values.  */
  {
    int i;
    for (i = 0; i < MAX_NR_PROCESSORS; ++i)
      {
	const char *name;
	asprintf (&name, "cpu%d", i);
	CPU_NAME (STATE_CPU (sd, i)) = name;
      }
  }

  /* Install all configured in modules.  */
  if (sim_module_install (sd) != SIM_RC_OK)
    return SIM_RC_FAIL;

  return SIM_RC_OK;
}

/* Initialize common parts after argument processing.  */

SIM_RC
sim_post_argv_init (SIM_DESC sd)
{
  int i;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  if (sim_module_init (sd) != SIM_RC_OK)
    return SIM_RC_FAIL;

  /* Set the cpu->state backlinks for each cpu.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    CPU_STATE (STATE_CPU (sd, i)) = sd;

  return SIM_RC_OK;
}

/* Install all modules.
   If this fails, no modules are left installed.  */

SIM_RC
sim_module_install (SIM_DESC sd)
{
  MODULE_INSTALL_FN * const *modp;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  for (modp = modules; *modp != NULL; ++modp)
    {
      if ((*modp) (sd) != SIM_RC_OK)
	{
	  sim_module_uninstall (sd);
	  return SIM_RC_FAIL;
	}
    }
  return SIM_RC_OK;
}

/* Called after all modules have been installed and after argv
   has been processed.  */

SIM_RC
sim_module_init (SIM_DESC sd)
{
  MODULE_INIT_LIST *modp;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  for (modp = STATE_INIT_LIST (sd); modp != NULL; modp = modp->next)
    {
      if ((*modp->fn) (sd) != SIM_RC_OK)
	return SIM_RC_FAIL;
    }
  return SIM_RC_OK;
}

/* Called when ever the simulator is resumed */

SIM_RC
sim_module_resume (SIM_DESC sd)
{
  MODULE_RESUME_LIST *modp;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  for (modp = STATE_RESUME_LIST (sd); modp != NULL; modp = modp->next)
    {
      if ((*modp->fn) (sd) != SIM_RC_OK)
	return SIM_RC_FAIL;
    }
  return SIM_RC_OK;
}

/* Called when ever the simulator is suspended */

SIM_RC
sim_module_suspend (SIM_DESC sd)
{
  MODULE_SUSPEND_LIST *modp;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  for (modp = STATE_SUSPEND_LIST (sd); modp != NULL; modp = modp->next)
    {
      if ((*modp->fn) (sd) != SIM_RC_OK)
	return SIM_RC_FAIL;
    }
  return SIM_RC_OK;
}

/* Uninstall installed modules, called by sim_close.  */

void
sim_module_uninstall (SIM_DESC sd)
{
  MODULE_UNINSTALL_LIST *modp;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  /* Uninstall the modules.  */
  for (modp = STATE_UNINSTALL_LIST (sd); modp != NULL; modp = modp->next)
    (*modp->fn) (sd);
}

/* Add FN to the init handler list.
   init in the same order as the install. */

void
sim_module_add_init_fn (SIM_DESC sd, MODULE_INIT_FN fn)
{
  MODULE_INIT_LIST *l =
    (MODULE_INIT_LIST *) xmalloc (sizeof (MODULE_INIT_LIST));
  MODULE_INIT_LIST **last = &STATE_INIT_LIST (sd);
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  while (*last != NULL)
    last = &((*last)->next);

  l->fn = fn;
  l->next = NULL;
  *last = l;
}

/* Add FN to the resume handler list.
   resume in the same order as the install. */

void
sim_module_add_resume_fn (SIM_DESC sd, MODULE_RESUME_FN fn)
{
  MODULE_RESUME_LIST *l = ZALLOC (MODULE_RESUME_LIST);
  MODULE_RESUME_LIST **last;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  last = &STATE_RESUME_LIST (sd);
  while (*last != NULL)
    last = &((*last)->next);

  l->fn = fn;
  l->next = NULL;
  *last = l;
}

/* Add FN to the init handler list.
   suspend in the reverse order to install. */

void
sim_module_add_suspend_fn (SIM_DESC sd, MODULE_SUSPEND_FN fn)
{
  MODULE_SUSPEND_LIST *l = ZALLOC (MODULE_SUSPEND_LIST);
  MODULE_SUSPEND_LIST **last;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  last = &STATE_SUSPEND_LIST (sd);
  while (*last != NULL)
    last = &((*last)->next);

  l->fn = fn;
  l->next = STATE_SUSPEND_LIST (sd);
  STATE_SUSPEND_LIST (sd) = l;
}

/* Add FN to the uninstall handler list.
   Uninstall in reverse order to install.  */

void
sim_module_add_uninstall_fn (SIM_DESC sd, MODULE_UNINSTALL_FN fn)
{
  MODULE_UNINSTALL_LIST *l =
    (MODULE_UNINSTALL_LIST *) xmalloc (sizeof (MODULE_UNINSTALL_LIST));

  l->fn = fn;
  l->next = STATE_UNINSTALL_LIST (sd);
  STATE_UNINSTALL_LIST (sd) = l;
}
