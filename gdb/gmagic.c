#include "defs.h"
#include "gdbcmd.h"
#include "symtab.h"
#include "value.h"
#include "frame.h"
#include "breakpoint.h"
#include "inferior.h"
#include "gdbcore.h"
#include "gmagic.h"
#include <ctype.h>
#include <string.h>

#ifdef DYNAMIC_COMMAND_SUPPORT
#include <dlfcn.h>
#endif

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif


/* Maximum number of bytes of extra data to print, or UINT_MAX for no limit.
   Note that "set extra-data-max 0" stores UINT_MAX in extra_data_max, which
   displays in a show command as "unlimited."  */

static unsigned int extra_data_max;
#define EXTRA_DATA_MAX_DEFAULT		1024

/* Return the number of elements in ARRAY.  */

#define ARRAY_NELEMENTS(array)		(sizeof (array) / sizeof (array[0]))

/* Basic information about a text label */

struct text_label
{
  char *name;		/* label name */
  CORE_ADDR addr;	/* label value or 0 if label not found */
};

/* Labels within the Magic Cap dispatcher that we need to know about
   in order to implement "magic stepping" (that is, stepping over
   Magic Cap method dispatches).  The label addresses are refreshed
   whenever a new symbol table is loaded.  */

struct text_label dispatch_labels[] =
{
  {"__DispatchMethod",         0},	/* normal dispatch entry point */
  {"__DispatchInherited",      0},	/* inherited dispatch entry point */
  {"__DispatchDelegated",      0},	/* delegated dispatch entry point */
  {"__DispatchIntrinsic",      0},	/* intrinsic dispatch entry point */
  {"__DoDispatchMethodBpSite", 0},	/* do dispatch site */
};

/* Accessors for the array above.  */

#define DISPATCH_METHOD_ADDR		dispatch_labels[0].addr
#define DISPATCH_INHERITED_ADDR		dispatch_labels[1].addr
#define DISPATCH_DELEGATED_ADDR		dispatch_labels[2].addr
#define DISPATCH_INTRINSIC_ADDR		dispatch_labels[3].addr
#define DO_DISPATCH_METHOD_ADDR		dispatch_labels[4].addr

/* Cached value objects describing functions in the target program that
   we call frequently.  These are refreshed whenever a new symbol table
   is loaded.  */

static value_ptr remote_get_class_name_val;
static value_ptr remote_get_base_ptr_val;

/* Nonzero means that "magic step" (stepping through the Magic Cap method
   dispatcher to the dispatch target) is enabled.  */

static int magic_step_enabled;

/* function prototypes */

static void
print_object PARAMS ((char *args, int dump));

static void
get_class_name PARAMS ((unsigned long objectID, char **name, int *is_scripted));

static CORE_ADDR
get_base_ptr PARAMS ((unsigned long objectID));

static int
should_dump_extra_data PARAMS ((char *class_name));

static void
dump_extra_data PARAMS ((CORE_ADDR addr, unsigned long length));

static value_ptr
call_function_by_name PARAMS ((char *function_name, int nargs, ...));

static value_ptr
call_function_by_value PARAMS ((value_ptr function_value, int nargs, ...));

static value_ptr
vcall_function_by_value PARAMS ((value_ptr function_value, int nargs,
				 va_list args));
static void
local_shell_escape PARAMS ((char *arg));

static CORE_ADDR
lookup_text_label PARAMS ((char *name, value_ptr *val_ptrptr));

static int
is_dispatcher_entry PARAMS ((CORE_ADDR pc));

static int
is_dispatcher_exit PARAMS ((CORE_ADDR pc));


/* This is the GDB handler for the "dobj" command, which prints a
   verbose description of an object.  ARGS is a string containing an
   expression for the object ID, and FROM_TTY is nonzero if the 
   command was issued interactively.  */

/* ARGSUSED */
static void
dobj_command (args, from_tty)
     char *args;
     int from_tty;
{
  if (remote_get_class_name_val == NULL)
    error ("This version of Magic Cap lacks the runtime support for \"dobj\".");

  print_object (args, 1);
}

/* This is the GDB handler for the "pobj" command, which prints a 
   brief description of an object.  ARGS is a string containing an
   expression for the object ID, and FROM_TTY is nonzero if the 
   command was issued interactively.  */

/* ARGSUSED */
static void
pobj_command (args, from_tty)
     char *args;
     int from_tty;
{
  if (remote_get_class_name_val == NULL)
    error ("This version of Magic Cap lacks the runtime support for \"pobj\".");

  print_object (args, 0);
}

