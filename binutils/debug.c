/* debug.c -- Handle generic debugging information.
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file implements a generic debugging format.  We may eventually
   have readers which convert different formats into this generic
   format, and writers which write it out.  The initial impetus for
   this was writing a convertor from stabs to HP IEEE-695 debugging
   format.  */

#include <stdio.h>
#include <assert.h>

#include "bfd.h"
#include "bucomm.h"
#include "libiberty.h"
#include "debug.h"

/* Global information we keep for debugging.  A pointer to this
   structure is the debugging handle passed to all the routines.  */

struct debug_handle
{
  /* A linked list of compilation units.  */
  struct debug_unit *units;
  /* The current compilation unit.  */
  struct debug_unit *current_unit;
  /* The current source file.  */
  struct debug_file *current_file;
  /* The current function.  */
  struct debug_function *current_function;
  /* The current block.  */
  struct debug_block *current_block;
  /* The current line number information for the current block.  */
  struct debug_lineno *current_lineno;
  /* Mark.  This is used by debug_write.  */
  unsigned int mark;
  /* Another mark used by debug_write.  */
  unsigned int class_mark;
};

/* Information we keep for a single compilation unit.  */

struct debug_unit
{
  /* The next compilation unit.  */
  struct debug_unit *next;
  /* A list of files included in this compilation unit.  The first
     file is always the main one, and that is where the main file name
     is stored.  */
  struct debug_file *files;
};

/* Information kept for a single source file.  */

struct debug_file
{
  /* The next source file in this compilation unit.  */
  struct debug_file *next;
  /* The name of the source file.  */
  const char *filename;
  /* Global functions, variables, types, etc.  */
  struct debug_namespace *globals;
};

/* A type.  */

struct debug_type
{
  /* Kind of type.  */
  enum debug_type_kind kind;
  /* Size of type (0 if not known).  */
  unsigned int size;
  /* Type which is a pointer to this type.  */
  debug_type pointer;
  /* Tagged union with additional information about the type.  */
  union
    {
      /* DEBUG_KIND_INDIRECT.  */
      struct debug_indirect_type *kindirect;
      /* DEBUG_KIND_INT.  */
      /* Whether the integer is unsigned.  */
      boolean kint;
      /* DEBUG_KIND_STRUCT, DEBUG_KIND_UNION, DEBUG_KIND_CLASS,
         DEBUG_KIND_UNION_CLASS.  */
      struct debug_class_type *kclass;
      /* DEBUG_KIND_ENUM.  */
      struct debug_enum_type *kenum;
      /* DEBUG_KIND_POINTER.  */
      struct debug_type *kpointer;
      /* DEBUG_KIND_FUNCTION.  */
      struct debug_function_type *kfunction;
      /* DEBUG_KIND_REFERENCE.  */
      struct debug_type *kreference;
      /* DEBUG_KIND_RANGE.  */
      struct debug_range_type *krange;
      /* DEBUG_KIND_ARRAY.  */
      struct debug_array_type *karray;
      /* DEBUG_KIND_SET.  */
      struct debug_set_type *kset;
      /* DEBUG_KIND_OFFSET.  */
      struct debug_offset_type *koffset;
      /* DEBUG_KIND_METHOD.  */
      struct debug_method_type *kmethod;
      /* DEBUG_KIND_CONST.  */
      struct debug_type *kconst;
      /* DEBUG_KIND_VOLATILE.  */
      struct debug_type *kvolatile;
      /* DEBUG_KIND_NAMED, DEBUG_KIND_TAGGED.  */
      struct debug_named_type *knamed;
    } u;
};

/* Information kept for an indirect type.  */

struct debug_indirect_type
{
  /* Slot where the final type will appear.  */
  debug_type *slot;
  /* Tag.  */
  const char *tag;
};

/* Information kept for a struct, union, or class.  */

struct debug_class_type
{
  /* NULL terminated array of fields.  */
  debug_field *fields;
  /* A mark field used to avoid recursively printing out structs.  */
  unsigned int mark;
  /* The remaining fields are only used for DEBUG_KIND_CLASS and
     DEBUG_KIND_UNION_CLASS.  */
  /* NULL terminated array of base classes.  */
  debug_baseclass *baseclasses;
  /* NULL terminated array of methods.  */
  debug_method *methods;
  /* The type of the class providing the virtual function table for
     this class.  This may point to the type itself.  */
  debug_type vptrbase;
};

/* Information kept for an enum.  */

struct debug_enum_type
{
  /* NULL terminated array of names.  */
  const char **names;
  /* Array of corresponding values.  */
  bfd_signed_vma *values;
};

/* Information kept for a function.  FIXME: We should be able to
   record the parameter types.  */

struct debug_function_type
{
  /* Return type.  */
  debug_type return_type;
};

/* Information kept for a range.  */

struct debug_range_type
{
  /* Range base type.  */
  debug_type type;
  /* Lower bound.  */
  bfd_signed_vma lower;
  /* Upper bound.  */
  bfd_signed_vma upper;
};

/* Information kept for an array.  */

struct debug_array_type
{
  /* Element type.  */
  debug_type element_type;
  /* Range type.  */
  debug_type range_type;
  /* Lower bound.  */
  bfd_signed_vma lower;
  /* Upper bound.  */
  bfd_signed_vma upper;
  /* Whether this array is really a string.  */
  boolean stringp;
};

/* Information kept for a set.  */

struct debug_set_type
{
  /* Base type.  */
  debug_type type;
  /* Whether this set is really a bitstring.  */
  boolean bitstringp;
};

/* Information kept for an offset type (a based pointer).  */

struct debug_offset_type
{
  /* The type the pointer is an offset from.  */
  debug_type base_type;
  /* The type the pointer points to.  */
  debug_type target_type;
};

/* Information kept for a method type.  */

struct debug_method_type
{
  /* The return type.  */
  debug_type return_type;
  /* The object type which this method is for.  */
  debug_type domain_type;
  /* A NULL terminated array of argument types.  */
  debug_type *arg_types;
};

/* Information kept for a named type.  */

struct debug_named_type
{
  /* Name.  */
  struct debug_name *name;
  /* Real type.  */
  debug_type type;
};

/* A field in a struct or union.  */

struct debug_field
{
  /* Name of the field.  */
  const char *name;
  /* Type of the field.  */
  struct debug_type *type;
  /* Visibility of the field.  */
  enum debug_visibility visibility;
  /* Whether this is a static member.  */
  boolean static_member;
  union
    {
      /* If static_member is false.  */
      struct
	{
	  /* Bit position of the field in the struct.  */
	  unsigned int bitpos;
	  /* Size of the field in bits.  */
	  unsigned int bitsize;
	} f;
      /* If static_member is true.  */
      struct
	{
	  const char *physname;
	} s;
    } u;
};

/* A base class for an object.  */

struct debug_baseclass
{
  /* Type of the base class.  */
  struct debug_type *type;
  /* Bit position of the base class in the object.  */
  unsigned int bitpos;
  /* Whether the base class is virtual.  */
  boolean virtual;
  /* Visibility of the base class.  */
  enum debug_visibility visibility;
};

/* A method of an object.  */

struct debug_method
{
  /* The name of the method.  */
  const char *name;
  /* A NULL terminated array of different types of variants.  */
  struct debug_method_variant **variants;
};

/* The variants of a method function of an object.  These indicate
   which method to run.  */

struct debug_method_variant
{
  /* The argument types of the function.  */
  const char *argtypes;
  /* The type of the function.  */
  struct debug_type *type;
  /* The visibility of the function.  */
  enum debug_visibility visibility;
  /* Whether the function is const.  */
  boolean constp;
  /* Whether the function is volatile.  */
  boolean volatilep;
  /* The offset to the function in the virtual function table.  */
  bfd_vma voffset;
  /* If voffset is VOFFSET_STATIC_METHOD, this is a static method.  */
#define VOFFSET_STATIC_METHOD (1)
  /* Context of a virtual method function.  */
  struct debug_type *context;
};

