/* Multi-process control for GDB, the GNU debugger.

   Copyright (C) 2008 Free Software Foundation, Inc.

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
#include "exec.h"
#include "inferior.h"
#include "target.h"
#include "command.h"
#include "gdbcmd.h"
#include "gdbthread.h"
#include "ui-out.h"
#include "observer.h"

void _initialize_inferiors (void);

/*static*/ struct inferior *inferior_list = NULL;
static int highest_inferior_num;

struct inferior *tmp_inf;

/* Print notices on inferior events (attach, detach, etc.), set with
   `set print inferior-events'.  */
static int print_inferior_events = 0;

/* Commands with a prefix of `inferiors'.  */
extern struct cmd_list_element *inferior_cmd_list;

struct inferior *find_inferior_id (int num);
static void info_inferiors_command (char *, int);

struct inferior*
current_inferior (void)
{
  struct inferior *inf = find_inferior_pid (ptid_get_pid (inferior_ptid));
  gdb_assert (inf);
  return inf;
}

static void
free_inferior (struct inferior *inf)
{
  /* FIXME: We can't safely free inferior data until all refs to it
     have been removed, such as from breakpoint locations etc.  */
#if 0
  discard_all_inferior_continuations (inf);
  xfree (inf->private);
  xfree (inf);
#endif
}

/* TODO: I'm not calling this anywhere.  If we stick with the one
   target vector fits all inferiors notion, maybe this should be
   called when the target is pushed on the stack.  */
void
init_inferior_list (void)
{
  struct inferior *inf, *infnext;

  highest_inferior_num = 0;
  if (!inferior_list)
    return;

  for (inf = inferior_list; inf; inf = infnext)
    {
      infnext = inf->next;
      free_inferior (inf);
    }

  inferior_list = NULL;
}

struct inferior *
add_inferior_silent (int pid)
{
  struct inferior *inf;

  inf = xmalloc (sizeof (*inf));
  memset (inf, 0, sizeof (*inf));
  inf->pid = pid;

  inf->num = ++highest_inferior_num;
  inf->next = inferior_list;
  inferior_list = inf;

  return inf;
}

struct inferior *
add_inferior (int pid)
{
  struct inferior *inf = add_inferior_silent (pid);

  observer_notify_new_inferior (pid);

  if (print_inferior_events)
    printf_unfiltered (_("[New inferior %d]\n"), pid);

  return inf;
}

struct delete_thread_of_inferior_arg
{
  int pid;
  int silent;
};

static int
delete_thread_of_inferior (struct thread_info *tp, void *data)
{
  struct delete_thread_of_inferior_arg *arg = data;

  if (ptid_get_pid (tp->ptid) == arg->pid)
    {
      if (arg->silent)
	delete_thread_silent (tp->ptid);
      else
	delete_thread (tp->ptid);
    }

  return 0;
}

/* If SILENT then be quiet -- don't announce a inferior death, or the
   exit of its threads.  */
static void
delete_inferior_1 (int pid, int silent)
{
  struct inferior *inf, *infprev;
  struct delete_thread_of_inferior_arg arg = { pid, silent };

  infprev = NULL;

  for (inf = inferior_list; inf; infprev = inf, inf = inf->next)
    if (inf->pid == pid)
      break;

  if (!inf)
    return;

  if (infprev)
    infprev->next = inf->next;
  else
    inferior_list = inf->next;

  free_inferior (inf);

  arg.pid = pid;
  arg.silent = silent;

  iterate_over_threads (delete_thread_of_inferior, &arg);

  observer_notify_inferior_exit (pid);
}

void
delete_inferior (int pid)
{
  delete_inferior_1 (pid, 0);

  if (print_inferior_events)
    printf_unfiltered (_("[Inferior %d exited]\n"), pid);
}

void
delete_inferior_silent (int pid)
{
  delete_inferior_1 (pid, 1);
}

void
detach_inferior (int pid)
{
  delete_inferior_1 (pid, 1);

  if (print_inferior_events)
    printf_unfiltered (_("[Inferior %d detached]\n"), pid);
}