/* This is the GDB handler for the "cdump" command, which prints a
   description of a cluster.  ARGS is a string containing a cluster
   selector, and FROM_TTY is nonzero if the command was issued
   interactively.  
   
   cdump <contextSlot> [/l[ocked]] [/s[tartAddr] <expr>]
		       [/c[lass] <className> | <classNumber>] */

/* ARGSUSED */
static void
cdump_command (args, from_tty)
     char *args;
     int from_tty;
{
  char *token;
  unsigned long cluster;
  CORE_ADDR min_object;
  long display_only_locked;
  long filter_classes;
  long display_only_class_number;
  char *display_only_class_name;

  if (args == NULL)
    error_no_arg ("expression for context slot to dump");

  token = strtok (args, " \t");
  if (token[0] == '/')
    error ("The first argument to cdump must be an expression for the context slot to dump.");

  cluster = parse_and_eval_address (token);

  /* Initialize option values.  Note that we assume that
     sizeof (long) == sizeof (void *) here, in that we pass
     min_object as a long, even though it is a pointer.  */

  min_object			= 0;
  display_only_locked		= 0;
  filter_classes		= 0;
  display_only_class_name	= NULL;
  display_only_class_number	= 0;

  while ((token = strtok (NULL, " \t")) != NULL)
    {
      if (token[0] != '/')
	goto bad_option;

      switch (token[1])
	{
	case 'l':
	  if (token[2] && strcmp (token + 1, "locked"))
	    goto bad_option;

	  display_only_locked = 1;
	  break;

	case 's':
	  if (token[2] && strcmp (token + 1, "startAddr"))
	    goto bad_option;

	  if ((token = strtok (NULL, " \t")) == NULL)
	    error ("Missing start address expression for `/s' option.");

	  min_object = parse_and_eval_address (token);
	  break;

	case 'c':
	  if (token[2] && strcmp (token + 1, "class"))
	    goto bad_option;

	  if ((token = strtok (NULL, " \t")) == NULL)
	    error ("Missing class name or number for `/c' option.");

	  filter_classes = 1;
	  if (isdigit (token[0]))
	    display_only_class_number = parse_and_eval_address (token);
	  else
	    display_only_class_name = token;

	  break;
	
	default:
	  goto bad_option;
	}
    }

  if (display_only_class_name != NULL)
    error ("Sorry, `/c <className>' isn't supported yet.");

  (void)call_function_by_name ("cdump", 6, cluster, min_object,
			       display_only_locked,
			       filter_classes, display_only_class_number,
			       display_only_class_name);
  return;

bad_option:
  error ("Invalid option: `%s'.", token);
}

/* This is the GDB handler for the "esc" command, which lists the
   exception handlers for a given actor.  ARGS is a string containing
   an expression for the objectID of the actor in question, and FROM_TTY
   is nonzero if the command was issued interactively.  */

/* ARGSUSED */
static void
esc_command (args, from_tty)
     char *args;
     int from_tty;
{
  unsigned long object;

  if (args == NULL)
    error_no_arg ("expression for actor's object ID");

  object = parse_and_eval_address (args);
  (void)call_function_by_name ("esc", 1, object);
}

/* This is the GDB handler for the "cnum" command, which converts
   a class number to a class name.  ARGS is a string containing an
   expression for the class number, and FROM_TTY is nonzero if the
   command was issued interactively.  */

/* ARGSUSED */
static void
cnum_command (args, from_tty)
     char *args;
     int from_tty;
{
  unsigned long cnum;

  if (args == NULL)
    error_no_arg ("expression for class number");

  cnum = parse_and_eval_address (args);
  (void)call_function_by_name ("cnum", 1, cnum);
}

/* This is the GDB handler for the "getint" command, which converts an
   intrinsic operation number to the corresponding intrinsic operation name,
   or vice-versa.  ARGS is a string containing the intrinsic number or name,
   and FROM_TTY is nonzero if the command was issued interactively.  */

/* ARGSUSED */
static void
getint_command (args, from_tty)
     char *args;
     int from_tty;
{
  char shell_command[256];

  if (args == NULL)
    error_no_arg ("intrinsic operation number or name");

  if (isdigit (args[0]))
    sprintf (shell_command, "getint %ld", parse_and_eval_address (args));
  else
    sprintf (shell_command, "getint %s", args);

  local_shell_escape (shell_command);
}

/* This is the GDB handler for the "getop" command, which converts an
   operation number to the corresponding operation name, or vice-versa.
   ARGS is a string containing the operation number or name, and FROM_TTY
   is nonzero if the command was issued interactively.  */