/* A variable.  This is the information we keep for a variable object.
   This has no name; a name is associated with a variable in a
   debug_name structure.  */

struct debug_variable
{
  /* Kind of variable.  */
  enum debug_var_kind kind;
  /* Type.  */
  debug_type type;
  /* Value.  The interpretation of the value depends upon kind.  */
  bfd_vma val;
};

/* A function.  This has no name; a name is associated with a function
   in a debug_name structure.  */

struct debug_function
{
  /* Return type.  */
  debug_type return_type;
  /* Parameter information.  */
  struct debug_parameter *parameters;
  /* Block information.  The first structure on the list is the main
     block of the function, and describes function local variables.  */
  struct debug_block *blocks;
};

/* A function parameter.  */

struct debug_parameter
{
  /* Next parameter.  */
  struct debug_parameter *next;
  /* Name.  */
  const char *name;
  /* Type.  */
  debug_type type;
  /* Kind.  */
  enum debug_parm_kind kind;
  /* Value (meaning depends upon kind).  */
  bfd_vma val;
};

/* A typed constant.  */

struct debug_typed_constant
{
  /* Type.  */
  debug_type type;
  /* Value.  FIXME: We may eventually need to support non-integral
     values.  */
  bfd_vma val;
};

/* Information about a block within a function.  */

struct debug_block
{
  /* Next block with the same parent.  */
  struct debug_block *next;
  /* Parent block.  */
  struct debug_block *parent;
  /* List of child blocks.  */
  struct debug_block *children;
  /* Start address of the block.  */
  bfd_vma start;
  /* End address of the block.  */
  bfd_vma end;
  /* Local variables.  */
  struct debug_namespace *locals;
  /* Line number information.  */
  struct debug_lineno *linenos;
};

/* Line number information we keep for a function.  FIXME: This
   structure is easy to create, but can be very space inefficient.  */

struct debug_lineno
{
  /* More line number information for this block.  */
  struct debug_lineno *next;
  /* Source file.  */
  struct debug_file *file;
  /* Line numbers, terminated by a -1 or the end of the array.  */
#define DEBUG_LINENO_COUNT 10
  unsigned long linenos[DEBUG_LINENO_COUNT];
  /* Addresses for the line numbers.  */
  bfd_vma addrs[DEBUG_LINENO_COUNT];
};

/* A namespace.  This is a mapping from names to objects.  FIXME: This
   should be implemented as a hash table.  */

struct debug_namespace
{
  /* List of items in this namespace.  */
  struct debug_name *list;
  /* Pointer to where the next item in this namespace should go.  */
  struct debug_name **tail;
};

/* Kinds of objects that appear in a namespace.  */

enum debug_object_kind
{
  /* A type.  */
  DEBUG_OBJECT_TYPE,
  /* A tagged type (really a different sort of namespace).  */
  DEBUG_OBJECT_TAG,
  /* A variable.  */
  DEBUG_OBJECT_VARIABLE,
  /* A function.  */
  DEBUG_OBJECT_FUNCTION,
  /* An integer constant.  */
  DEBUG_OBJECT_INT_CONSTANT,
  /* A floating point constant.  */
  DEBUG_OBJECT_FLOAT_CONSTANT,
  /* A typed constant.  */
  DEBUG_OBJECT_TYPED_CONSTANT
};

/* Linkage of an object that appears in a namespace.  */

enum debug_object_linkage
{
  /* Local variable.  */
  DEBUG_LINKAGE_AUTOMATIC,
  /* Static--either file static or function static, depending upon the
     namespace is.  */
  DEBUG_LINKAGE_STATIC,
  /* Global.  */
  DEBUG_LINKAGE_GLOBAL,
  /* No linkage.  */
  DEBUG_LINKAGE_NONE
};

/* A name in a namespace.  */

struct debug_name
{
  /* Next name in this namespace.  */
  struct debug_name *next;
  /* Name.  */
  const char *name;
  /* Mark.  This is used by debug_write.  */
  unsigned int mark;
  /* Kind of object.  */
  enum debug_object_kind kind;
  /* Linkage of object.  */
  enum debug_object_linkage linkage;
  /* Tagged union with additional information about the object.  */
  union
    {
      /* DEBUG_OBJECT_TYPE.  */
      struct debug_type *type;
      /* DEBUG_OBJECT_TAG.  */
      struct debug_type *tag;
      /* DEBUG_OBJECT_VARIABLE.  */
      struct debug_variable *variable;
      /* DEBUG_OBJECT_FUNCTION.  */
      struct debug_function *function;
      /* DEBUG_OBJECT_INT_CONSTANT.  */
      bfd_vma int_constant;
      /* DEBUG_OBJECT_FLOAT_CONSTANT.  */
      double float_constant;
      /* DEBUG_OBJECT_TYPED_CONSTANT.  */
      struct debug_typed_constant *typed_constant;
    } u;
};

static void debug_error PARAMS ((const char *));
static struct debug_name *debug_add_to_namespace
  PARAMS ((struct debug_handle *, struct debug_namespace **, const char *,
	   enum debug_object_kind, enum debug_object_linkage));
static struct debug_name *debug_add_to_current_namespace
  PARAMS ((struct debug_handle *, const char *, enum debug_object_kind,
	   enum debug_object_linkage));
static struct debug_type *debug_make_type
  PARAMS ((struct debug_handle *, enum debug_type_kind, unsigned int));
static boolean debug_write_name
  PARAMS ((struct debug_handle *, const struct debug_write_fns *, PTR,
	   struct debug_name *));
static boolean debug_write_type
  PARAMS ((struct debug_handle *, const struct debug_write_fns *, PTR,
	   struct debug_type *, struct debug_name *));
static boolean debug_write_class_type
  PARAMS ((struct debug_handle *, const struct debug_write_fns *, PTR,
	   struct debug_type *));
static boolean debug_write_function
  PARAMS ((struct debug_handle *, const struct debug_write_fns *, PTR,
	   const char *, enum debug_object_linkage, struct debug_function *));
static boolean debug_write_block
  PARAMS ((struct debug_handle *, const struct debug_write_fns *, PTR,
	   struct debug_block *));

/* Issue an error message.  */

static void
debug_error (message)
     const char *message;
{
  fprintf (stderr, "%s\n", message);
}

/* Add an object to a namespace.  */

static struct debug_name *
debug_add_to_namespace (info, nsp, name, kind, linkage)
     struct debug_handle *info;
     struct debug_namespace **nsp;
     const char *name;
     enum debug_object_kind kind;
     enum debug_object_linkage linkage;
{
  struct debug_name *n;
  struct debug_namespace *ns;

  n = (struct debug_name *) xmalloc (sizeof *n);
  memset (n, 0, sizeof *n);

  n->name = name;
  n->kind = kind;
  n->linkage = linkage;

  ns = *nsp;
  if (ns == NULL)
    {
      ns = (struct debug_namespace *) xmalloc (sizeof *ns);
      memset (ns, 0, sizeof *ns);

      ns->tail = &ns->list;

      *nsp = ns;
    }

  *ns->tail = n;
  ns->tail = &n->next;

  return n;
}

/* Add an object to the current namespace.  */

static struct debug_name *
debug_add_to_current_namespace (info, name, kind, linkage)
     struct debug_handle *info;
     const char *name;
     enum debug_object_kind kind;
     enum debug_object_linkage linkage;
{
  struct debug_namespace **nsp;

  if (info->current_unit == NULL
      || info->current_file == NULL)
    {
      debug_error ("debug_add_to_current_namespace: no current file");
      return NULL;
    }

  if (info->current_block != NULL)
    nsp = &info->current_block->locals;
  else
    nsp = &info->current_file->globals;