void
discard_all_inferiors (void)
{
  struct inferior *inf, *infnext;

  for (inf = inferior_list; inf; inf = infnext)
    {
      infnext = inf->next;
      delete_inferior_silent (inf->pid);
    }
}

struct inferior *
find_inferior_id (int num)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->num == num)
      return inf;

  return NULL;
}

/* Find a inferior by matching PID.  */
struct inferior *
find_inferior_pid (int pid)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->pid == pid)
      return inf;

  return NULL;
}

struct inferior *
find_inferior_by_name (char *name)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->name && strcmp (inf->name, name) == 0)
      return inf;

  return NULL;
}

struct inferior *
iterate_over_inferiors (int (*callback) (struct inferior *, void *),
			void *data)
{
  struct inferior *inf, *infnext;

  for (inf = inferior_list; inf; inf = infnext)
    {
      infnext = inf->next;
      if ((*callback) (inf, data))
	return inf;
    }

  return NULL;
}

int
valid_gdb_inferior_id (int num)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->num == num)
      return 1;

  return 0;
}

int
pid_to_gdb_inferior_id (int pid)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->pid == pid)
      return inf->num;

  return 0;
}

int
gdb_inferior_id_to_pid (int num)
{
  struct inferior *inferior = find_inferior_id (num);
  if (inferior)
    return inferior->pid;
  else
    return -1;
}

int
in_inferior_list (int pid)
{
  struct inferior *inf;

  for (inf = inferior_list; inf; inf = inf->next)
    if (inf->pid == pid)
      return 1;

  return 0;
}

int
have_inferiors (void)
{
  return inferior_list != NULL;
}

int
number_of_inferiors (void)
{
  struct inferior *inf;
  int rslt = 0;

  for (inf = inferior_list; inf; inf = inf->next)
    ++rslt;
  return rslt;
}


/* Prints the list of inferiors and their details on UIOUT.  This is a
   version of 'info_inferior_command' suitable for use from MI.

   If REQUESTED_INFERIOR is not -1, it's the GDB id of the inferior that
   should be printed.  Otherwise, all inferiors are printed.  */
void
print_inferior (struct ui_out *uiout, int requested_inferior)
{
  struct inferior *inf;
  char *extra_info;
  struct cleanup *old_chain;

  old_chain = make_cleanup_ui_out_list_begin_end (uiout, "inferiors");

  for (inf = inferior_list; inf; inf = inf->next)
    {
      struct cleanup *chain2;

      if (requested_inferior != -1 && inf->num != requested_inferior)
	continue;

      chain2 = make_cleanup_ui_out_tuple_begin_end (uiout, NULL);

      if (inf->pid > 0 && inf->pid == PIDGET (inferior_ptid))
	ui_out_text (uiout, "* ");
      else
	ui_out_text (uiout, "  ");

      ui_out_field_int (uiout, "id", inf->num);
      ui_out_text (uiout, " ");
      ui_out_field_int (uiout, "target-id", inf->pid);
      if (inf->name)
	{
	  ui_out_text (uiout, " #");
	  ui_out_field_string (uiout, "name", inf->name);
	  ui_out_text (uiout, "#");
	}
      if (inf->exec)
	{
	  ui_out_text (uiout, " ");
	  /* Use short names for execs, except for exec's own
	     inferior.  */
	  ui_out_field_string (uiout, "exec",
			       (inf->exec->inferior == inf
				? inf->exec->name
				: inf->exec->shortname));
	  if (inf->args)
	    {
	      ui_out_text (uiout, " ");
	      ui_out_field_string (uiout, "args", inf->args);
	    }
	  if (inf->inf_environ)
	    {
	      ui_out_text (uiout, " ");
	      ui_out_field_string (uiout, "environ", "env=X");
	    }
	  if (inf->io_terminal)
	    {
	      ui_out_text (uiout, " ");
	      ui_out_field_string (uiout, "io_terminal", "term=Y");
	    }
	}
#if 0
      extra_info = target_extra_inferior_info (inf);
      if (extra_info)
	{
	  ui_out_text (uiout, " (");
	  ui_out_field_string (uiout, "details", extra_info);
	  ui_out_text (uiout, ")");
	}
#endif
      ui_out_text (uiout, "\n");
      do_cleanups (chain2);
    }

  do_cleanups (old_chain);
}