/* ARGSUSED */
static void
getop_command (args, from_tty)
     char *args;
     int from_tty;
{
  char shell_command[256];
  CORE_ADDR op_number;

  if (args == NULL)
    error_no_arg ("operation number or name");

  if (isdigit (args[0]))
    sprintf (shell_command, "getop %ld", parse_and_eval_address (args));
  else
    sprintf (shell_command, "getop %s", args);

  local_shell_escape (shell_command);
}

/* This is the GDB handler for the "getindexical" command, which converts
   an indexical number to the corresponding indexical name, or vice-versa.
   ARGS is a string containing the indexical number or name, and FROM_TTY
   is nonzero if the command was issued interactively.  */

/* ARGSUSED */
static void
getindexical_command (args, from_tty)
     char *args;
     int from_tty;
{
  char shell_command[256];

  if (args == NULL)
    error_no_arg ("indexical number or name");

  if (isdigit (args[0]))
    sprintf (shell_command, "getindexical 0x%lx",
	     parse_and_eval_address (args));
  else
    sprintf (shell_command, "getindexical %s", args);

  local_shell_escape (shell_command);
}

/* This is the GDB handler for the "exc" command, which converts an
   exception number to the corresponding exception name, or vice-versa.
   ARGS is a string containing the exception number or name, and FROM_TTY
   is nonzero if the command was issued interactively.

   FIXME why is this one "exc" instead of "getexc?" (inconsistent naming).  */

/* ARGSUSED */
static void
exc_command (args, from_tty)
     char *args;
     int from_tty;
{
  char shell_command[256];

  if (args == NULL)
    error_no_arg ("exception number or name");
	
  if (isdigit (args[0]))
    sprintf (shell_command, "getexc %ld", parse_and_eval_address (args));
  else
    sprintf (shell_command, "getexc %s", args);

  local_shell_escape (shell_command);
}

#ifdef DYNAMIC_COMMAND_SUPPORT
/* Open a dynamic library and invoke an entry point within it.
   ARGS is a string containing the names of the dynamic library
   and the symbolic entry point, separated by whitespace.  */

/* ARGSUSED */
static void
dlopen_command (args, from_tty)
     char *args;
     int from_tty;
{
  char *p;
  void *hdl;
  void (*sym)();

  if (args == 0)
    {
      error ("No arguments specified.");
      return;
    }

  p = args;
  while (*p != ' ' && *p != '\0')
    p++;

  if (*p != ' ')
    {
      error ("Not enough arguments.");
      return;
    }
  *p++ = '\0';

  hdl = dlopen (args, RTLD_NOW);
  if (hdl == NULL)
    {
      fprintf (stderr, "%s: %s\n", args, dlerror ());
      return;
    }

  sym = dlsym (hdl, p);
  if (sym == NULL)
    {
      fprintf (stderr, "%s: %s\n", p, dlerror ());
      return;
    }

  sym();
}
#endif /* DYNAMIC_COMMAND_SUPPORT */

/* Given an object ID OBJECT, return a pointer to a type structure
   representing the GDB type that describes the layout of the object's
   fields in memory (i.e., the "_AllFields" structure corresponding 
   to the object's class).  */

struct type *
type_of_object (object)
     CORE_ADDR object;
{
  char *class_name = NULL;
  char classAllFieldsName[128];
  struct type *type = NULL;
  int is_scripted;

  get_class_name (object, &class_name, &is_scripted);
  sprintf (classAllFieldsName, "%s_AllFields", class_name);
  free (class_name);

  type = lookup_typename (classAllFieldsName, (struct block *)NULL, 0);
  return lookup_pointer_type (type);
}

/* Given OBJECT, an object ID, return the address of the object's
   fixed fields.  */

CORE_ADDR
baseptr_of_object (object)
     unsigned long object;
{
  return get_base_ptr (object) + kSizeOfObjectHeader;
}

/* Given an expression for an object ID, ARGS, print information about
   the object--including its class, the values of its fixed fields, and
   the values in its extra data block.  */