  return debug_add_to_namespace (info, nsp, name, kind, linkage);
}

/* Return a handle for debugging information.  */

PTR
debug_init ()
{
  struct debug_handle *ret;

  ret = (struct debug_handle *) xmalloc (sizeof *ret);
  memset (ret, 0, sizeof *ret);
  return (PTR) ret;
}

/* Set the source filename.  This implicitly starts a new compilation
   unit.  */

boolean
debug_set_filename (handle, name)
     PTR handle;
     const char *name;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_file *nfile;
  struct debug_unit *nunit;

  if (name == NULL)
    name = "";

  nfile = (struct debug_file *) xmalloc (sizeof *nfile);
  memset (nfile, 0, sizeof *nfile);

  nfile->filename = name;

  nunit = (struct debug_unit *) xmalloc (sizeof *nunit);
  memset (nunit, 0, sizeof *nunit);

  nunit->files = nfile;
  info->current_file = nfile;

  if (info->current_unit != NULL)
    info->current_unit->next = nunit;
  else
    {
      assert (info->units == NULL);
      info->units = nunit;
    }

  info->current_unit = nunit;

  info->current_function = NULL;
  info->current_block = NULL;
  info->current_lineno = NULL;

  return true;
}

/* Append a string to the source filename.  */

boolean
debug_append_filename (handle, string)
     PTR handle;
     const char *string;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  char *n;

  if (string == NULL)
    string = "";

  if (info->current_unit == NULL)
    {
      debug_error ("debug_append_filename: no current file");
      return false;
    }

  n = (char *) xmalloc (strlen (info->current_unit->files->filename)
			+ strlen (string)
			+ 1);
  sprintf (n, "%s%s", info->current_unit->files->filename, string);
  info->current_unit->files->filename = n;

  return true;
}

/* Change source files to the given file name.  This is used for
   include files in a single compilation unit.  */

boolean
debug_start_source (handle, name)
     PTR handle;
     const char *name;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_file *f, **pf;

  if (name == NULL)
    name = "";

  if (info->current_unit == NULL)
    {
      debug_error ("debug_start_source: no debug_set_filename call");
      return false;
    }

  for (f = info->current_unit->files; f != NULL; f = f->next)
    {
      if (f->filename[0] == name[0]
	  && f->filename[1] == name[1]
	  && strcmp (f->filename, name) == 0)
	{
	  info->current_file = f;
	  return true;
	}
    }

  f = (struct debug_file *) xmalloc (sizeof *f);
  memset (f, 0, sizeof *f);

  f->filename = name;

  for (pf = &info->current_file->next;
       *pf != NULL;
       pf = &(*pf)->next)
    ;
  *pf = f;

  info->current_file = f;

  return true;
}

/* Record a function definition.  This implicitly starts a function
   block.  The debug_type argument is the type of the return value.
   The boolean indicates whether the function is globally visible.
   The bfd_vma is the address of the start of the function.  Currently
   the parameter types are specified by calls to
   debug_record_parameter.  FIXME: There is no way to specify nested
   functions.  FIXME: I don't think there is any way to record where a
   function ends.  */

boolean
debug_record_function (handle, name, return_type, global, addr)
     PTR handle;
     const char *name;
     debug_type return_type;
     boolean global;
     bfd_vma addr;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_function *f;
  struct debug_block *b;
  struct debug_name *n;

  if (name == NULL)
    name = "";
  if (return_type == NULL)
    return false;

  if (info->current_unit == NULL)
    {
      debug_error ("debug_record_function: no debug_set_filename call");
      return false;
    }

  f = (struct debug_function *) xmalloc (sizeof *f);
  memset (f, 0, sizeof *f);

  f->return_type = return_type;

  b = (struct debug_block *) xmalloc (sizeof *b);
  memset (b, 0, sizeof *b);

  b->start = addr;
  b->end = (bfd_vma) -1;

  f->blocks = b;

  info->current_function = f;
  info->current_block = b;
  info->current_lineno = NULL;

  /* FIXME: If we could handle nested functions, this would be the
     place: we would want to use a different namespace.  */
  n = debug_add_to_namespace (info,
			      &info->current_file->globals,
			      name,
			      DEBUG_OBJECT_FUNCTION,
			      (global
			       ? DEBUG_LINKAGE_GLOBAL
			       : DEBUG_LINKAGE_STATIC));
  if (n == NULL)
    return false;

  n->u.function = f;

  return true;
}

/* Record a parameter for the current function.  */

boolean
debug_record_parameter (handle, name, type, kind, val)
     PTR handle;
     const char *name;
     debug_type type;
     enum debug_parm_kind kind;
     bfd_vma val;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_parameter *p, **pp;

  if (name == NULL || type == NULL)
    return false;

  if (info->current_unit == NULL
      || info->current_function == NULL)
    {
      debug_error ("debug_record_parameter: no current function");
      return false;
    }

  p = (struct debug_parameter *) xmalloc (sizeof *p);
  memset (p, 0, sizeof *p);

  p->name = name;
  p->type = type;
  p->kind = kind;
  p->val = val;

  for (pp = &info->current_function->parameters;
       *pp != NULL;
       pp = &(*pp)->next)
    ;
  *pp = p;

  return true;
}

/* End a function.  FIXME: This should handle function nesting.  */

boolean
debug_end_function (handle, addr)
     PTR handle;
     bfd_vma addr;
{
  struct debug_handle *info = (struct debug_handle *) handle;

  if (info->current_unit == NULL
      || info->current_block == NULL
      || info->current_function == NULL)
    {
      debug_error ("debug_end_function: no current function");
      return false;
    }

  if (info->current_block->parent != NULL)
    {
      debug_error ("debug_end_function: some blocks were not closed");
      return false;
    }

  info->current_block->end = addr;

  info->current_function = NULL;
  info->current_block = NULL;
  info->current_lineno = NULL;

  return true;
}

/* Start a block in a function.  All local information will be
   recorded in this block, until the matching call to debug_end_block.
   debug_start_block and debug_end_block may be nested.  The bfd_vma
   argument is the address at which this block starts.  */

boolean
debug_start_block (handle, addr)
     PTR handle;
     bfd_vma addr;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_block *b, **pb;

  /* We must always have a current block: debug_record_function sets
     one up.  */
  if (info->current_unit == NULL
      || info->current_block == NULL)
    {
      debug_error ("debug_start_block: no current block");
      return false;
    }

  b = (struct debug_block *) xmalloc (sizeof *b);
  memset (b, 0, sizeof *b);

  b->parent = info->current_block;
  b->start = addr;
  b->end = (bfd_vma) -1;

  /* This new block is a child of the current block.  */
  for (pb = &info->current_block->children;
       *pb != NULL;
       pb = &(*pb)->next)
    ;
  *pb = b;

  info->current_block = b;
  info->current_lineno = NULL;

  return true;
}

/* Finish a block in a function.  This matches the call to
   debug_start_block.  The argument is the address at which this block
   ends.  */

boolean
debug_end_block (handle, addr)
     PTR handle;
     bfd_vma addr;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_block *parent;

  if (info->current_unit == NULL
      || info->current_block == NULL)
    {
      debug_error ("debug_end_block: no current block");
      return false;
    }

  parent = info->current_block->parent;
  if (parent == NULL)
    {
      debug_error ("debug_end_block: attempt to close top level block");
      return false;
    }

  info->current_block->end = addr;

  info->current_block = parent;
  info->current_lineno = NULL;

  return true;
}

/* Associate a line number in the current source file and function
   with a given address.  */

