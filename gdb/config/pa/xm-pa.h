/* Definitions for all PA machines.  */

#define MAKEVA_START(list) \
  list->argindex = list->nargs * list->max_arg_size;

#define MAKEVA_ARG(list, argaddr, argsize) \
  if (argsize > 8) \
    /* Currently this never happens; printf_command only uses argsize */ \
    /* of sizeof (int), sizeof (double), or sizeof (long long).  */ \
    error ("MAKEVA_ARG not fully written for hp-pa"); \
  memcpy (&list->arg_bytes[list->argindex - argsize], argaddr, argsize); \
  if (argsize <= 4) \
    list->argindex -= 4; \
  else if (argsize <= 8) \
    list->argindex -= 8; \

/* The address of the arglist is the address right after the args
   (which is what you'd expect).  */
#define MAKEVA_END(list) \
  return (va_list) (list->arg_bytes \
		    + list->nargs * list->max_arg_size)