/* ARGSUSED */
static void
print_object (args, dump)
     char *args;
     int dump;
{
  CORE_ADDR addr;
  unsigned long object, object_length;
  char *class_name = NULL;
  char class_all_fields_name[128];
  struct type *type;
  int is_scripted;
  struct cleanup *old_chain;

  if (args == NULL)
    error_no_arg ("expression for object ID");

  object = parse_and_eval_address (args);

  /* check for nilObject */

  if (!object)
    {
      printf_filtered ("\"%s\" evaluates to nilObject.\n", args);
      return;
    }

  /* allow shortcut for system object ids */

  if (IsObjectID (object))
    object |= 1 << kIDBitUsable;
  else if (object < 0x5000)
    object |= (1 << kIDBitObject) | (1 << kIDBitUsable); /* 0x84000000 */

  /* Get the name of the object's class, as well as the GDB type that
     describes the layout of the object's fixed fields.  */
   
  get_class_name (object, &class_name, &is_scripted);
  old_chain = make_cleanup (free_current_contents, &class_name);

  sprintf (class_all_fields_name, "%s_AllFields", class_name);
  type = lookup_typename (class_all_fields_name, (struct block *)NULL, 1);

  /* Get pointer to object's fields.
     FIXME: ADDR is actually an (ObjectHeader *); should use normal expression
     evaluator to extract the length member, rather than hardwiring the 
     format of the structure in this code.  */

  addr = get_base_ptr (object);
  object_length = read_memory_unsigned_integer (addr, 4);
  object_length -= kSizeOfObjectHeader;
  addr += kSizeOfObjectHeader;

  if (type == NULL || (TYPE_CODE (type) != TYPE_CODE_UNDEF
      		       && !(TYPE_FLAGS (type) & TYPE_FLAG_STUB)))
    {
      if (dump)
	{
	  unsigned long fixed_length;

	  printf_filtered ("Object 0x%08lx%s at address 0x%08lx of class %s\n", 
			   object, (is_scripted) ? " (scripted)" : "",
			   addr, class_name);

	  /* If the object has fixed fields, dump them.  */

	  if (type != NULL)
	    {
	      value_ptr valptr = value_at_lazy (type, addr);
	      int histindex = record_latest_value (valptr);

	      if (histindex >= 0)
		printf_filtered ("$%d = ", histindex);

	      value_print (valptr, gdb_stdout, 0, Val_prettyprint);
	      puts_filtered ("\n");
	      fixed_length = TYPE_LENGTH (type);
	    }
	  else
	    fixed_length = 0;

	  /* If the object's length is less than that of its fixed fields,
	     warn the user.  */

	  if (object_length < fixed_length)
	    {
	      error ("Warning: object is too small (should be at least %d bytes, is %d bytes).",
		     fixed_length, object_length);
	    }

	  /* Dump the object's extra data, if any.  should_dump_extra_data ()
	     filters out classes (e.g. Cluster) that have too much extra data
	     to be dumped usefully in this format.  */

	  if (should_dump_extra_data (class_name))
	    {
	      dump_extra_data (addr + fixed_length,
			       object_length - fixed_length);
	    }
	}
      else
	{
	  struct type *pointer_type;
  	  value_ptr valptr;
	  int histindex;

	  pointer_type = lookup_pointer_type ((type == NULL) ?
					      builtin_type_void : type);
	  valptr = value_from_longest (pointer_type, addr);
						
	  histindex = record_latest_value (valptr);
	  if (histindex >= 0)
	    printf_filtered ("$%d = ", histindex);

	  value_print (valptr, gdb_stdout, 0, Val_prettyprint);
	  puts_filtered ("\n");
  	}
    }
  do_cleanups (old_chain);
}

/* Get the name of the class of the object referenced by OBJECTID.
   *NAME is set to a pointer to the string containing the class
   name; it is the caller's responsibility to free the memory for
   the string.  *IS_SCRIPTED is set to nonzero if the object is
   scripted, zero otherwise.  */

static void
get_class_name (objectID, name, is_scripted)
     unsigned long objectID;
     char **name;
     int *is_scripted;
{
  value_ptr val;
  int errno_val;

  val = call_function_by_value (remote_get_class_name_val, 1, objectID);

  /* As RemoteGetClassName() is currently (9/21/95) written, an empty string,
     rather than a nil pointer, is returned upon failure.  I'm leaving the
     value_logical_not test in anyway, though, just for added robustness.  */
     
  if (!value_logical_not (val))
    {
      (void)target_read_string (value_as_pointer (val), name, 256, &errno_val);
      if (errno_val)
	error ("Can't read class name for object 0x%08lx.", objectID);

      if (**name)
	{
	  char *scripted_suffix;

	  if ((scripted_suffix = strstr (*name, " (scripted)")) != NULL)
	    {
	      *scripted_suffix = '\0';
	      *is_scripted = 1;
	    }
	  else
	    *is_scripted = 0;

          return;
	}
      else
	free (*name);
    }

  error ("Bad object ID: 0x%08lx.", objectID);
}

/* Given an object ID, return a pointer to the object's data.  */

static CORE_ADDR
get_base_ptr (objectID)
     unsigned long objectID;
{
  register value_ptr val;

  val = call_function_by_value (remote_get_base_ptr_val, 1, objectID);

  if (value_logical_not (val))
    error ("Could not get base pointer to object.");

  return value_as_pointer (val);
}

/* Return nonzero if we should dump the extra data for an object 
   of class CLASS_NAME.

   FIXME this only works for explicitly named classes, and doesn't
   handle subclasses.  */

