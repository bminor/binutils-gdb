/* Support for printing Fortran values for GDB, the GNU debugger.
   Copyright 1993, 1994 Free Software Foundation, Inc.
   Contributed by Motorola.  Adapted from the C definitions by Farooq Butt
   (fmbutt@engage.sps.mot.com), additionally worked over by Stan Shebs.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "value.h"
#include "demangle.h"
#include "valprint.h"
#include "language.h"
#include "f-lang.h" 
#include "frame.h"

extern struct obstack dont_print_obstack;

extern unsigned int print_max; /* No of array elements to print */ 

int f77_array_offset_tbl[MAX_FORTRAN_DIMS+1][2];

/* Array which holds offsets to be applied to get a row's elements
   for a given array. Array also holds the size of each subarray.  */

/* The following macro gives us the size of the nth dimension, Where 
   n is 1 based. */ 

#define F77_DIM_SIZE(n) (f77_array_offset_tbl[n][1])

/* The following gives us the offset for row n where n is 1-based. */ 

#define F77_DIM_OFFSET(n) (f77_array_offset_tbl[n][0])

int 
f77_get_dynamic_lowerbound (type, lower_bound)
     struct type *type;
     int *lower_bound; 
{
  CORE_ADDR current_frame_addr;   
  CORE_ADDR ptr_to_lower_bound; 
  
  switch (TYPE_ARRAY_LOWER_BOUND_TYPE (type))
    {
    case BOUND_BY_VALUE_ON_STACK:
      current_frame_addr = selected_frame->frame;
      if (current_frame_addr > 0) 
	{
	  *lower_bound = 
	    read_memory_integer (current_frame_addr + 
				 TYPE_ARRAY_LOWER_BOUND_VALUE (type),4);
	}
      else
	{
	  *lower_bound = DEFAULT_LOWER_BOUND; 
	  return BOUND_FETCH_ERROR; 
	}
      break; 
      
    case BOUND_SIMPLE:
      *lower_bound = TYPE_ARRAY_LOWER_BOUND_VALUE (type);
      break; 
      
    case BOUND_CANNOT_BE_DETERMINED: 
      error("Lower bound may not be '*' in F77"); 
      break; 
      
    case BOUND_BY_REF_ON_STACK:
      current_frame_addr = selected_frame->frame;
      if (current_frame_addr > 0) 
	{
	  ptr_to_lower_bound = 
	    read_memory_integer (current_frame_addr + 
				 TYPE_ARRAY_LOWER_BOUND_VALUE (type),
				 4);
	  *lower_bound = read_memory_integer(ptr_to_lower_bound); 
	}
      else
	{
	  *lower_bound = DEFAULT_LOWER_BOUND; 
	  return BOUND_FETCH_ERROR; 
	}
      break; 
      
    case BOUND_BY_REF_IN_REG: 
    case BOUND_BY_VALUE_IN_REG: 
    default: 
      error ("??? unhandled dynamic array bound type ???");
      break; 
    }
  return BOUND_FETCH_OK;
}

