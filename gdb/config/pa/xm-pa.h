/* Definitions for all PA machines.  */

#define MAKEVA_EXTRA_INFO int arglist_address;

#define MAKEVA_START(list) \
  list->argindex = list->nargs * list->max_arg_size; \
  list->arglist_address = 0;

#define MAKEVA_ARG(list, argaddr, argsize) \
  { \
    int rounded_argsize; \
    if (argsize > 8) \
      /* Currently this never happens; printf_command only uses argsize */ \
      /* of sizeof (int), sizeof (double), or sizeof (long long).  */ \
      error ("MAKEVA_ARG not fully written for hp-pa"); \
    if (argsize <= 4) \
      rounded_argsize = 4; \
    else if (argsize <= 8) \
      rounded_argsize = 8; \
    list->argindex -= rounded_argsize; \
    while ((int)(&list->arg_bytes[list->argindex]) % rounded_argsize) \
      list->argindex--; \
    /* arglist_address is used to store the address of the first arguent.  */ \
    if (list->arglist_address == 0) \
      list->arglist_address = &list->arg_bytes[list->argindex] \
	+ rounded_argsize; \
    memcpy (&list->arg_bytes[list->argindex], argaddr, argsize); \
  }

/* The address of the arglist is the address right after the args
   (which is what you'd expect).  This address is stored in the arglist_address
   field.  */
#define MAKEVA_END(list) \
  return (va_list) list->arglist_address;