static int
should_dump_extra_data (class_name)
     char *class_name;
{
  int i;
  char **name;
  static char *dont_dump_extra_classes[] =
    {
      "Cluster"
    };

  for (i = 0, name = dont_dump_extra_classes;
       i < ARRAY_NELEMENTS(dont_dump_extra_classes);
       i++, name++)
    {
      if (!strcmp (class_name, *name))
	return 0;
    }

  return 1;
}

/* Given ADDR, the address of an object's extra data block, and LENGTH,
   the length of that block in bytes, dump the object's extra data to
   standard output.  */

static void
dump_extra_data (addr, length)
     CORE_ADDR addr;
     unsigned long length;
{
  unsigned long buf[5];
  int chunk;
  int chunk_longs;
  int i;
  int bytes_printed;
  char *p;

  bytes_printed = 0;

  while (length > 3 && bytes_printed < extra_data_max)
    {
      QUIT;		/* allow user to interrupt dump */

      /* read a chunk of extra data */

      chunk = (length > 16) ? 16 : length;
      memset (buf, 0, sizeof (buf));
      read_memory (addr, (char *) &buf, chunk);

      /* format data as hex longwords */

      chunk_longs = chunk >> 2;
      for (i = 0; i < chunk_longs; i++)
	printf_filtered ("%08lx ",
			 extract_unsigned_integer (buf + i, sizeof (long)));

      /* pad to 4 longs */

      for (i = chunk_longs; i < 4; i++)
	puts_filtered ("         ");

      puts_filtered ("| ");

      /* format data as ascii bytes */

      for (i = 0, p = (char*)buf; i < chunk; i++, p++)
	{
	  if (!isprint (*p))
	    *p = '.';
	}
      printf_filtered ("%s |\n", buf);

      addr += chunk;
      length -= chunk;
      bytes_printed += chunk;
    }

  if (length > 0)
    printf_filtered ("(%d bytes of extra data remaining but not displayed.)\n",
		     length);
}

/* Given the name of a function in the target program and a list of
   long arguments, call the function and return a pointer to a value
   object describing the function's return value.  NAME is a string
   containing the name of the function to be called; NARGS is the
   number of arguments to the function; and the remaining parameters
   are the arguments to passed to the function, all assumed to be of
   type long.  */

static value_ptr
#ifdef ANSI_PROTOTYPES
call_function_by_name (char *function_name, int nargs, ...)
#else
call_function_by_name (va_alist)
     va_dcl
#endif
{
  va_list args;
  value_ptr return_value;
  value_ptr function_value;
#ifndef ANSI_PROTOTYPES
  char *function_name;
  int nargs;

  va_start (args);
  function_name = va_arg (args, char *);
  nargs = va_arg (args, int);
#else
  va_start (args, nargs);
#endif

  /* Find the address of function NAME in the inferior.  */

  if (!lookup_text_label (function_name, &function_value))
    error ("Execution of this command requires the debugged program to have a function \"%s.\"",
	   function_name);

  /* Call the function.  */

  return_value = vcall_function_by_value (function_value, nargs, args);
  va_end (args);

  return return_value;
}

/* Given a value object describing a function in the target program and
   a list of long arguments, call the function and return a pointer to a
   value object describing the function's return value.  FUNCTION_VALUE
   is a pointer to a value struct describing the function; NARGS is the
   number of arguments to the function; and the remaining parameters are
   the arguments to passed to the function, all assumed to be of type long.  */

static value_ptr
#ifdef ANSI_PROTOTYPES
call_function_by_value (value_ptr function_value, int nargs, ...)
#else
call_function_by_value (va_alist)
     va_dcl
#endif
{
  va_list args;
  value_ptr return_value;
#ifndef ANSI_PROTOTYPES
  value_ptr function_value;
  int nargs;

  va_start (args);
  function_value = va_arg (args, value_ptr);
  nargs = va_arg (args, int);
#else
  va_start (args, nargs);
#endif

  /* Call the function and return its return value.  */

  return_value = vcall_function_by_value (function_value, nargs, args);
  va_end (args);

  return return_value;
}

/* Helper routine for call_function_by_name and call_function_by_value 
   above.  This function does the work of collecting the function
   arguments into an array of value objects, and then invoking 
   call_function_by_hand to do the real work.  FUNCTION_VALUE is a 
   pointer to a value object describing the function to be called,
   NARGS is the number of arguments to the function, and ARGS is a
   list (va_list) of the arguments to the function, all assumed to
   be of type long.

   Returns a pointer to a value object describing the return value
   of the function.  */