/* Print information about currently known inferiors.  */

static void
info_inferiors_command (char *arg, int from_tty)
{
  print_inferior (uiout, -1);
}

/* Print notices when new inferiors are created and die.  */
static void
show_print_inferior_events (struct ui_file *file, int from_tty,
			   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Printing of inferior events is %s.\n"), value);
}

void
add_inferior_command (char *args, int from_tty)
{
  struct exec *ex = NULL;
  struct inferior *inf;
  int i, copies = 1;
  char *namebase = "", *namebuf;
  char **argv;

  if (args)
    {
      argv = buildargv (args);
      for (; *argv != NULL; argv++)
	{
	  if (**argv == '-')
	    {
	      if (strcmp (*argv, "-copies") == 0)
		{
		  ++argv;
		  if (!*argv)
		    error ("No argument to -copies");
		  copies = strtol (*argv, NULL, 10);
		}
	      else if (strcmp (*argv, "-name") == 0)
		{
		  ++argv;
		  if (!*argv)
		    error ("No argument to -name");
		  namebase = xstrdup (*argv);
		}
	    }
	  else
	    {
	      ex = find_exec_by_name (*argv);
	      if (!ex)
		{
		  printf_unfiltered ("No exec found named '%s'\n", *argv);
		  /* Should ask whether to continue.  */
		}
	    }
	}
    }

  if (ex)
    ;
  else if (current_exec)
    ex = current_exec;

  for (i = 0; i < copies; ++i)
    {
      inf = add_inferior (0);
      inf->exec = ex;
      inf->removable = 1;
      namebuf = (char *) xmalloc (strlen (namebase) + 10);
      sprintf (namebuf, "%s%d", namebase, inf->num);
      set_inferior_name (inf, namebuf);
      /* Should flag as not having run yet.  */
    }

  printf_filtered ("%d inferiors added.\n", copies);
}

void
remove_inferior_command (char *args, int from_tty)
{
  struct itset *itset;
  struct itset_entry *entry;
  struct inferior *inf;
  int ix;

  itset = make_itset_from_spec (args);

  dump_itset (itset);

  for (ix = 0; VEC_iterate (itset_entry, itset->inferiors, ix, entry); ++ix)
    {
#if 0
      if (entry->inferior->removable)
	delete_inferior (entry->inferior);
      /* (should remove threads?) */
#endif
    }
}

void
set_inferior_name (struct inferior *inf, char *name)
{
  inf->name = name;
}

void
name_inferior_command (char *args, int from_tty)
{
  char *oldname, *newname;
  struct inferior *inf;

  if (args == NULL)
    return;

  while (*args == ' ' || *args == '\t')
    args++;
  oldname = args;
  while (*args && !(*args == ' ' || *args == '\t'))
    args++;
  oldname = savestring (oldname, args - oldname);
  while (*args == ' ' || *args == '\t')
    args++;
  if (*args)
    {
      inf = find_inferior_by_name (oldname);
      newname = args;
      while (*args && !(*args == ' ' || *args == '\t'))
	args++;
      newname = savestring (newname, args - newname);
    }
  else
    {
      /* One-argument case.  */
      inf = first_inferior_in_set (current_itset);
      newname = oldname;
    }
  if (!inf)
    {
      printf_unfiltered("No inferior '%s' to rename.\n", oldname);
      return;
    }
  printf_unfiltered("Rename '%s' to '%s'\n", inf->name, newname);
  set_inferior_name (inf, newname);
  printf_unfiltered("Inferior %d now named '%s'\n", inf->num, inf->name);
}

void
set_inferior_exec (struct inferior *inf, struct exec *exec)
{
  inf->exec = exec;
}

/* set-exec ITSET EXEC
   Set the inferiors found in ITSET to have the executable EXEC. Since
   this command is primarily useful to repairing cases where GDB can't
   do the right thing on its own, we only do minimal error checking,
   and it is possible to overwrite a valid executable with one whose
   addresses don't match up with the inferior.  */