boolean
debug_record_line (handle, lineno, addr)
     PTR handle;
     unsigned long lineno;
     bfd_vma addr;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_lineno *l;
  unsigned int i;

  if (info->current_unit == NULL
      || info->current_block == NULL)
    {
      debug_error ("debug_record_line: no current block");
      return false;
    }

  l = info->current_lineno;
  if (l != NULL && l->file == info->current_file)
    {
      for (i = 0; i < DEBUG_LINENO_COUNT; i++)
	{
	  if (l->linenos[i] == (unsigned long) -1)
	    {
	      l->linenos[i] = lineno;
	      l->addrs[i] = addr;
	      return true;
	    }
	}
    }

  /* If we get here, then either 1) there is no current_lineno
     structure, which means this is the first line number since we got
     to this block, 2) the current_lineno structure is for a different
     file, or 3) the current_lineno structure is full.  Regardless, we
     want to allocate a new debug_lineno structure, put it in the
     right place, and make it the new current_lineno structure.  */

  l = (struct debug_lineno *) xmalloc (sizeof *l);
  memset (l, 0, sizeof *l);

  l->file = info->current_file;
  l->linenos[0] = lineno;
  l->addrs[0] = addr;
  for (i = 1; i < DEBUG_LINENO_COUNT; i++)
    l->linenos[i] = (unsigned long) -1;

  if (info->current_lineno != NULL)
    info->current_lineno->next = l;
  else
    {
      struct debug_lineno **pl;

      /* We may be back in this block after dealing with child blocks,
         which means we may have some line number information for this
         block even though current_lineno was NULL.  */
      for (pl = &info->current_block->linenos;
	   *pl != NULL;
	   pl = &(*pl)->next)
	;
      *pl = l;
    }

  info->current_lineno = l;

  return true;
}

/* Start a named common block.  This is a block of variables that may
   move in memory.  */

boolean
debug_start_common_block (handle, name)
     PTR handle;
     const char *name;
{
  /* FIXME */
  debug_error ("debug_start_common_block: not implemented");
  return false;
}

/* End a named common block.  */

boolean
debug_end_common_block (handle, name)
     PTR handle;
     const char *name;
{
  /* FIXME */
  debug_error ("debug_end_common_block: not implemented");
  return false;
}

/* Record a named integer constant.  */

boolean
debug_record_int_const (handle, name, val)
     PTR handle;
     const char *name;
     bfd_vma val;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_name *n;

  if (name == NULL)
    return false;

  n = debug_add_to_current_namespace (info, name, DEBUG_OBJECT_INT_CONSTANT,
				      DEBUG_LINKAGE_NONE);
  if (n == NULL)
    return false;

  n->u.int_constant = val;

  return true;
}

/* Record a named floating point constant.  */

boolean
debug_record_float_const (handle, name, val)
     PTR handle;
     const char *name;
     double val;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_name *n;

  if (name == NULL)
    return false;

  n = debug_add_to_current_namespace (info, name, DEBUG_OBJECT_FLOAT_CONSTANT,
				      DEBUG_LINKAGE_NONE);
  if (n == NULL)
    return false;

  n->u.float_constant = val;

  return true;
}

/* Record a typed constant with an integral value.  */

boolean
debug_record_typed_const (handle, name, type, val)
     PTR handle;
     const char *name;
     debug_type type;
     bfd_vma val;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_name *n;
  struct debug_typed_constant *tc;

  if (name == NULL || type == NULL)
    return false;

  n = debug_add_to_current_namespace (info, name, DEBUG_OBJECT_TYPED_CONSTANT,
				      DEBUG_LINKAGE_NONE);
  if (n == NULL)
    return false;

  tc = (struct debug_typed_constant *) xmalloc (sizeof *tc);
  memset (tc, 0, sizeof *tc);

  tc->type = type;
  tc->val = val;

  n->u.typed_constant = tc;

  return true;
}

/* Record a label.  */

boolean
debug_record_label (handle, name, type, addr)
     PTR handle;
     const char *name;
     debug_type type;
     bfd_vma addr;
{
  /* FIXME.  */
  debug_error ("debug_record_label not implemented");
  return false;
}

/* Record a variable.  */

boolean
debug_record_variable (handle, name, type, kind, val)
     PTR handle;
     const char *name;
     debug_type type;
     enum debug_var_kind kind;
     bfd_vma val;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_namespace **nsp;
  enum debug_object_linkage linkage;
  struct debug_name *n;
  struct debug_variable *v;

  if (name == NULL || type == NULL)
    return false;

  if (info->current_unit == NULL
      || info->current_file == NULL)
    {
      debug_error ("debug_record_variable: no current file");
      return false;
    }

  if (kind == DEBUG_GLOBAL || kind == DEBUG_STATIC)
    {
      nsp = &info->current_file->globals;
      if (kind == DEBUG_GLOBAL)
	linkage = DEBUG_LINKAGE_GLOBAL;
      else
	linkage = DEBUG_LINKAGE_STATIC;
    }
  else
    {
      if (info->current_block == NULL)
	{
	  debug_error ("debug_record_variable: no current block");
	  return false;
	}
      nsp = &info->current_block->locals;
      linkage = DEBUG_LINKAGE_AUTOMATIC;
    }

  n = debug_add_to_namespace (info, nsp, name, DEBUG_OBJECT_VARIABLE, linkage);
  if (n == NULL)
    return false;

  v = (struct debug_variable *) xmalloc (sizeof *v);
  memset (v, 0, sizeof *v);

  v->kind = kind;
  v->type = type;
  v->val = val;

  n->u.variable = v;

  return true;  
}

/* Make a type with a given kind and size.  */

/*ARGSUSED*/
static struct debug_type *
debug_make_type (info, kind, size)
     struct debug_handle *info;
     enum debug_type_kind kind;
     unsigned int size;
{
  struct debug_type *t;

  t = (struct debug_type *) xmalloc (sizeof *t);
  memset (t, 0, sizeof *t);

  t->kind = kind;
  t->size = size;

  return t;
}

/* Make an indirect type which may be used as a placeholder for a type
   which is referenced before it is defined.  */

debug_type
debug_make_indirect_type (handle, slot, tag)
     PTR handle;
     debug_type *slot;
     const char *tag;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_indirect_type *i;

  t = debug_make_type (info, DEBUG_KIND_INDIRECT, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  i = (struct debug_indirect_type *) xmalloc (sizeof *i);
  memset (i, 0, sizeof *i);

  i->slot = slot;
  i->tag = tag;

  t->u.kindirect = i;

  return t;
}

/* Make a void type.  There is only one of these.  */

debug_type
debug_make_void_type (handle)
     PTR handle;
{
  struct debug_handle *info = (struct debug_handle *) handle;

  return debug_make_type (info, DEBUG_KIND_VOID, 0);
}

/* Make an integer type of a given size.  The boolean argument is true
   if the integer is unsigned.  */

debug_type
debug_make_int_type (handle, size, unsignedp)
     PTR handle;
     unsigned int size;
     boolean unsignedp;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  t = debug_make_type (info, DEBUG_KIND_INT, size);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  t->u.kint = unsignedp;

  return t;
}

/* Make a floating point type of a given size.  FIXME: On some
   platforms, like an Alpha, you probably need to be able to specify
   the format.  */

debug_type
debug_make_float_type (handle, size)
     PTR handle;
     unsigned int size;
{
  struct debug_handle *info = (struct debug_handle *) handle;

  return debug_make_type (info, DEBUG_KIND_FLOAT, size);
}

/* Make a boolean type of a given size.  */

debug_type
debug_make_bool_type (handle, size)
     PTR handle;
     unsigned int size;
{
  struct debug_handle *info = (struct debug_handle *) handle;

  return debug_make_type (info, DEBUG_KIND_BOOL, size);
}

/* Make a complex type of a given size.  */

debug_type
debug_make_complex_type (handle, size)
     PTR handle;
     unsigned int size;
{
  struct debug_handle *info = (struct debug_handle *) handle;

  return debug_make_type (info, DEBUG_KIND_COMPLEX, size);
}