static value_ptr
vcall_function_by_value (function_value, nargs, args)
     value_ptr function_value;
     int nargs;
     va_list args;
{
  value_ptr *arg_values;
  value_ptr return_value;
  struct cleanup *old_chain;
  int i;

  /* Construct a vector of value objects describing the arguments
     to the function to be called.  */

  arg_values = (value_ptr *) xmalloc (nargs * sizeof (value_ptr));
  old_chain = make_cleanup (free_current_contents, &arg_values);

  for (i = 0; i < nargs; i++)
    arg_values[i] = value_from_longest (builtin_type_long,
					(LONGEST) va_arg (args, unsigned long));

  /* Call the function and return its return value.  */

  return_value = call_function_by_hand (function_value, nargs, arg_values);
  do_cleanups (old_chain);
  return return_value;
}

/* Invoke a shell, supplying ARG as the command to be executed.  */

static void
local_shell_escape (arg)
     char *arg;
{
#ifdef CANT_FORK
  /* FIXME: what about errors (I don't know how GO32 system() handles
     them)?  */
  system (arg);
#else /* Can fork.  */
  int rc, status, pid;
  char *p, *user_shell;

  if ((user_shell = (char *) getenv ("SHELL")) == NULL)
    user_shell = "/bin/sh";

  /* Get the name of the shell for arg0 */
  if ((p = strrchr (user_shell, '/')) == NULL)
    p = user_shell;
  else
    p++;			/* Get past '/' */

  if ((pid = fork()) == 0)
    {
      if (!arg)
	execl (user_shell, p, 0);
      else
	execl (user_shell, p, "-c", arg, 0);

      fprintf_unfiltered (gdb_stderr, "Cannot execute %s: %s\n", user_shell,
			  safe_strerror (errno));
      gdb_flush (gdb_stderr);
      _exit (0177);
    }

  if (pid != -1)
    while ((rc = wait (&status)) != pid && rc != -1)
      ;
  else
    error ("Fork failed");
#endif /* Can fork.  */
}

/* Lookup NAME as a text label in the target program.  If NAME is the
   name of a function, and VAL_PTRPTR is not NULL, a pointer to a value
   object describing the function is stored at VAL_PTRPTR.

   Returns the text address to which the label refers, or 0 if the
   label is not found in the target program.  */

static CORE_ADDR
lookup_text_label (name, val_ptrptr)
     char *name;
     value_ptr *val_ptrptr;
{
  struct symbol *sym;
  CORE_ADDR addr;

  addr = 0;

  /* Try looking up NAME as a first-class symbol.  */

  sym = lookup_symbol (name, 0, VAR_NAMESPACE, 0, NULL);
  if (sym != NULL)
    {
      switch (SYMBOL_CLASS (sym))
	{
	case LOC_BLOCK:
	  addr = BLOCK_START (SYMBOL_BLOCK_VALUE (sym));
	  if (val_ptrptr != NULL)
	    *val_ptrptr = value_of_variable (sym, NULL);

	  break;

	case LOC_STATIC:
	case LOC_LABEL:
	  addr = SYMBOL_VALUE_ADDRESS (sym);
	  break;
	}
    }
  else
    {
      struct minimal_symbol *msymbol;
      struct type *type;

      /* Try to find a minimal symbol for NAME.  */

      msymbol = lookup_minimal_symbol (name, "", (struct objfile *) NULL);
      if (msymbol != NULL)
	{
	  addr = SYMBOL_VALUE_ADDRESS (msymbol);
	  if (val_ptrptr != NULL)
	    {
	      type = lookup_pointer_type (builtin_type_char);
	      type = lookup_function_type (type);
	      type = lookup_pointer_type (type);
	      *val_ptrptr = value_from_longest (type, addr);
	    }
	}
    }
  return addr;
}

/* The following two routines adapt GDB's framework for stepping over
   shared library trampoline code to the problem of stepping over the
   Magic Cap method dispatcher.  While the method dispatcher is not a
   shared library trampoline, we can use the interfaces for controlling
   stepping over trampolines to do what we want.  */

/* Return nonzero if STOP_PC is within the Magic Cap method dispatcher.
   NAME is unused.  This function serves as the implementation of both
   IN_SOLIB_CALL_TRAMPOLINE() and IN_SOLIB_RETURN_TRAMPOLINE() when GDB
   is configured to target Magic Cap.  We don't need to distinguish
   between the two types of trampolines (because they're not really
   trampolines); we just need to tell GDB to set a breakpoint at the
   site of the next "hop" on our way through the dispatcher, and to
   keep going.  */

int
magic_in_dispatcher (stop_pc, name)
     CORE_ADDR stop_pc;
     char *name;
{
  return magic_step_enabled
	 && (is_dispatcher_entry (stop_pc) || is_dispatcher_exit (stop_pc));
}

