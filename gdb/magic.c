#include "defs.h"
#include "gdbcmd.h"
#include "symtab.h"
#include "value.h"
#include <ctype.h>
#include <string.h>
#ifdef DYNAMIC_COMMAND_SUPPORT
#include <dlfcn.h>
#endif

typedef unsigned long ulong;

#ifdef DYNAMIC_COMMAND_SUPPORT
static void
dlopen_command PARAMS ((char *, int));
#endif

#ifdef DYNAMIC_COMMAND_SUPPORT
/* ARGSUSED */
static void
dlopen_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	char *p;
	void *hdl;
	void (*sym)();

	if (arg == 0) {
		error ("No arguments specified.");
		return;
	}
	p = arg;
	while(*p != ' ' && *p != '\0')
		p++;
	if (*p != ' ') {
		error ("Not enough arguments.");
		return;
	}
    *p++ = '\0';

	hdl = dlopen(arg, RTLD_NOW);
	if (hdl == NULL) {
		fprintf(stderr, "%s: %s\n", arg, dlerror());
		return;
	}
	sym = dlsym(hdl, p);

	if (sym == NULL) {
		fprintf(stderr, "%s: %s\n", p, dlerror());
		return;
	}

	sym();
}
#endif

static void
local_shell_escape (char *arg)
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

static void
GetClassName(long objectID, char* name)
{
  register value_ptr val;
  register struct symbol *sym;
  struct minimal_symbol *msymbol;
  struct type *type;
  value_ptr blocklen;
  LONGEST maddr;

  /* Find the address of RemoteGetClassName in the inferior.  */

  sym = lookup_symbol ("RemoteGetClassName", 0, VAR_NAMESPACE, 0, NULL);
  if (sym != NULL)
    {
      if (SYMBOL_CLASS (sym) != LOC_BLOCK)
	{
	  error ("\"RemoteGetClassName\" exists in this program but is not a function.");
	}
      val = value_of_variable (sym, NULL);
    }
  else
    {
      msymbol = lookup_minimal_symbol ("RemoteGetClassName", "", (struct objfile *) NULL);
      if (msymbol != NULL)
	{
	  type = lookup_pointer_type (builtin_type_char);
	  type = lookup_function_type (type);
	  type = lookup_pointer_type (type);
	  maddr = (LONGEST) SYMBOL_VALUE_ADDRESS (msymbol);
	  val = value_from_longest (type, maddr);
	}
      else
	{
	  error ("evaluation of this expression requires the program to have a function \"RemoteGetClassName\".");
	}
    }

  blocklen = value_from_longest (builtin_type_int, (LONGEST) objectID);
  val = call_function_by_hand (val, 1, &blocklen);
  if (value_logical_not (val))
    {
      error ("Could not get class name.");
    }
	read_memory(value_as_pointer(val), name, 32);
	
}

static CORE_ADDR
GetBasePtr(long objectID)
{
  register value_ptr val;
  register struct symbol *sym;
  struct minimal_symbol *msymbol;
  struct type *type;
  value_ptr blocklen;
  LONGEST maddr;

  /* Find the address of RemoteGetBasePtr in the inferior.  */

  sym = lookup_symbol ("RemoteGetBasePtr", 0, VAR_NAMESPACE, 0, NULL);
  if (sym != NULL)
    {
      if (SYMBOL_CLASS (sym) != LOC_BLOCK)
	{
	  error ("\"RemoteGetBasePtr\" exists in this program but is not a function.");
	}
      val = value_of_variable (sym, NULL);
    }
  else
    {
      msymbol = lookup_minimal_symbol ("RemoteGetBasePtr", "",  (struct objfile *) NULL);
      if (msymbol != NULL)
	{
	  type = lookup_pointer_type (builtin_type_char);
	  type = lookup_function_type (type);
	  type = lookup_pointer_type (type);
	  maddr = (LONGEST) SYMBOL_VALUE_ADDRESS (msymbol);
	  val = value_from_longest (type, maddr);
	}
      else
	{
	  error ("evaluation of this expression requires the program to have a function \"RemoteGetBasePtr\".");
	}
    }

  blocklen = value_from_longest (builtin_type_int, (LONGEST) objectID);
  val = call_function_by_hand (val, 1, &blocklen);
  if (value_logical_not (val))
    {
      error ("Could not get base pointer to object.");
    }
	return value_as_pointer(val);
}

static void
dump_extra_data(CORE_ADDR addr, ulong length)
{
	ulong buf[5], chunk, i;
	char *p;

	while (length > 3) {
		chunk = (length > 16) ? 16 : length;

		memset(buf, 0, 5*sizeof(long));
		read_memory(addr, &buf, chunk);
		fprintf(gdb_stdout, "%08lx %08lx %08lx %08lx | ", buf[0], 
			buf[1], buf[2], buf[3]);
		for (i = 0, p = (char*)buf; i < chunk; i++, p++) {
			if (!isprint(*p))
				*p = '.';
		}
		fprintf(gdb_stdout, "%s |\n", buf);
		addr += chunk;
		length -= chunk;
	}
}

struct type *type_of_object(CORE_ADDR object)
{
	char className[32], classAllFieldsName[128];
	struct type *type = NULL;
	GetClassName(object, className);
	sprintf(classAllFieldsName, "%s_AllFields", className);

	type = lookup_typename(classAllFieldsName, (struct block *)NULL, 0);
	return lookup_pointer_type(type);
}

CORE_ADDR baseptr_of_object(ulong object)
{
	return GetBasePtr(object) + 12;
}