/* Make a structure type.  The second argument is true for a struct,
   false for a union.  The third argument is the size of the struct.
   The fourth argument is a NULL terminated array of fields.  */

debug_type
debug_make_struct_type (handle, structp, size, fields)
     PTR handle;
     boolean structp;
     bfd_vma size;
     debug_field *fields;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_class_type *c;

  t = debug_make_type (info,
		       structp ? DEBUG_KIND_STRUCT : DEBUG_KIND_UNION,
		       size);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  c = (struct debug_class_type *) xmalloc (sizeof *c);
  memset (c, 0, sizeof *c);

  c->fields = fields;

  t->u.kclass = c;

  return t;
}

/* Make an object type.  The first three arguments after the handle
   are the same as for debug_make_struct_type.  The next arguments are
   a NULL terminated array of base classes, a NULL terminated array of
   methods, the type of the object holding the virtual function table
   if it is not this object, and a boolean which is true if this
   object has its own virtual function table.  */

debug_type
debug_make_object_type (handle, structp, size, fields, baseclasses,
			methods, vptrbase, ownvptr)
     PTR handle;
     boolean structp;
     bfd_vma size;
     debug_field *fields;
     debug_baseclass *baseclasses;
     debug_method *methods;
     debug_type vptrbase;
     boolean ownvptr;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_class_type *c;

  t = debug_make_type (info,
		       structp ? DEBUG_KIND_CLASS : DEBUG_KIND_UNION_CLASS,
		       size);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  c = (struct debug_class_type *) xmalloc (sizeof *c);
  memset (c, 0, sizeof *c);

  c->fields = fields;
  c->baseclasses = baseclasses;
  c->methods = methods;
  if (ownvptr)
    c->vptrbase = t;
  else
    c->vptrbase = vptrbase;

  t->u.kclass = c;

  return t;
}

/* Make an enumeration type.  The arguments are a null terminated
   array of strings, and an array of corresponding values.  */

debug_type
debug_make_enum_type (handle, names, values)
     PTR handle;
     const char **names;
     bfd_signed_vma *values;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_enum_type *e;

  t = debug_make_type (info, DEBUG_KIND_ENUM, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  e = (struct debug_enum_type *) xmalloc (sizeof *e);
  memset (e, 0, sizeof *e);

  e->names = names;
  e->values = values;

  t->u.kenum = e;

  return t;
}

/* Make a pointer to a given type.  */

debug_type
debug_make_pointer_type (handle, type)
     PTR handle;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  if (type->pointer != DEBUG_TYPE_NULL)
    return type->pointer;

  t = debug_make_type (info, DEBUG_KIND_POINTER, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  t->u.kpointer = type;

  type->pointer = t;

  return t;
}

/* Make a function returning a given type.  FIXME: We should be able
   to record the parameter types.  */

debug_type
debug_make_function_type (handle, type)
     PTR handle;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_function_type *f;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_FUNCTION, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  f = (struct debug_function_type *) xmalloc (sizeof *f);
  memset (f, 0, sizeof *f);

  f->return_type = type;

  t->u.kfunction = f;

  return t;
}

/* Make a reference to a given type.  */

debug_type
debug_make_reference_type (handle, type)
     PTR handle;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_REFERENCE, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  t->u.kreference = type;

  return t;
}

/* Make a range of a given type from a lower to an upper bound.  */

debug_type
debug_make_range_type (handle, type, lower, upper)
     PTR handle;
     debug_type type;
     bfd_signed_vma lower;
     bfd_signed_vma upper;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_range_type *r;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_RANGE, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  r = (struct debug_range_type *) xmalloc (sizeof *r);
  memset (r, 0, sizeof *r);

  r->type = type;
  r->lower = lower;
  r->upper = upper;

  t->u.krange = r;

  return t;
}

/* Make an array type.  The second argument is the type of an element
   of the array.  The third argument is the type of a range of the
   array.  The fourth and fifth argument are the lower and upper
   bounds, respectively.  The sixth argument is true if this array is
   actually a string, as in C.  */

debug_type
debug_make_array_type (handle, element_type, range_type, lower, upper,
		       stringp)
     PTR handle;
     debug_type element_type;
     debug_type range_type;
     bfd_signed_vma lower;
     bfd_signed_vma upper;
     boolean stringp;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_array_type *a;

  if (element_type == NULL || range_type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_ARRAY, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  a = (struct debug_array_type *) xmalloc (sizeof *a);
  memset (a, 0, sizeof *a);

  a->element_type = element_type;
  a->range_type = range_type;
  a->lower = lower;
  a->upper = upper;
  a->stringp = stringp;

  t->u.karray = a;

  return t;
}

/* Make a set of a given type.  For example, a Pascal set type.  The
   boolean argument is true if this set is actually a bitstring, as in
   CHILL.  */

debug_type
debug_make_set_type (handle, type, bitstringp)
     PTR handle;
     debug_type type;
     boolean bitstringp;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_set_type *s;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_SET, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  s = (struct debug_set_type *) xmalloc (sizeof *s);
  memset (s, 0, sizeof *s);

  s->type = type;
  s->bitstringp = bitstringp;

  t->u.kset = s;

  return t;
}

/* Make a type for a pointer which is relative to an object.  The
   second argument is the type of the object to which the pointer is
   relative.  The third argument is the type that the pointer points
   to.  */

debug_type
debug_make_offset_type (handle, base_type, target_type)
     PTR handle;
     debug_type base_type;
     debug_type target_type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_offset_type *o;

  if (base_type == NULL || target_type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_OFFSET, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  o = (struct debug_offset_type *) xmalloc (sizeof *o);
  memset (o, 0, sizeof *o);

  o->base_type = base_type;
  o->target_type = target_type;

  t->u.koffset = o;

  return t;
}

/* Make a type for a method function.  The second argument is the
   return type, the third argument is the domain, and the fourth
   argument is a NULL terminated array of argument types.  */

debug_type
debug_make_method_type (handle, return_type, domain_type, arg_types)
     PTR handle;
     debug_type return_type;
     debug_type domain_type;
     debug_type *arg_types;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_method_type *m;

  if (return_type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_METHOD, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  m = (struct debug_method_type *) xmalloc (sizeof *m);
  memset (m, 0, sizeof *m);

  m->return_type = return_type;
  m->domain_type = domain_type;
  m->arg_types = arg_types;

  t->u.kmethod = m;

  return t;
}

/* Make a const qualified version of a given type.  */

debug_type
debug_make_const_type (handle, type)
     PTR handle;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_CONST, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  t->u.kconst = type;

  return t;
}

/* Make a volatile qualified version of a given type.  */

debug_type
debug_make_volatile_type (handle, type)
     PTR handle;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  if (type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_VOLATILE, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  t->u.kvolatile = type;

  return t;
}

/* Make an undefined tagged type.  For example, a struct which has
   been mentioned, but not defined.  */

debug_type
debug_make_undefined_tagged_type (handle, name, kind)
     PTR handle;
     const char *name;
     enum debug_type_kind kind;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;

  if (name == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, kind, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  return debug_tag_type (handle, name, t);
}

/* Make a base class for an object.  The second argument is the base
   class type.  The third argument is the bit position of this base
   class in the object (always 0 unless doing multiple inheritance).
   The fourth argument is whether this is a virtual class.  The fifth
   argument is the visibility of the base class.  */

/*ARGSUSED*/
debug_baseclass
debug_make_baseclass (handle, type, bitpos, virtual, visibility)
     PTR handle;
     debug_type type;
     bfd_vma bitpos;
     boolean virtual;
     enum debug_visibility visibility;
{     
  struct debug_baseclass *b;

  b = (struct debug_baseclass *) xmalloc (sizeof *b);
  memset (b, 0, sizeof *b);

  b->type = type;
  b->bitpos = bitpos;
  b->virtual = virtual;
  b->visibility = visibility;

  return b;
}

