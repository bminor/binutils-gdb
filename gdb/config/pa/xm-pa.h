/* Definitions for all PA machines.  */

#define MAKEVA_START(list) \
  list->argindex = list->nargs * list->max_arg_size;

/* In addition to having the arguments passed with the first ones at the
   highest memory addresses, the pa also apparently aligns each argument
   to the size of that argument.  This only works because the caller only
   deals in 4 and 8 byte arguments.  If we wanted to handle args > 8 bytes,
   we would have to pass the address by reference.  */
#define MAKEVA_ARG(list, argaddr, argsize) \
  list->argindex = (list->argindex - argsize) & ~ (argsize - 1); \
  memcpy (&list->arg_bytes[list->argindex], argaddr, argsize);

/* Apparently the address of the arglist is the address right after the args
   (which is what you'd expect).  */
#define MAKEVA_END(list) \
  return (va_list) (list->arg_bytes \
		    + list->nargs * list->max_arg_size)