void
set_inferior_exec_command (char *args, int from_tty)
{
  struct exec *exec, *old_exec;
  struct inferior *inf;
  struct itset *itset;
  struct itset_entry *entry;
  char **argv, *itset_spec = NULL, *exec_name = NULL;
  int ix;

  if (!args)
    return;

  argv = buildargv (args);
  itset_spec = *argv++;
  if (!itset_spec)
    error ("No i/t set supplied");
  exec_name = *argv++;
  if (!exec_name)
    error ("No executable name supplied");
  if (*argv)
    warning ("Extra arguments ignored");

  itset = make_itset_from_spec (itset_spec);
  if (itset_is_empty (itset))
    error ("No inferiors in [%s]", itset_spec);

  exec = find_exec_by_name (exec_name);
  if (!exec)
    error ("No exec named `%s'", exec_name);

  for (ix = 0; VEC_iterate (itset_entry, itset->inferiors, ix, entry); ++ix)
    {
      inf = entry->inferior;
      old_exec = inf->exec;
      if (exec == old_exec)
	continue;
      /* Don't touch the exec's own inferior.  */
      if (old_exec && inf == old_exec->inferior)
	continue;
      set_inferior_exec (inf, exec);
      if (old_exec)
	printf_filtered ("Inferior %d exec changed from `%s' to `%s'.\n",
			 inf->num, old_exec->name, exec->name);
      else
	printf_filtered ("Inferior %d now has exec `%s'.\n",
			 inf->num, exec->name);
    }

  /* should free itset here? */
}

/* Inferior/thread sets.  */

struct itid_pair { char *infspec; char *tidspec; };

extern void parse_itset_spec (struct itset *itset);
extern void parse_itset_list (struct itset *itset);
extern void parse_itset_range (struct itset *itset);
extern struct itid_pair parse_itset_itid (struct itset *itset);
extern char *parse_itset_iid (struct itset *itset);
extern char *parse_itset_tid (struct itset *itset);
extern void get_inferior_or_pid (char *spec, struct inferior **infp, int *pidp);
extern void add_inferior_to_itset (struct itset *itset, struct inferior *inf,
				   char *lo_tidspec, char *hi_tidspec);
extern void add_threads_to_itset (struct itset_entry *entry,
				  char *lo_tidspec, char *hi_tidspec);


/* Create a new i/t set, doing a test parse to check syntax.  */

struct itset *
new_itset (char *spec)
{
  struct itset *itset;

  /* We must have a specification to work with.  */
  if (!spec)
    return NULL;

  /* FIXME need to dispose of these properly */
  itset = (struct itset *) xmalloc (sizeof (struct itset));

  itset->name = NULL;
  itset->spec = xstrdup (spec);
  itset->dynamic = 1;
  itset->inferiors = NULL;

  /* Do a first test parse to check syntax.  */
  itset->parsed = 0;
  itset->parse_errors = 0;
  parse_itset_spec (itset);
  itset->parsed = 1;

  /* Do a second time to get in all the semantic actions, aka adding
     items to vectors.  */
  if (!itset->parse_errors)
    parse_itset_spec (itset);

  return itset;
}

void
update_itset (struct itset *itset)
{
  if (!itset)
    return;

  /* Don't touch static itsets.  */
  if (!itset->dynamic)
    return;

  /* If the spec has a syntax error, don't try to work with it.  */
  if (itset->parse_errors)
    return;

  /* Clear the vectors in preparation.  */
  /* FIXME clear thread subvectors too */
  VEC_truncate (itset_entry, itset->inferiors, 0);
  parse_itset_spec (itset);
}

void
parse_itset_spec (struct itset *itset)
{
  itset->parse_head = itset->spec;

  if (*(itset->parse_head) == '!')
    {
      ++(itset->parse_head);
      if (itset->parsed && !itset->parse_errors)
	itset->dynamic = 0;
    }

  parse_itset_list (itset);
}