/* Determine if STOP_PC is an address within the Magic Cap method
   dispatcher, and if so, return the address at which GDB should set
   a step resume breakpoint in order to skip over the dispatcher code.
   In fact, we have to skip over the dispatcher in two separate "hops:"
   the first hop gets us from a dispatcher entry point to the dispatcher
   exit site; the second hop gets us from this exit site to the first
   instruction of the method.

   This function serves as the implementation of SKIP_TRAMPOLINE_CODE()
   when GDB is configured to target Magic Cap.  */

CORE_ADDR 
magic_skip_dispatcher (stop_pc)
     CORE_ADDR stop_pc;
{
  /* If magic stepping is disabled, return 0, indicating that GDB should
     process this step event normally.  This will have the effect of
     allowing the user to step through the dispatcher code itself.  */

  if (!magic_step_enabled)
    return 0;

  /* If the program is stopped at an entry point to the dispatcher,
     tell GDB to set a breakpoint at a well-known label in the
     dispatcher where we will be able to determine the address of
     the method to which we are dispatching.  Note that the dispatcher
     has hair to ensure that the code at this label is executed when we
     are completing a top-level dispatch; recursive dispatches generated
     from within the dispatcher do not exit through this code.  */

  if (is_dispatcher_entry (stop_pc))
    return DO_DISPATCH_METHOD_ADDR;

  /* If we have hit the breakpoint set previously at a dispatcher exit site,
     determine the method address and tell GDB to set a breakpoint there.  */

  else if (is_dispatcher_exit (stop_pc))
    return read_register (14);	/* assumes that we branch through t6 */
  else
    return 0;
}

/* Return nonzero if PC is an entry point to the Magic Cap method
   dispatcher.  */

static int
is_dispatcher_entry (pc)
     CORE_ADDR pc;
{
  return pc == DISPATCH_METHOD_ADDR
	 || pc == DISPATCH_INTRINSIC_ADDR
	 || pc == DISPATCH_INHERITED_ADDR
	 || pc == DISPATCH_DELEGATED_ADDR;
}

/* Return nonzero if PC is an exit site from the Magic Cap method
   dispatcher.  */

static int
is_dispatcher_exit (pc)
     CORE_ADDR pc;
{
  return pc == DO_DISPATCH_METHOD_ADDR;
}

/* Store away addresses in the inferior we need to control single-stepping
   through Magic Cap method dispatches, as well as other addresses of
   interest in Magic Cap.  */

void
init_magic ()
{
  struct text_label *label;
  int i;

  /* Cache method dispatch label addresses.  */

  for (i = 0, label = dispatch_labels;
       i < ARRAY_NELEMENTS (dispatch_labels);
       i++, label++)
    {
      if (!(label->addr = lookup_text_label (label->name, NULL)))
	{
	  /* If we can't find all of the dispatcher addresses, don't attempt
	     to do magic stepping.  */

	  magic_step_enabled = 0;
	  break;
	}
    }

  /* Cache value objects for RemoteGetClassName () and RemoteGetBasePtr (),
     which are used to implement the "dobj" and "pobj" commands.  Note that
     we must call release_value () on these values to prevent GDB from freeing
     them automatically.  */

  if (remote_get_class_name_val != NULL)
    {
      value_free (remote_get_class_name_val);
      remote_get_class_name_val = NULL;
    }

  if (remote_get_base_ptr_val != NULL)
    {
      value_free (remote_get_base_ptr_val);
      remote_get_base_ptr_val = NULL;
    }

  if (lookup_text_label ("RemoteGetClassName", &remote_get_class_name_val))
    {
      release_value (remote_get_class_name_val);

      if (lookup_text_label ("RemoteGetBasePtr", &remote_get_base_ptr_val))
	release_value (remote_get_base_ptr_val);
    }
} 

/* Hook routine called when an inferior (i.e., debugged) process is
   created.  */

void
magic_create_inferior_hook ()
{
  struct symbol *sym = lookup_symbol ("gHandleError", NULL, VAR_NAMESPACE,
				      NULL, NULL);
  if (sym)
    {
      CORE_ADDR addr = SYMBOL_VALUE (sym);
      unsigned long errorDebugger = 2;

      target_write_memory (addr, (char *) &errorDebugger, 4);
    }
}

/* Initialization routine for magic.c.  This is where we define debugger
   commands specific to Magic Cap.  */