int 
f77_get_dynamic_upperbound (type, upper_bound)
     struct type *type;
     int *upper_bound;
{
  CORE_ADDR current_frame_addr = 0;
  CORE_ADDR ptr_to_upper_bound; 
  
  switch (TYPE_ARRAY_UPPER_BOUND_TYPE (type))
    {
    case BOUND_BY_VALUE_ON_STACK:
      current_frame_addr = selected_frame->frame;
      if (current_frame_addr > 0) 
	{
	  *upper_bound = 
	    read_memory_integer (current_frame_addr + 
				 TYPE_ARRAY_UPPER_BOUND_VALUE (type),4);
	}
      else
	{
	  *upper_bound = DEFAULT_UPPER_BOUND; 
	  return BOUND_FETCH_ERROR; 
	}
      break; 
      
    case BOUND_SIMPLE:
      *upper_bound = TYPE_ARRAY_UPPER_BOUND_VALUE (type);
      break; 
      
    case BOUND_CANNOT_BE_DETERMINED: 
      /* we have an assumed size array on our hands. Assume that 
	 upper_bound == lower_bound so that we show at least 
	 1 element.If the user wants to see more elements, let 
	 him manually ask for 'em and we'll subscript the 
	 array and show him */
      f77_get_dynamic_lowerbound (type, &upper_bound);
      break; 
      
    case BOUND_BY_REF_ON_STACK:
      current_frame_addr = selected_frame->frame;
      if (current_frame_addr > 0) 
	{
	  ptr_to_upper_bound = 
	    read_memory_integer (current_frame_addr + 
				 TYPE_ARRAY_UPPER_BOUND_VALUE (type),
				 4);
	  *upper_bound = read_memory_integer(ptr_to_upper_bound); 
	}
      else
	{
	  *upper_bound = DEFAULT_UPPER_BOUND; 
	  return BOUND_FETCH_ERROR;
	}
      break; 
      
    case BOUND_BY_REF_IN_REG: 
    case BOUND_BY_VALUE_IN_REG: 
    default: 
      error ("??? unhandled dynamic array bound type ???");
      break; 
    }
  return BOUND_FETCH_OK;
}

/* Obtain F77 adjustable array dimensions */ 

void
f77_get_dynamic_length_of_aggregate (type)
     struct type *type;
{
  int upper_bound = -1;
  int lower_bound = 1; 
  unsigned int current_total = 1;
  int retcode; 
  
  /* Recursively go all the way down into a possibly 
     multi-dimensional F77 array 
     and get the bounds.  For simple arrays, this is pretty easy 
     but when the bounds are dynamic, we must be very careful 
     to add up all the lengths correctly.  Not doing this right 
     will lead to horrendous-looking arrays in parameter lists.
     
     This function also works for strings which behave very 
     similarly to arrays.  */ 
  
  if (TYPE_CODE(TYPE_TARGET_TYPE (type)) == TYPE_CODE_ARRAY
      || TYPE_CODE(TYPE_TARGET_TYPE (type)) == TYPE_CODE_STRING)
    f77_get_dynamic_length_of_aggregate (TYPE_TARGET_TYPE (type));
  
  /* Recursion ends here, start setting up lengths.  */ 
  retcode = f77_get_dynamic_lowerbound (type, &lower_bound); 
  if (retcode == BOUND_FETCH_ERROR)
    error ("Cannot obtain valid array lower bound"); 
  
  retcode = f77_get_dynamic_upperbound (type, &upper_bound); 
  if (retcode == BOUND_FETCH_ERROR)
    error ("Cannot obtain valid array upper bound"); 
  
  /* Patch in a valid length value. */ 
  
  TYPE_LENGTH (type) =
    (upper_bound - lower_bound + 1) * TYPE_LENGTH (TYPE_TARGET_TYPE (type));
}       

/* Print a FORTRAN COMPLEX value of type TYPE, pointed to in GDB by VALADDR,
   on STREAM.  which_complex indicates precision, which may be regular,
   *16, or *32 */

void
f77_print_cmplx (valaddr, type, stream, which_complex)
     char *valaddr;
     struct type *type;
     FILE *stream;
     int which_complex;
{
  float *f1,*f2;
  double *d1, *d2;
  int i; 
  
  switch (which_complex)
    {
    case TARGET_COMPLEX_BIT:
      f1 = (float *) valaddr;
      f2 = (float *) (valaddr + sizeof(float));
      fprintf_filtered (stream, "(%.7e,%.7e)", *f1, *f2);
      break;
      
    case TARGET_DOUBLE_COMPLEX_BIT:
      d1 = (double *) valaddr;
      d2 = (double *) (valaddr + sizeof(double));
      fprintf_filtered (stream, "(%.16e,%.16e)", *d1, *d2);
      break;
#if 0
    case TARGET_EXT_COMPLEX_BIT:
      fprintf_filtered (stream, "<complex*32 format unavailable, "
		       "printing raw data>\n");
      
      fprintf_filtered (stream, "( [ "); 
      
      for (i = 0;i<4;i++)
	fprintf_filtered (stream, "0x%x ",
			 * ( (unsigned int *) valaddr+i));
      
      fprintf_filtered (stream, "],\n  [ "); 
      
      for (i=4;i<8;i++)
	fprintf_filtered (stream, "0x%x ",
			 * ((unsigned int *) valaddr+i));
      
      fprintf_filtered (stream, "] )");
      
      break;
#endif
    default:
      fprintf_filtered (stream, "<cannot handle complex of this type>");
      break;
    }
}