void
parse_itset_list (struct itset *itset)
{
  /* This is the empty set case.  */
  if (*(itset->parse_head) == '\0')
    return;

  parse_itset_range (itset);
  while (*itset->parse_head == ',')
    {
      ++(itset->parse_head);
      parse_itset_range (itset);
    }
}

void
parse_itset_range (struct itset *itset)
{
  struct itid_pair lopair, hipair;
  struct inferior *lo_inf, *hi_inf, *inf;
  int lo_pid, hi_pid;
  struct itset_entry *entry;

  hipair.infspec = hipair.tidspec = NULL;

  lopair = parse_itset_itid (itset);
  if (*(itset->parse_head) == ':')
    {
      ++(itset->parse_head);
      hipair = parse_itset_itid (itset);
    }
  if (itset->parsed && !itset->parse_errors)
    {
      if (strcmp (lopair.infspec, "all") == 0
	  || strcmp (lopair.infspec, "*") == 0
	  || (hipair.infspec && strcmp (hipair.infspec, "*") == 0))
	{
	  /* Wildcards override everything else.  We need to do the
	     addition incrementally, in case it's supplementing a
	     previous spec; for instance, "2.3,*.5" should add the 3rd
	     thread of inferior 2 and threads 5 of all inferiors.  */
	  for (inf = inferior_list; inf; inf = inf->next)
	    add_inferior_to_itset (itset, inf, lopair.tidspec, hipair.tidspec);
	}
      else
	{
	  get_inferior_or_pid (lopair.infspec, &lo_inf, &lo_pid);
	  if (hipair.infspec)
	    {
	      get_inferior_or_pid (hipair.infspec, &hi_inf, &hi_pid);
	      /* This is a hack to use raw id's for range.  */
	      if (lo_inf && lo_inf->pid == 0 && hi_inf && hi_inf->pid == 0)
		{
		  for (inf = inferior_list; inf; inf = inf->next)
		    if (inf->num >= lo_inf->num && inf->num <= hi_inf->num)
		      add_inferior_to_itset (itset, inf,
					     lopair.tidspec, hipair.tidspec);
		  return;
		}
	      /* Find every inferior in the range of the two supplied
		 pids.  */
	      if (lo_pid >= 0 && hi_pid >= 0)
		{
		  for (inf = inferior_list; inf; inf = inf->next)
		    if (inf->pid >= lo_pid && inf->pid <= hi_pid)
		      add_inferior_to_itset (itset, inf,
					     lopair.tidspec, hipair.tidspec);
		}
	      /* (else error?) */
	    }
	  else
	    {
	      /* Only the low part of a range is found, just add the
		 one inferior.  */
	      if (lo_inf)
		add_inferior_to_itset (itset, lo_inf,
				       lopair.tidspec, hipair.tidspec);
	      /* (else error?) */
	    }
	}
    }
}

/* Given a spec string, find a name or number plausibly representing
   an inferior name or a process pid.  */

void
get_inferior_or_pid (char *spec, struct inferior **infp, int *pidp)
{
  struct inferior *inf;
  int num;
  char *endspec;

  *infp = NULL;
  *pidp = -1;
  /* Look for an inferior with a matching name.  Note that we
     want to allow names with leading digits.  */
  for (inf = inferior_list; inf; inf = inf->next)
    {
      if (inf->name && strcmp (spec, inf->name) == 0)
	{
	  *infp = inf;
	  *pidp = inf->pid;
	  return;
	}
    }
  num = strtol (spec, &endspec, 10);
  if (*endspec != '\0')
    {
      /* (should error?) */
      return;
    }
  *infp = find_inferior_pid (num);
  if (*infp)
    {
      *pidp = (*infp)->pid;
      return;
    }
  /* Hack fallback that look at raw inferior id.  */
  *infp = find_inferior_id (num);
  if (*infp)
    {
      *pidp = (*infp)->pid;
      return;
    }
  /* Assume number is being used as a range bound, don't insist that
     it correspond to an actual inferior.  */
  *pidp = num;
}  