void
_initialize_magic ()
{
  add_com ("dobj", class_support, dobj_command,
  	   "Display object contents.\n\
Usage: dobj <objectID>\n\
Where: <objectID> is an expression for the object ID to dump.");

  add_com ("pobj", class_support, pobj_command,
  	   "Print object base pointer.\n\
Usage: pobj <objectID>\n\
Where: <objectID> is an expression for the object ID to examine.");

  add_com ("cdump", class_support, cdump_command,
  	   concat ("Display the contents of a cluster.\n\
Usage: cdump <contextSlot> [/l[ocked]] [/s[tartAddr] <addr>]\n\
			   [/c[lass] <classNumber>]\n\
Where: <contextSlot> is an expression describing the cluster to dump;\n\
           if <contextSlot> is a number between 0x8 and 0xf, it is \n\
           interpreted as the high-order nibble of an object ID\n\
           belonging to the cluster to dump, with the second highest-\n\
           order nibble assumed to be 0.  (For example, \"cdump 8\" and \n\
           \"cdump 0xa\" dump the System Persistent and Persistent RAM\n\
           clusters, respectively.)\n",
                   "\n\
           if <contextSlot> is a number between 0xf0 and 0x100, it is\n\
           interpreted as the high-order byte of an object ID belonging to\n\
           the cluster to dump.  (For example, \"cdump 0x88\" and \n\
           \"cdump 0xa8\" dump the Locked Persistent and Transient RAM\n\
           clusters, respectively.)\n",
                   "\n\
       /locked or /l indicates that only locked objects are to be displayed.\n\
\n\
       /startAddr or /s indicates that only objects whose base pointers are\n\
           greater than or equal to the address specified by the following\n\
           expression (<startAddr>) are to be displayed.\n\
\n\
       /class or /c indicates that only objects of the class specified by\n\
           the following expression <classNumber> are to be displayed.",
	           NULL));

  add_com ("esc", class_support, esc_command,
	   "List all the exception handlers for a given actor.\n\
Usage: esc <objectID>\n\
Where: <objectID> is an expression for the object ID of the actor\n\
           whose exception handlers are to be listed.");

  add_com ("cnum", class_support, cnum_command,
  	   "Convert class number to name.\n\
Usage: cnum <classNumber>\n\
Where: <classNumber> is an expression for the class number to convert.");

  add_com ("getint", class_support, getint_command,
  	   "Convert intrinsic name to number or vice versa.\n\
Usage: getint <intrinsicName> | <intrinsicNumber>\n\
Where: <intrinsicName> | <intrinsicNumber> is an intrinsic operation name\n\
           to be converted to an operation number, or an intrinsic operation\n\
           number to be converted to an operation name.");

  add_com ("getop", class_support, getop_command,
  	   "Convert operation name to number or vice versa.\n\
Usage: getop <operationName> | <operationNumber>\n\
Where: <operationName> | <operationNumber> is an operation name to be\n\
           converted to an operation number, or an operation number to\n\
           be converted to an operation name.");

  add_com ("getindexical", class_support, getindexical_command,
	   "Convert indexical name to number or vice versa.\n\
Usage: getindexical <indexicalName> | <indexicalNumber>\n\
Where: <indexicalName> | <indexicalNumber> is an indexical name to be\n\
           converted to an an indexical number, or an indexical number\n\
           to be converted to an indexical name.");

  add_com ("exc", class_support, exc_command,
	   "Convert exception name to number or vice versa.\n\
Usage: exc <exceptionName> | <exceptionNumber>\n\
Where: <exceptionName> | <exceptionNumber> is an exception name to be\n\
           converted to an an exception number, or an exception number\n\
           to be converted to an exception name.");

  add_show_from_set
    (add_set_cmd ("extra-data-max", class_support, var_uinteger,
		  (char *) &extra_data_max,
		  "Set limit on number of bytes of extra data to print.\n\
This command sets an upper limit on the number of bytes of extra\n\
data displayed by the \"dobj\" command when dumping a Magic Cap\n\
object.  \"set extra-data-max 0\" causes there to be no limit.",
		  &setlist),
     &showlist);

  extra_data_max = EXTRA_DATA_MAX_DEFAULT;

  add_show_from_set
    (add_set_cmd ("magic-step", class_support, var_boolean,
		  (char *) &magic_step_enabled,
		  "Set stepping over Magic Cap method dispatches.\n\
When set to \"on\" (the default), issuing a \"step\" command at a Magic Cap\n\
operation call site will cause the program to stop at the first line of\n\
the corresponding method.  Set this to \"off\" only if you need to debug\n\
the dispatcher itself.",
		  &setlist),
     &showlist);

  magic_step_enabled = 1;

#ifdef DYNAMIC_COMMAND_SUPPORT
  add_com ("dlopen", class_support, dlopen_command,
	 "Load the dynamic library specified and execute the specified symbol");
#endif
}