/* Function that sets up the array offset,size table for the array 
   type "type". */ 

void 
f77_create_arrayprint_offset_tbl (type, stream)
     struct type *type;
     FILE *stream;
{
  struct type *tmp_type;
  int eltlen; 
  int ndimen = 1;
  int upper, lower, retcode; 
  
  tmp_type = type; 
  
  while ((TYPE_CODE (tmp_type) == TYPE_CODE_ARRAY)) 
    {
      if (TYPE_ARRAY_UPPER_BOUND_TYPE (tmp_type) == BOUND_CANNOT_BE_DETERMINED)
	fprintf_filtered (stream, "<assumed size array> "); 
      
      retcode = f77_get_dynamic_upperbound (tmp_type, &upper);
      if (retcode == BOUND_FETCH_ERROR)
	error ("Cannot obtain dynamic upper bound"); 
      
      retcode = f77_get_dynamic_lowerbound(tmp_type,&lower); 
      if (retcode == BOUND_FETCH_ERROR)
	error("Cannot obtain dynamic lower bound"); 
      
      F77_DIM_SIZE (ndimen) = upper - lower + 1;
      
      if (ndimen == 1)
	F77_DIM_OFFSET (ndimen) = 1;
      else
	F77_DIM_OFFSET (ndimen) = 
	  F77_DIM_OFFSET (ndimen - 1) * F77_DIM_SIZE(ndimen - 1);
      
      tmp_type = TYPE_TARGET_TYPE (tmp_type);
      ndimen++; 
    }
  
  eltlen = TYPE_LENGTH (tmp_type); 

  /* Now we multiply eltlen by all the offsets, so that later we 
     can print out array elements correctly.  Up till now we 
     know an offset to apply to get the item but we also 
     have to know how much to add to get to the next item */
  
  tmp_type = type; 
  ndimen = 1; 
  
  while ((TYPE_CODE (tmp_type) == TYPE_CODE_ARRAY)) 
    {
      F77_DIM_OFFSET (ndimen) *= eltlen; 
      ndimen++;
      tmp_type = TYPE_TARGET_TYPE (tmp_type);
    }
}

/* Actual function which prints out F77 arrays, Valaddr == address in 
   the superior.  Address == the address in the inferior.  */

void 
f77_print_array_1 (nss, ndimensions, type, valaddr, address, 
		   stream, format, deref_ref, recurse, pretty)
     int nss;
     int ndimensions; 
     char *valaddr;
     struct type *type;
     CORE_ADDR address;
     FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  int i;
  
  if (nss != ndimensions)
    {
      for (i = 0; i< F77_DIM_SIZE(nss); i++)
	{
	  fprintf_filtered (stream, "( ");
	  f77_print_array_1 (nss + 1, ndimensions, TYPE_TARGET_TYPE (type),
			    valaddr + i * F77_DIM_OFFSET (nss),
			    address + i * F77_DIM_OFFSET (nss), 
			    stream, format, deref_ref, recurse, pretty, i);
	  fprintf_filtered (stream, ") ");
	}
    }
  else
    {
      for (i = 0; (i < F77_DIM_SIZE (nss) && i < print_max); i++)
	{
	  val_print (TYPE_TARGET_TYPE (type),
		     valaddr + i * F77_DIM_OFFSET (ndimensions),
		     address + i * F77_DIM_OFFSET (ndimensions),
		     stream, format, deref_ref, recurse, pretty); 

	  if (i != (F77_DIM_SIZE (nss) - 1))
	    fprintf_filtered (stream, ", "); 
	  
	  if (i == print_max - 1)
	    fprintf_filtered (stream, "...");
	}
    }
}