void
add_inferior_to_itset (struct itset *itset, struct inferior *inf,
		       char *lo_tidspec, char *hi_tidspec)
{
  struct inferior *inf2;
  struct itset_entry new_entry, *entry;
  int ix, found = 0;
  
  /* Only add one copy of each inferior to the set.  */
  /* FIXME: this will be a performance problem when we have thousands
     of inferiors.  */
  for (ix = 0; VEC_iterate (itset_entry, itset->inferiors, ix, entry); ++ix)
    if (inf == entry->inferior)
      {
	found = 1;
	break;
      }

  if (!found)
    {
      new_entry.inferior = inf;
      new_entry.threads = NULL;
      entry = VEC_safe_push (itset_entry, itset->inferiors, &new_entry);
    }

  add_threads_to_itset (entry, lo_tidspec, hi_tidspec);

  /* The inclusion of an exec's own inferior directs the addition of
     all inferiors derived from that exec.  */
  if (inf->exec && inf->exec->inferior == inf)
    {
      for (inf2 = inferior_list; inf2; inf2 = inf2->next)
	{
	  if (inf2 != inf && inf2->exec == inf->exec)
	    add_inferior_to_itset (itset, inf2, lo_tidspec, hi_tidspec);
	}
    }
}

/* Encapsulation for data passed to thread callback.  */

struct addbits
{
  struct itset_entry *entry;
  int lo, hi;
};

static int
itset_add_thread_callback (struct thread_info *thread, void *data)
{
  struct addbits *addbits = data;
  struct itset_entry *entry;
  struct thread_info *thr;
  int ix;

  entry = addbits->entry;
  if ((thread->ptid.pid == entry->inferior->pid)
      && (addbits->lo < 0
	  || (thread->num >= addbits->lo && thread->num <= addbits->hi)))
    {
      /* Skip adding dups of thread info.  */
      for (ix = 0; VEC_iterate (thread_p, entry->threads, ix, thr); ++ix)
	if (thread == thr)
	  return 0;
      VEC_safe_push (thread_p, entry->threads, thread);
    }
  return 0;
}

void
add_threads_to_itset (struct itset_entry *entry,
		      char *lo_tidspec, char *hi_tidspec)
{
  struct addbits data;
  int lo_tid, hi_tid;
  char *endspec;

  /* Treat a missing thread spec as equivalent to '*'.  */
  if (!lo_tidspec)
    lo_tidspec = "*";

  data.entry = entry;
  if (strcmp (lo_tidspec, "*") == 0
      || (hi_tidspec && strcmp (hi_tidspec, "*") == 0))
    {
      data.lo = data.hi = -1;
    }
  else
    {
      data.lo = strtol (lo_tidspec, &endspec, 10);
      data.hi = data.lo;
      if (hi_tidspec)
	data.hi = strtol (hi_tidspec, &endspec, 10);
      /* (should detect non-empty endspec and warn) */
    }
  iterate_over_threads (itset_add_thread_callback, &data);
}

/* Parse an inferior.thread pair and return it as a two-element struct.  */

struct itid_pair
parse_itset_itid (struct itset *itset)
{
  struct itid_pair rslt;

  rslt.infspec = parse_itset_iid (itset);
  rslt.tidspec = NULL;
  if (*(itset->parse_head) == '.')
    {
      ++(itset->parse_head);
      rslt.tidspec = parse_itset_tid (itset);
    }
  return rslt;
}

/* Recognize an inferior id, which can be just about anything; not
   much to do here, pass it back up for semantic analysis.  */

char *
parse_itset_iid (struct itset *itset)
{
  char *endpos = itset->parse_head;
  char *term;

  while (strchr ("],:.", *endpos) == NULL)
    ++endpos;
  /* FIXME arrange to discard */
  term = savestring (itset->parse_head, endpos - itset->parse_head);
  itset->parse_head = endpos;

  return term;
}

/* Parse a thread id, which may be a decimal number or "*".  */