/* ARGSUSED */
static void
print_object (arg, dump)
     char *arg;
	 int dump;
{
	CORE_ADDR addr;
	ulong object, objectLength, typeLength = 0;
	char className[32], classAllFieldsName[128];
	struct type* type = NULL;

	object = parse_and_eval_address(arg);

	GetClassName(object, className);
	sprintf(classAllFieldsName, "%s_AllFields", className);

	type = lookup_typename(classAllFieldsName, (struct block *)NULL, 0);
	typeLength = TYPE_LENGTH(type);
	addr = GetBasePtr(object);
	read_memory(addr, &objectLength, 4);
	objectLength -= 12;
	addr += 12;
	if (TYPE_CODE(type) != TYPE_CODE_UNDEF && !(TYPE_FLAGS(type)&TYPE_FLAG_STUB)) {
		if (dump) {
			value_ptr valptr = value_at_lazy(type, addr);
			int histindex = record_latest_value(valptr);
			printf_filtered("Object 0x%08lx at address 0x%08lx of class %s\n", 
					object, addr, className);
			if (histindex >= 0) printf_filtered ("$%d = ", histindex);
			value_print(valptr, gdb_stdout, 0, Val_prettyprint);
			objectLength -= typeLength;
			addr += typeLength;
			printf_filtered("\n");
			dump_extra_data(addr, objectLength);
			printf_filtered("\n");
		} else {
			value_ptr valptr = value_from_longest(lookup_pointer_type(type), addr);
			int histindex = record_latest_value(valptr);
			if (histindex >= 0) printf_filtered ("$%d = ", histindex);
			value_print(valptr, gdb_stdout, 0, Val_prettyprint);
			printf_filtered("\n");
		}
	}
}

/* ARGSUSED */
static void
dobj_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	print_object(arg, 1);
}

/* ARGSUSED */
static void
pobj_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	print_object(arg, 0);
}

/* ARGSUSED */
static void
getint_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	char shellCommand[128];
	
	sprintf(shellCommand, "getint %s", arg);
	local_shell_escape(shellCommand);
}

/* ARGSUSED */
static void
getindexical_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	char shellCommand[128];

	sprintf(shellCommand, "getindexical %s", arg);
	local_shell_escape(shellCommand);
}

/* ARGSUSED */
static void
exc_command (arg, from_tty)
     char *arg;
     int from_tty;
{
	char shellCommand[128];
	ulong exception;
	
	sprintf(shellCommand, "getexc %s", arg);
	local_shell_escape(shellCommand);
}

static CORE_ADDR dispatch_method_addr = -1, dispatch_inherited_addr = -1, dispatch_delegated_addr = -1, dispatch_intrinsic_addr = -1;
CORE_ADDR do_dispatch_method_addr = -1, do_dispatch_intrinsic_addr = -1;

static CORE_ADDR
lookup_address(const char *name)
{
  struct symbol *sym = lookup_symbol(name, NULL, VAR_NAMESPACE, NULL, NULL);
  if (sym)
    return BLOCK_START(SYMBOL_BLOCK_VALUE(sym));
  else
    {
/*      printf("Couldn't find %s!\n", name); */
      return -1;
    }
}

void
init_magic()
{
  dispatch_method_addr = lookup_address("__DispatchMethod");
  dispatch_inherited_addr = lookup_address("__DispatchInherited");
  dispatch_delegated_addr = lookup_address("__DispatchDelegated");
  dispatch_intrinsic_addr = lookup_address("__DispatchIntrinsic");
  do_dispatch_method_addr = lookup_address("__DoTheDispatch");
  do_dispatch_intrinsic_addr = lookup_address("__DoDispatchIntrinsic");
} 

int
is_dispatch(CORE_ADDR pc)
{
  return (pc == dispatch_method_addr) || (pc == dispatch_inherited_addr) || (pc == dispatch_delegated_addr);
}

int
is_dispatch_intrinsic(CORE_ADDR pc)
{
  return pc == dispatch_intrinsic_addr;
}

/* If we are stopped at one of the entry points to the dispatcher, we want to continue until just
   before we jump to the implementation.  If we are at that point, we want to continue until we
   actually get to the implementation.  Likewise for the intrinsic dispatcher
   */
CORE_ADDR 
deal_with_dispatch(CORE_ADDR stop_pc)
{
  if (is_dispatch(stop_pc))
    return do_dispatch_method_addr;
  else if (is_dispatch_intrinsic(stop_pc))
    return do_dispatch_intrinsic_addr;
  else if (stop_pc == do_dispatch_method_addr)
    /* This assumes that we branch through t6 */
    return read_register(14);
  else if (stop_pc == do_dispatch_intrinsic_addr)
    /* This assumes that we branch through t0 */
    return read_register(8);
  else
    return 0;
}

void
magic_create_inferior_hook()
{
	struct symbol *sym = lookup_symbol("gHandleError", NULL, VAR_NAMESPACE, NULL, NULL);
	if (sym)
		{
			CORE_ADDR addr = SYMBOL_VALUE(sym);
			unsigned long errorDebugger = 2;
			target_write_memory(addr, &errorDebugger, 4);
		}

	init_magic ();
}


_initialize_magic ()
{
	add_com ("dobj", class_support, dobj_command, "Display Object Contents");
	add_com ("pobj", class_support, pobj_command, "Print object base pointer");
	add_com ("getint", class_support, getint_command, "Convert intrinsic name to number or vice versa.");
	add_com ("getindexical", class_support, getindexical_command, "Convert indexical name to number or vice versa.");
	add_com ("exc", class_support, exc_command, "Convert exception name to number or vice versa.");

#ifdef DYNAMIC_COMMAND_SUPPORT
  add_com ("dlopen", class_support, dlopen_command,
	   "Load the dynamic library specified and execute the specified symbol");
#endif
}