/* This function gets called to print an F77 array, we set up some 
   stuff and then immediately call f77_print_array_1() */

void 
f77_print_array (type, valaddr, address, stream, format, deref_ref, recurse, 
		 pretty)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  int array_size_array[MAX_FORTRAN_DIMS+1]; 
  int ndimensions; 
  
  ndimensions = calc_f77_array_dims (type); 
  
  if (ndimensions > MAX_FORTRAN_DIMS || ndimensions < 0)
    error ("Type node corrupt! F77 arrays cannot have %d subscripts (%d Max)",
	   ndimensions, MAX_FORTRAN_DIMS);
  
  /* Since F77 arrays are stored column-major, we set up an 
     offset table to get at the various row's elements. The 
     offset table contains entries for both offset and subarray size. */ 
  
  f77_create_arrayprint_offset_tbl (type, stream); 
  
  f77_print_array_1 (1, ndimensions, type, valaddr, address, stream, format, 
                     deref_ref, recurse, pretty);
}


/* Print data of type TYPE located at VALADDR (within GDB), which came from
   the inferior at address ADDRESS, onto stdio stream STREAM according to
   FORMAT (a letter or 0 for natural format).  The data at VALADDR is in
   target byte order.
   
   If the data are a string pointer, returns the number of string characters
   printed.
   
   If DEREF_REF is nonzero, then dereference references, otherwise just print
   them like pointers.
   
   The PRETTY parameter controls prettyprinting.  */

int
f_val_print (type, valaddr, address, stream, format, deref_ref, recurse,
	     pretty)
     struct type *type;
     char *valaddr;
     CORE_ADDR address;
     FILE *stream;
     int format;
     int deref_ref;
     int recurse;
     enum val_prettyprint pretty;
{
  register unsigned int i = 0;		/* Number of characters printed */
  unsigned len;
  struct type *elttype;
  unsigned eltlen;
  LONGEST val;
  struct internalvar *ivar; 
  char *localstr; 
  unsigned char c;
  CORE_ADDR addr;
  
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_LITERAL_STRING: 
      /* It is trivial to print out F77 strings allocated in the 
	 superior process. The address field is actually a 
	 pointer to the bytes of the literal. For an internalvar,
	 valaddr points to a ptr. which points to 
	 VALUE_LITERAL_DATA(value->internalvar->value)
	 and for straight literals (i.e. of the form 'hello world'), 
	 valaddr points a ptr to VALUE_LITERAL_DATA(value). */
      
      /* First deref. valaddr  */ 
      
      addr = * (CORE_ADDR *) valaddr; 
      
      if (addr)
	{
	  len = TYPE_LENGTH (type); 
	  localstr = alloca (len + 1);
	  strncpy (localstr, addr, len);
	  localstr[len] = '\0'; 
	  fprintf_filtered (stream, "'%s'", localstr);
	}
      else
	fprintf_filtered (stream, "Unable to print literal F77 string");
      break; 
      
      /* Strings are a little bit funny. They can be viewed as 
	 monolithic arrays that are dealt with as atomic data 
	 items. As such they are the only atomic data items whose 
	 contents are not located in the superior process. Instead 
	 instead of having the actual data, they contain pointers 
	 to addresses in the inferior where data is located.  Thus 
	 instead of using valaddr, we use address. */ 
      
    case TYPE_CODE_STRING: 
      f77_get_dynamic_length_of_aggregate (type);
      val_print_string (address, TYPE_LENGTH (type), stream);
      break;
      
