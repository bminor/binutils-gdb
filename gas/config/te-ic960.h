/*
 * This file is te-ic960.h and is intended to define ic960 environment
 * specific differences.
 */

#define TE_IC960 1

/* intel uses host byte order for headers */
#ifdef CROSS_COMPILE
#undef CROSS_COMPILE
#endif /* CROSS_COMPILE */

#define OBJ_COFF_OMIT_OPTIONAL_HEADER
#define LOCAL_LABEL(name) ( (name[0] =='L') \
			   || (name[0] =='.' \
			       && (name[1]=='C' || name[1]=='I' || name[1]=='.')))
#include "obj-format.h"

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of te-ic960.h */