/* Make a field for a struct.  The second argument is the name.  The
   third argument is the type of the field.  The fourth argument is
   the bit position of the field.  The fifth argument is the size of
   the field (it may be zero).  The sixth argument is the visibility
   of the field.  */

/*ARGSUSED*/
debug_field
debug_make_field (handle, name, type, bitpos, bitsize, visibility)
     PTR handle;
     const char *name;
     debug_type type;
     bfd_vma bitpos;
     bfd_vma bitsize;
     enum debug_visibility visibility;
{
  struct debug_field *f;

  f = (struct debug_field *) xmalloc (sizeof *f);
  memset (f, 0, sizeof *f);

  f->name = name;
  f->type = type;
  f->static_member = false;
  f->u.f.bitpos = bitpos;
  f->u.f.bitsize = bitsize;
  f->visibility = visibility;

  return f;
}

/* Make a static member of an object.  The second argument is the
   name.  The third argument is the type of the member.  The fourth
   argument is the physical name of the member (i.e., the name as a
   global variable).  The fifth argument is the visibility of the
   member.  */

/*ARGSUSED*/
debug_field
debug_make_static_member (handle, name, type, physname, visibility)
     PTR handle;
     const char *name;
     debug_type type;
     const char *physname;
     enum debug_visibility visibility;
{
  struct debug_field *f;

  f = (struct debug_field *) xmalloc (sizeof *f);
  memset (f, 0, sizeof *f);

  f->name = name;
  f->type = type;
  f->static_member = true;
  f->u.s.physname = physname;
  f->visibility = visibility;

  return f;
}

/* Make a method.  The second argument is the name, and the third
   argument is a NULL terminated array of method variants.  */

/*ARGSUSED*/
debug_method
debug_make_method (handle, name, variants)
     PTR handle;
     const char *name;
     debug_method_variant *variants;
{
  struct debug_method *m;

  m = (struct debug_method *) xmalloc (sizeof *m);
  memset (m, 0, sizeof *m);

  m->name = name;
  m->variants = variants;

  return m;
}

/* Make a method argument.  The second argument is the real name of
   the function.  The third argument is the type of the function.  The
   fourth argument is the visibility.  The fifth argument is whether
   this is a const function.  The sixth argument is whether this is a
   volatile function.  The seventh argument is the offset in the
   virtual function table, if any.  The eighth argument is the virtual
   function context.  FIXME: Are the const and volatile arguments
   necessary?  Could we just use debug_make_const_type?  */

/*ARGSUSED*/
debug_method_variant
debug_make_method_variant (handle, argtypes, type, visibility, constp,
			   volatilep, voffset, context)
     PTR handle;
     const char *argtypes;
     debug_type type;
     enum debug_visibility visibility;
     boolean constp;
     boolean volatilep;
     bfd_vma voffset;
     debug_type context;
{
  struct debug_method_variant *m;

  m = (struct debug_method_variant *) xmalloc (sizeof *m);
  memset (m, 0, sizeof *m);

  m->argtypes = argtypes;
  m->type = type;
  m->visibility = visibility;
  m->constp = constp;
  m->volatilep = volatilep;
  m->voffset = voffset;
  m->context = context;

  return m;
}

/* Make a static method argument.  The arguments are the same as for
   debug_make_method_variant, except that the last two are omitted
   since a static method can not also be virtual.  */

debug_method_variant
debug_make_static_method_variant (handle, argtypes, type, visibility,
				  constp, volatilep)
     PTR handle;
     const char *argtypes;
     debug_type type;
     enum debug_visibility visibility;
     boolean constp;
     boolean volatilep;
{
  struct debug_method_variant *m;

  m = (struct debug_method_variant *) xmalloc (sizeof *m);
  memset (m, 0, sizeof *m);

  m->argtypes = argtypes;
  m->type = type;
  m->visibility = visibility;
  m->constp = constp;
  m->volatilep = volatilep;
  m->voffset = VOFFSET_STATIC_METHOD;

  return m;
}

/* Name a type.  */

debug_type
debug_name_type (handle, name, type)
     PTR handle;
     const char *name;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_named_type *n;
  struct debug_name *nm;

  if (name == NULL || type == NULL)
    return DEBUG_TYPE_NULL;

  t = debug_make_type (info, DEBUG_KIND_NAMED, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  n = (struct debug_named_type *) xmalloc (sizeof *n);
  memset (n, 0, sizeof *n);

  n->type = type;

  t->u.knamed = n;

  /* We also need to add the name to the current namespace.  */

  nm = debug_add_to_current_namespace (info, name, DEBUG_OBJECT_TYPE,
				       DEBUG_LINKAGE_NONE);
  if (nm == NULL)
    return false;

  nm->u.type = t;

  n->name = nm;

  return t;
}

/* Tag a type.  */

debug_type
debug_tag_type (handle, name, type)
     PTR handle;
     const char *name;
     debug_type type;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_type *t;
  struct debug_named_type *n;
  struct debug_name *nm;

  if (name == NULL || type == NULL)
    return DEBUG_TYPE_NULL;

  if (info->current_file == NULL)
    {
      debug_error ("debug_tag_type: no current file");
      return DEBUG_TYPE_NULL;
    }

  if (type->kind == DEBUG_KIND_TAGGED)
    {
      if (strcmp (type->u.knamed->name->name, name) == 0)
	return type;
      debug_error ("debug_tag_type: extra tag attempted");
      return DEBUG_TYPE_NULL;
    }

  t = debug_make_type (info, DEBUG_KIND_TAGGED, 0);
  if (t == NULL)
    return DEBUG_TYPE_NULL;

  n = (struct debug_named_type *) xmalloc (sizeof *n);
  memset (n, 0, sizeof *n);

  n->type = type;

  t->u.knamed = n;

  /* We keep a global namespace of tags for each compilation unit.  I
     don't know if that is the right thing to do.  */

  nm = debug_add_to_namespace (info, &info->current_file->globals, name,
			       DEBUG_OBJECT_TAG, DEBUG_LINKAGE_NONE);
  if (nm == NULL)
    return false;

  nm->u.tag = t;

  n->name = nm;

  return t;
}

/* Record the size of a given type.  */

/*ARGSUSED*/
boolean
debug_record_type_size (handle, type, size)
     PTR handle;
     debug_type type;
     unsigned int size;
{
  if (type->size != 0 && type->size != size)
    fprintf (stderr, "Warning: changing type size from %d to %d\n",
	     type->size, size);

  type->size = size;

  return true;
}

/* Find a tagged type.  */

debug_type
debug_find_tagged_type (handle, name, kind)
     PTR handle;
     const char *name;
     enum debug_type_kind kind;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_unit *u;

  /* We search the globals of all the compilation units.  I don't know
     if this is correct or not.  It would be easy to change.  */

  for (u = info->units; u != NULL; u = u->next)
    {
      struct debug_file *f;

      for (f = u->files; f != NULL; f = f->next)
	{
	  struct debug_name *n;

	  if (f->globals != NULL)
	    {
	      for (n = f->globals->list; n != NULL; n = n->next)
		{
		  if (n->kind == DEBUG_OBJECT_TAG
		      && (kind == DEBUG_KIND_VOID
			  || n->u.tag->kind == kind)
		      && n->name[0] == name[0]
		      && strcmp (n->name, name) == 0)
		    return n->u.tag;
		}
	    }
	}
    }

  return DEBUG_TYPE_NULL;
}

/* Get the name of a type.  */

/*ARGSUSED*/
const char *
debug_get_type_name (handle, type)
     PTR handle;
     debug_type type;
{
  if (type->kind == DEBUG_KIND_INDIRECT)
    {
      if (*type->u.kindirect->slot != NULL)
	return debug_get_type_name (handle, *type->u.kindirect->slot);
      return type->u.kindirect->tag;
    }
  if (type->kind == DEBUG_KIND_NAMED
      || type->kind == DEBUG_KIND_TAGGED)
    return type->u.knamed->name->name;
  return NULL;
}