    case TYPE_CODE_ARRAY:
      fprintf_filtered (stream, "("); 
      f77_print_array (type, valaddr, address, stream, format, 
		       deref_ref, recurse, pretty); 
      fprintf_filtered (stream, ")");
      break;
#if 0
      /* Array of unspecified length: treat like pointer to first elt.  */
      valaddr = (char *) &address;
      /* FALL THROUGH */
#endif 
    case TYPE_CODE_PTR:
      if (format && format != 's')
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      else
	{
	  addr = unpack_pointer (type, valaddr);
	  elttype = TYPE_TARGET_TYPE (type);
	  
	  if (TYPE_CODE (elttype) == TYPE_CODE_FUNC)
	    {
	      /* Try to print what function it points to.  */
	      print_address_demangle (addr, stream, demangle);
	      /* Return value is irrelevant except for string pointers.  */
	      return 0;
	    }
	  
	  if (addressprint && format != 's')
	    fprintf_filtered (stream, "0x%x", addr);
	  
	  /* For a pointer to char or unsigned char, also print the string
	     pointed to, unless pointer is null.  */
	  if (TYPE_LENGTH (elttype) == 1
	      && TYPE_CODE (elttype) == TYPE_CODE_INT
	      && (format == 0 || format == 's')
	      && addr != 0)
	    i = val_print_string (addr, 0, stream);
	  
	  /* Return number of characters printed, plus one for the
	     terminating null if we have "reached the end".  */
	  return (i + (print_max && i != print_max));
	}
      break;
      
    case TYPE_CODE_FUNC:
      if (format)
	{
	  print_scalar_formatted (valaddr, type, format, 0, stream);
	  break;
	}
      /* FIXME, we should consider, at least for ANSI C language, eliminating
	 the distinction made between FUNCs and POINTERs to FUNCs.  */
      fprintf_filtered (stream, "{");
      type_print (type, "", stream, -1);
      fprintf_filtered (stream, "} ");
      /* Try to print what function it points to, and its address.  */
      print_address_demangle (address, stream, demangle);
      break;
      
    case TYPE_CODE_INT:
      format = format ? format : output_format;
      if (format)
	print_scalar_formatted (valaddr, type, format, 0, stream);
      else
	{
	  val_print_type_code_int (type, valaddr, stream);
	  /* C and C++ has no single byte int type, char is used instead.
	     Since we don't know whether the value is really intended to
	     be used as an integer or a character, print the character
	     equivalent as well. */
	  if (TYPE_LENGTH (type) == 1)
	    {
	      fputs_filtered (" ", stream);
	      LA_PRINT_CHAR ((unsigned char) unpack_long (type, valaddr),
			     stream);
	    }
	}
      break;
      
    case TYPE_CODE_FLT:
      if (format)
	print_scalar_formatted (valaddr, type, format, 0, stream);
      else
	print_floating (valaddr, type, stream);
      break;
      
    case TYPE_CODE_VOID:
      fprintf_filtered (stream, "VOID");
      break;
      
    case TYPE_CODE_ERROR:
      fprintf_filtered (stream, "<error type>");
      break;
      
    case TYPE_CODE_RANGE:
      /* FIXME, we should not ever have to print one of these yet.  */
      fprintf_filtered (stream, "<range type>");
      break;
      
    case TYPE_CODE_BOOL:
      format = format ? format : output_format;
      if (format)
	print_scalar_formatted (valaddr, type, format, 0, stream);
      else
	{
          val = 0; 
          switch (TYPE_LENGTH(type))
	    {
	    case 1:
	      val = unpack_long (builtin_type_f_logical_s1, valaddr);
	      break ; 
	      
	    case 2: 
	      val = unpack_long (builtin_type_f_logical_s2, valaddr);
	      break ; 
	      
	    case 4: 
	      val = unpack_long (builtin_type_f_logical, valaddr);
	      break ; 
	      
	    default:
	      error ("Logicals of length %d bytes not supported",
		     TYPE_LENGTH (type));
	      
	    }
	  
          if (val == 0) 
	    fprintf_filtered (stream, ".FALSE.");
          else 
	    if (val == 1) 
	      fprintf_filtered (stream, ".TRUE.");
	    else
	      /* Not a legitimate logical type, print as an integer.  */
	      {
		/* Bash the type code temporarily.  */
		TYPE_CODE (type) = TYPE_CODE_INT;
		f_val_print (type, valaddr, address, stream, format, 
			     deref_ref, recurse, pretty); 
		/* Restore the type code so later uses work as intended. */
		TYPE_CODE (type) = TYPE_CODE_BOOL; 
	      }
	}
      break;
      