char *
parse_itset_tid (struct itset *itset)
{
  char *endpos = itset->parse_head;
  char *term, *endterm;

  while (strchr ("],:.", *endpos) == NULL)
    ++endpos;
  /* FIXME arrange to discard */
  term = savestring (itset->parse_head, endpos - itset->parse_head);
  itset->parse_head = endpos;
  if (strcmp (term, "*") == 0)
    return term;
  else
    {
      strtol (term, &endterm, 10);
      if (*endterm == '\0')
	return term;
      else
	{
	  warning ("'%s' is not a valid thread id\n", term);
	  itset->parse_errors = 1;
	  return NULL;
	}
    }
}

struct itset *
make_itset_from_spec (char *spec)
{
  int len;
  struct itset *itset;

  /* Canonicalize by removing brackets.  */
  if (spec[0] == '[')
    ++spec;
  len = strlen (spec);
  if (spec[len - 1] == ']')
    --len;
  /* FIXME arrange to discard later */
  spec = savestring (spec, len);

  itset = new_itset (spec);

  return itset;
}

int
itset_is_empty (struct itset *itset)
{
  return (itset == NULL || VEC_length (itset_entry, itset->inferiors) == 0);
}

/* See if a given inferior and thread is in the i/t set.  */

int
itset_member (struct itset *itset, struct inferior *inf, int thread_id)
{
  struct itset_entry *entry;
  struct thread_info *thr;
  int ix, iy;

  /* FIXME: this will be a performance problem when we have thousands
     of inferiors.  */
  for (ix = 0; VEC_iterate (itset_entry, itset->inferiors, ix, entry); ++ix)
    if (inf == entry->inferior)
      {
	for (iy = 0; VEC_iterate (thread_p, entry->threads, iy, thr); ++iy)
	  if (thread_id == thr->num)
	    return 1;
      }
  return 0;
}

/* Return the first inferior found in the i/t set.  */

struct inferior *
first_inferior_in_set (struct itset *itset)
{
  struct itset_entry *entry;

  if (!itset
      || !itset->inferiors
      || VEC_length (itset_entry, itset->inferiors) == 0)
    return NULL;

  entry = VEC_index (itset_entry, itset->inferiors, 0);

  if (!entry)
    return NULL;

  return entry->inferior;
}

/* Debugging dump for i/t sets.  */

void
dump_itset (struct itset *itset)
{
  struct itset_entry *entry;
  struct inferior *inf;
  struct thread_info *thr;
  int ix, iy;

  if (!itset)
    {
      printf_unfiltered ("null itset\n");
      return;
    }

  printf_unfiltered ("i/t set specified as '%s' (%s)",
		     itset->spec, (itset->dynamic ? "dynamic" : "static"));

  printf_unfiltered (" {%di", VEC_length (itset_entry, itset->inferiors));
  for (ix = 0; VEC_iterate (itset_entry, itset->inferiors, ix, entry); ++ix)
    {
      printf_unfiltered (",%d", entry->inferior->num);
      printf_unfiltered (".{%dt", VEC_length (thread_p, entry->threads));
      for (iy = 0; VEC_iterate (thread_p, entry->threads, iy, thr); ++iy)
	printf_unfiltered (",%d", thr->num);
      printf_unfiltered ("}");
    }
  printf_unfiltered ("}");
  printf_unfiltered ("\n");
}

void
_initialize_inferiors (void)
{
  add_com ("add-inferior", no_class, add_inferior_command, _("\
Add more inferiors to be run for EXEC."));

  add_com ("remove-inferior", no_class, remove_inferior_command, _("\
Remove the inferiors in ITSET."));

  add_com ("name-inferior", no_class, name_inferior_command, _("\
Change the name of inferior OLDNAME to NEWNAME."));

  add_com ("set-exec", no_class, set_inferior_exec_command, _("\
Change the exec of inferiors in ITSET to EXECNAME"));

  add_info ("inferiors", info_inferiors_command, _("\
Info about currently known inferiors."));

  add_setshow_boolean_cmd ("inferior-events", no_class,
         &print_inferior_events, _("\
Set printing of inferior events (e.g., inferior start and exit)."), _("\
Show printing of inferior events (e.g., inferior start and exit)."), NULL,
         NULL,
         show_print_inferior_events,
         &setprintlist, &showprintlist);
}