/* Write out the debugging information.  This is given a handle to
   debugging information, and a set of function pointers to call.  */

boolean
debug_write (handle, fns, fhandle)
     PTR handle;
     const struct debug_write_fns *fns;
     PTR fhandle;
{
  struct debug_handle *info = (struct debug_handle *) handle;
  struct debug_unit *u;

  /* We use a mark to tell whether we have already written out a
     particular name.  We use an integer, so that we don't have to
     clear the mark fields if we happen to write out the same
     information more than once.  */
  ++info->mark;

  for (u = info->units; u != NULL; u = u->next)
    {
      struct debug_file *f;
      boolean first_file;

      if (! (*fns->start_compilation_unit) (fhandle, u->files->filename))
	return false;

      first_file = true;
      for (f = u->files; f != NULL; f = f->next)
	{
	  struct debug_name *n;

	  if (first_file)
	    first_file = false;
	  else
	    {
	      if (! (*fns->start_source) (fhandle, f->filename))
		return false;
	    }

	  if (f->globals != NULL)
	    {
	      for (n = f->globals->list; n != NULL; n = n->next)
		{
		  if (! debug_write_name (info, fns, fhandle, n))
		    return false;
		}
	    }
	}
    }

  return true;
}

/* Write out an element in a namespace.  */

static boolean
debug_write_name (info, fns, fhandle, n)
     struct debug_handle *info;
     const struct debug_write_fns *fns;
     PTR fhandle;
     struct debug_name *n;
{
  /* The class_mark field is used to prevent recursively outputting a
     struct or class.  */
  ++info->class_mark;