    case TYPE_CODE_LITERAL_COMPLEX:
      /* We know that the literal complex is stored in the superior 
	 process not the inferior and that it is 16 bytes long. 
	 Just like the case above with a literal array, the
	 bytes for the the literal complex number are stored   
	 at the address pointed to by valaddr */ 
      
      if (TYPE_LENGTH(type) == 32)
	error("Cannot currently print out complex*32 literals");
      
      /* First deref. valaddr  */ 
      
      addr = * (CORE_ADDR *) valaddr; 
      
      if (addr)
	{
	  fprintf_filtered (stream, "("); 
	  
	  if (TYPE_LENGTH(type) == 16) 
	    { 
	      fprintf_filtered (stream, "%.16f", * (double *) addr); 
	      fprintf_filtered (stream, ", %.16f", * (double *) 
				(addr + sizeof(double)));
	    }
	  else
	    {
	      fprintf_filtered (stream, "%.8f", * (float *) addr); 
	      fprintf_filtered (stream, ", %.8f", * (float *) 
				(addr + sizeof(float)));
	    }
	  fprintf_filtered (stream, ") ");             
	}
      else
	fprintf_filtered (stream, "Unable to print literal F77 array");
      break; 
      
    case TYPE_CODE_COMPLEX:
      switch (TYPE_LENGTH (type))
	{
	case 8:
	  f77_print_cmplx (valaddr, type, stream, TARGET_COMPLEX_BIT);
	  break;
	  
	case 16: 
	  f77_print_cmplx(valaddr, type, stream, TARGET_DOUBLE_COMPLEX_BIT);
	  break; 
#if 0
	case 32:
	  f77_print_cmplx(valaddr, type, stream, TARGET_EXT_COMPLEX_BIT);
	  break; 
#endif
	default:
	  error ("Cannot print out complex*%d variables", TYPE_LENGTH(type)); 
	}
      break;
      
    case TYPE_CODE_UNDEF:
      /* This happens (without TYPE_FLAG_STUB set) on systems which don't use
	 dbx xrefs (NO_DBX_XREFS in gcc) if a file has a "struct foo *bar"
	 and no complete type for struct foo in that file.  */
      fprintf_filtered (stream, "<incomplete type>");
      break;
      
    default:
      error ("Invalid F77 type code %d in symbol table.", TYPE_CODE (type));
    }
  fflush (stream);
  return 0;
}

void
list_all_visible_commons (funname)
     char *funname;
{
  SAVED_F77_COMMON_PTR  tmp;
  
  tmp = head_common_list;
  
  printf_filtered ("All COMMON blocks visible at this level:\n\n");
  
  while (tmp != NULL)
    {
      if (STREQ(tmp->owning_function,funname))
	printf_filtered ("%s\n", tmp->name); 
      
      tmp = tmp->next;
    }
}

/* This function is used to print out the values in a given COMMON 
   block. It will always use the most local common block of the 
   given name */ 

static void 
info_common_command (comname, from_tty)
     char *comname;
     int from_tty;
{
  SAVED_F77_COMMON_PTR  the_common; 
  COMMON_ENTRY_PTR entry; 
  struct frame_info *fi;
  register char *funname = 0;
  struct symbol *func;
  char *cmd; 
  
  /* We have been told to display the contents of F77 COMMON 
     block supposedly visible in this function.  Let us 
     first make sure that it is visible and if so, let 
     us display its contents */ 
  
  fi = selected_frame; 
  
  if (fi == NULL)
    error ("No frame selected"); 
  
  /* The following is generally ripped off from stack.c's routine 
     print_frame_info() */ 
  
  func = find_pc_function (fi->pc);
  if (func)
    {
      /* In certain pathological cases, the symtabs give the wrong
	 function (when we are in the first function in a file which
	 is compiled without debugging symbols, the previous function
	 is compiled with debugging symbols, and the "foo.o" symbol
	 that is supposed to tell us where the file with debugging symbols
	 ends has been truncated by ar because it is longer than 15
	 characters).
	 
	 So look in the minimal symbol tables as well, and if it comes
	 up with a larger address for the function use that instead.
	 I don't think this can ever cause any problems; there shouldn't
	 be any minimal symbols in the middle of a function.
	 FIXME:  (Not necessarily true.  What about text labels) */
      
      struct minimal_symbol *msymbol = lookup_minimal_symbol_by_pc (fi->pc);
      
      if (msymbol != NULL
	  && (SYMBOL_VALUE_ADDRESS (msymbol) 
	      > BLOCK_START (SYMBOL_BLOCK_VALUE (func))))
	funname = SYMBOL_NAME (msymbol);
      else
	funname = SYMBOL_NAME (func);
    }
  else
    {
      register struct minimal_symbol *msymbol =
	lookup_minimal_symbol_by_pc (fi->pc);
      
      if (msymbol != NULL)
	funname = SYMBOL_NAME (msymbol);
    }
  
  /* If comnname is NULL, we assume the user wishes to see the 
     which COMMON blocks are visible here and then return */ 
  
  if (strlen (comname) == 0) 
    {
      list_all_visible_commons (funname);
      return; 
    }
  
  the_common = find_common_for_function (comname,funname); 
  
  if (the_common)
    {
      if (STREQ(comname,BLANK_COMMON_NAME_LOCAL))
	printf_filtered ("Contents of blank COMMON block:\n");
      else 
	printf_filtered ("Contents of F77 COMMON block '%s':\n",comname); 
      
      printf_filtered ("\n"); 
      entry = the_common->entries; 
      
      while (entry != NULL)
	{
	  printf_filtered ("%s = ",SYMBOL_NAME(entry->symbol)); 
	  print_variable_value (entry->symbol,fi,stdout); 
	  printf_filtered ("\n"); 
	  entry = entry->next; 
	}
    }
  else 
    printf_filtered ("Cannot locate the common block %s in function '%s'\n",
		    comname, funname);
}

/* This function is used to determine whether there is a
   F77 common block visible at the current scope called 'comname'. */ 

int
there_is_a_visible_common_named (comname)
     char *comname;
{
  SAVED_F77_COMMON_PTR  the_common; 
  COMMON_ENTRY_PTR entry; 
  struct frame_info *fi;
  register char *funname = 0;
  struct symbol *func;
  
  if (comname == NULL)
    error ("Cannot deal with NULL common name!"); 
  
  fi = selected_frame; 
  
  if (fi == NULL)
    error ("No frame selected"); 
  
  /* The following is generally ripped off from stack.c's routine 
     print_frame_info() */ 
  
  func = find_pc_function (fi->pc);
  if (func)
    {
      /* In certain pathological cases, the symtabs give the wrong
	 function (when we are in the first function in a file which
	 is compiled without debugging symbols, the previous function
	 is compiled with debugging symbols, and the "foo.o" symbol
	 that is supposed to tell us where the file with debugging symbols
	 ends has been truncated by ar because it is longer than 15
	 characters).
	 
	 So look in the minimal symbol tables as well, and if it comes
	 up with a larger address for the function use that instead.
	 I don't think this can ever cause any problems; there shouldn't
	 be any minimal symbols in the middle of a function.
	 FIXME:  (Not necessarily true.  What about text labels) */
      
      struct minimal_symbol *msymbol = lookup_minimal_symbol_by_pc (fi->pc);
      
      if (msymbol != NULL
	  && (SYMBOL_VALUE_ADDRESS (msymbol) 
	      > BLOCK_START (SYMBOL_BLOCK_VALUE (func))))
	funname = SYMBOL_NAME (msymbol);
      else
	funname = SYMBOL_NAME (func);
    }
  else
    {
      register struct minimal_symbol *msymbol = 
	lookup_minimal_symbol_by_pc (fi->pc);
      
      if (msymbol != NULL)
	funname = SYMBOL_NAME (msymbol);
    }
  
  the_common = find_common_for_function (comname, funname); 
  
  return (the_common ? 1 : 0);
}

void
_initialize_f_valprint ()
{
  add_info ("common", info_common_command,
	    "Print out the values contained in a Fortran COMMON block.");
}