  switch (n->kind)
    {
    case DEBUG_OBJECT_TYPE:
      if (! debug_write_type (info, fns, fhandle, n->u.type, n)
	  || ! (*fns->typdef) (fhandle, n->name))
	return false;
      return true;
    case DEBUG_OBJECT_TAG:
      if (! debug_write_type (info, fns, fhandle, n->u.tag, n))
	return false;
      return (*fns->tag) (fhandle, n->name);
    case DEBUG_OBJECT_VARIABLE:
      if (! debug_write_type (info, fns, fhandle, n->u.variable->type,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->variable) (fhandle, n->name, n->u.variable->kind,
			       n->u.variable->val);
    case DEBUG_OBJECT_FUNCTION:
      return debug_write_function (info, fns, fhandle, n->name,
				   n->linkage, n->u.function);
    case DEBUG_OBJECT_INT_CONSTANT:
      return (*fns->int_constant) (fhandle, n->name, n->u.int_constant);
    case DEBUG_OBJECT_FLOAT_CONSTANT:
      return (*fns->float_constant) (fhandle, n->name, n->u.float_constant);
    case DEBUG_OBJECT_TYPED_CONSTANT:
      if (! debug_write_type (info, fns, fhandle, n->u.typed_constant->type,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->typed_constant) (fhandle, n->name,
				     n->u.typed_constant->val);
    default:
      abort ();
      return false;
    }
  /*NOTREACHED*/
}

/* Write out a type.  */

static boolean
debug_write_type (info, fns, fhandle, type, name)
     struct debug_handle *info;
     const struct debug_write_fns *fns;
     PTR fhandle;
     struct debug_type *type;
     struct debug_name *name;
{
  unsigned int i;

  /* If we have a name for this type, just output it.  We only output
     typedef names after they have been defined.  We output type tags
     whenever we are not actually defining them.  */
  if ((type->kind == DEBUG_KIND_NAMED
       || type->kind == DEBUG_KIND_TAGGED)
      && (type->u.knamed->name->mark == info->mark
	  || (type->kind == DEBUG_KIND_TAGGED
	      && type->u.knamed->name != name)))
    {
      if (type->kind == DEBUG_KIND_NAMED)
	return (*fns->typedef_type) (fhandle, type->u.knamed->name->name);
      else
	return (*fns->tag_type) (fhandle, type->u.knamed->name->name,
				 type->u.knamed->type->kind);
    }

  /* Mark the name after we have already looked for a known name, so
     that we don't just define a type in terms of itself.  We need to
     mark the name here so that a struct containing a pointer to
     itself will work.  */
  if (name != NULL)
    name->mark = info->mark;

  switch (type->kind)
    {
    case DEBUG_KIND_INDIRECT:
      if (*type->u.kindirect->slot == DEBUG_TYPE_NULL)
	return (*fns->empty_type) (fhandle);
      return debug_write_type (info, fns, fhandle, *type->u.kindirect->slot,
			       (struct debug_name *) NULL);
    case DEBUG_KIND_VOID:
      return (*fns->void_type) (fhandle);
    case DEBUG_KIND_INT:
      return (*fns->int_type) (fhandle, type->size, type->u.kint);
    case DEBUG_KIND_FLOAT:
      return (*fns->float_type) (fhandle, type->size);
    case DEBUG_KIND_COMPLEX:
      return (*fns->complex_type) (fhandle, type->size);
    case DEBUG_KIND_BOOL:
      return (*fns->bool_type) (fhandle, type->size);
    case DEBUG_KIND_STRUCT:
    case DEBUG_KIND_UNION:
      if (info->class_mark == type->u.kclass->mark)
	{
	  /* We are currently outputting this struct.  I don't know if
             this can happen, but it can happen for a class.  */
	  return (*fns->tag_type) (fhandle, "?defining?", type->kind);
	}
      type->u.kclass->mark = info->class_mark;

      if (! (*fns->start_struct_type) (fhandle,
				       type->kind == DEBUG_KIND_STRUCT,
				       type->size))
	return false;
      if (type->u.kclass->fields != NULL)
	{
	  for (i = 0; type->u.kclass->fields[i] != NULL; i++)
	    {
	      struct debug_field *f;

	      f = type->u.kclass->fields[i];
	      if (! debug_write_type (info, fns, fhandle, f->type,
				      (struct debug_name *) NULL)
		  || ! (*fns->struct_field) (fhandle, f->name, f->u.f.bitpos,
					     f->u.f.bitsize, f->visibility))
		return false;
	    }
	}
      return (*fns->end_struct_type) (fhandle);
    case DEBUG_KIND_CLASS:
    case DEBUG_KIND_UNION_CLASS:
      return debug_write_class_type (info, fns, fhandle, type);
    case DEBUG_KIND_ENUM:
      return (*fns->enum_type) (fhandle, type->u.kenum->names,
				type->u.kenum->values);
    case DEBUG_KIND_POINTER:
      if (! debug_write_type (info, fns, fhandle, type->u.kpointer,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->pointer_type) (fhandle);
    case DEBUG_KIND_FUNCTION:
      if (! debug_write_type (info, fns, fhandle,
			      type->u.kfunction->return_type,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->function_type) (fhandle);
    case DEBUG_KIND_REFERENCE:
      if (! debug_write_type (info, fns, fhandle, type->u.kreference,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->reference_type) (fhandle);
    case DEBUG_KIND_RANGE:
      if (! debug_write_type (info, fns, fhandle, type->u.krange->type,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->range_type) (fhandle, type->u.krange->lower,
				 type->u.krange->upper);
    case DEBUG_KIND_ARRAY:
      if (! debug_write_type (info, fns, fhandle, type->u.karray->element_type,
			      (struct debug_name *) NULL)
	  || ! debug_write_type (info, fns, fhandle,
				 type->u.karray->range_type,
				 (struct debug_name *) NULL))
	return false;
      return (*fns->array_type) (fhandle, type->u.karray->lower,
				 type->u.karray->upper,
				 type->u.karray->stringp);
    case DEBUG_KIND_SET:
      if (! debug_write_type (info, fns, fhandle, type->u.kset->type,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->set_type) (fhandle, type->u.kset->bitstringp);
    case DEBUG_KIND_OFFSET:
      if (! debug_write_type (info, fns, fhandle, type->u.koffset->base_type,
			      (struct debug_name *) NULL)
	  || ! debug_write_type (info, fns, fhandle,
				 type->u.koffset->target_type,
				 (struct debug_name *) NULL))
	return false;
      return (*fns->offset_type) (fhandle);
    case DEBUG_KIND_METHOD:
      if (! debug_write_type (info, fns, fhandle,
			      type->u.kmethod->return_type,
			      (struct debug_name *) NULL))
	return false;
      if (type->u.kmethod->arg_types == NULL)
	i = -1;
      else
	{
	  for (i = 0; type->u.kmethod->arg_types[i] != NULL; i++)
	    {
	      if (! debug_write_type (info, fns, fhandle,
				      type->u.kmethod->arg_types[i],
				      (struct debug_name *) NULL))
		return false;
	    }
	}
      if (type->u.kmethod->domain_type != NULL)
	{
	  if (! debug_write_type (info, fns, fhandle,
				  type->u.kmethod->domain_type,
				  (struct debug_name *) NULL))
	    return false;
	}
      return (*fns->method_type) (fhandle,
				  type->u.kmethod->domain_type != NULL,
				  i);
    case DEBUG_KIND_CONST:
      if (! debug_write_type (info, fns, fhandle, type->u.kconst,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->const_type) (fhandle);
    case DEBUG_KIND_VOLATILE:
      if (! debug_write_type (info, fns, fhandle, type->u.kvolatile,
			      (struct debug_name *) NULL))
	return false;
      return (*fns->volatile_type) (fhandle);
    case DEBUG_KIND_NAMED:
    case DEBUG_KIND_TAGGED:
      return debug_write_type (info, fns, fhandle, type->u.knamed->type,
			       (struct debug_name *) NULL);
    default:
      abort ();
      return false;
    }
}

/* Write out a class type.  */

static boolean
debug_write_class_type (info, fns, fhandle, type)
     struct debug_handle *info;
     const struct debug_write_fns *fns;
     PTR fhandle;
     struct debug_type *type;
{
  unsigned int i;

  if (info->class_mark == type->u.kclass->mark)
    {
      /* We are currently outputting this class.  This can happen when
         there are methods for an anonymous class.  */
      return (*fns->tag_type) (fhandle, "?defining?", type->kind);
    }
  type->u.kclass->mark = info->class_mark;

  if (type->u.kclass->vptrbase != NULL
      && type->u.kclass->vptrbase != type)
    {
      if (! debug_write_type (info, fns, fhandle, type->u.kclass->vptrbase,
			      (struct debug_name *) NULL))
	return false;
    }

  if (! (*fns->start_class_type) (fhandle,
				  type->kind == DEBUG_KIND_CLASS,
				  type->size,
				  type->u.kclass->vptrbase != NULL,
				  type->u.kclass->vptrbase == type))
    return false;
  if (type->u.kclass->fields != NULL)
    {
      for (i = 0; type->u.kclass->fields[i] != NULL; i++)
	{
	  struct debug_field *f;

	  f = type->u.kclass->fields[i];
	  if (! debug_write_type (info, fns, fhandle, f->type,
				  (struct debug_name *) NULL))
	    return false;
	  if (f->static_member)
	    {
	      if (! (*fns->class_static_member) (fhandle, f->name,
						 f->u.s.physname,
						 f->visibility))
		return false;
	    }
	  else
	    {
	      if (! (*fns->struct_field) (fhandle, f->name, f->u.f.bitpos,
					  f->u.f.bitsize, f->visibility))
		return false;
	    }
	}
    }

  if (type->u.kclass->baseclasses != NULL)
    {
      for (i = 0; type->u.kclass->baseclasses[i] != NULL; i++)
	{
	  struct debug_baseclass *b;

	  b = type->u.kclass->baseclasses[i];
	  if (! debug_write_type (info, fns, fhandle, b->type,
				  (struct debug_name *) NULL))
	    return false;
	  if (! (*fns->class_baseclass) (fhandle, b->bitpos, b->virtual,
					 b->visibility))
	    return false;
	}
    }

  if (type->u.kclass->methods != NULL)
    {
      for (i = 0; type->u.kclass->methods[i] != NULL; i++)
	{
	  struct debug_method *m;
	  unsigned int j;

	  m = type->u.kclass->methods[i];
	  if (! (*fns->class_start_method) (fhandle, m->name))
	    return false;
	  for (j = 0; m->variants[j] != NULL; j++)
	    {
	      struct debug_method_variant *v;

	      v = m->variants[j];
	      if (v->context != NULL)
		{
		  if (! debug_write_type (info, fns, fhandle, v->context,
					  (struct debug_name *) NULL))
		    return false;
		}
	      if (! debug_write_type (info, fns, fhandle, v->type,
				      (struct debug_name *) NULL))
		return false;
	      if (v->voffset != VOFFSET_STATIC_METHOD)
		{
		  if (! (*fns->class_method_variant) (fhandle, v->argtypes,
						      v->visibility,
						      v->constp, v->volatilep,
						      v->voffset,
						      v->context != NULL))
		    return false;
		}
	      else
		{
		  if (! (*fns->class_static_method_variant) (fhandle,
							     v->argtypes,
							     v->visibility,
							     v->constp,
							     v->volatilep))
		    return false;
		}
	    }
	  if (! (*fns->class_end_method) (fhandle))
	    return false;
	}
    }

  return (*fns->end_class_type) (fhandle);
}

/* Write out information for a function.  */

static boolean
debug_write_function (info, fns, fhandle, name, linkage, function)
     struct debug_handle *info;
     const struct debug_write_fns *fns;
     PTR fhandle;
     const char *name;
     enum debug_object_linkage linkage;
     struct debug_function *function;
{
  struct debug_parameter *p;
  struct debug_block *b;

  if (! debug_write_type (info, fns, fhandle, function->return_type,
			  (struct debug_name *) NULL))
    return false;

  if (! (*fns->start_function) (fhandle, name,
				linkage == DEBUG_LINKAGE_GLOBAL))
    return false;

  for (p = function->parameters; p != NULL; p = p->next)
    {
      if (! debug_write_type (info, fns, fhandle, p->type,
			      (struct debug_name *) NULL)
	  || ! (*fns->function_parameter) (fhandle, p->name, p->kind, p->val))
	return false;
    }

  for (b = function->blocks; b != NULL; b = b->next)
    {
      if (! debug_write_block (info, fns, fhandle, b))
	return false;
    }

  return (*fns->end_function) (fhandle);
}

/* Write out information for a block.  */

static boolean
debug_write_block (info, fns, fhandle, block)
     struct debug_handle *info;
     const struct debug_write_fns *fns;
     PTR fhandle;
     struct debug_block *block;
{
  struct debug_name *n;
  struct debug_lineno *l;
  struct debug_block *b;

  if (! (*fns->start_block) (fhandle, block->start))
    return false;

  if (block->locals != NULL)
    {
      for (n = block->locals->list; n != NULL; n = n->next)
	{
	  if (! debug_write_name (info, fns, fhandle, n))
	    return false;
	}
    }

  for (l = block->linenos; l != NULL; l = l->next)
    {
      unsigned int i;

      for (i = 0; i < DEBUG_LINENO_COUNT; i++)
	{
	  if (l->linenos[i] == (unsigned long) -1)
	    break;
	  if (! (*fns->lineno) (fhandle, l->file->filename, l->linenos[i],
				l->addrs[i]))
	    return false;
	}
    }

  for (b = block->children; b != NULL; b = b->next)
    {
      if (! debug_write_block (info, fns, fhandle, b))
	return false;
    }

  return (*fns->end_block) (fhandle, block->end);
}
