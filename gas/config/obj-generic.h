/*
 * This file is obj-generic.h and is intended to be a template for
 * object format specific header files. 
 */

 /* define an obj specific macro off which target cpu back ends may key. */
#define OBJ_GENERIC 1

 /* include whatever target cpu is appropriate. */
#include "targ-cpu.h"

/*
 * SYMBOLS
 */

/*
 * If your object format needs to reorder symbols, define this.  When
 * defined, symbols are kept on a doubly linked list and functions are
 * made available for push, insert, append, and delete.  If not defined,
 * symbols are kept on a singly linked list, only the append and clear
 * facilities are available, and they are macros.
 */

 /* #define SYMBOLS_NEED_PACKPOINTERS */

 /*  */
typedef struct {
	void *nothing;
} obj_symbol_type; /* should be the format's symbol structure */

typedef void *object_headers;

 /* symbols have names */
#define S_GET_NAME(s)		("foo") /* get the name of a symbolP */
#define S_SET_NAME(s,v)		;
 /* symbols have segments */
#define S_GET_SEGMENT(s)	(SEG_UNKNOWN)
#define S_SET_SEGMENT(s,v)	;
 /* symbols have a value */
#define S_GET_VALUE(s)		(0)
#define S_SET_VALUE(s,v)	;
 /* symbols may be external */
#define S_IS_EXTERNAL(s)	(0)
#define S_SET_EXTERNAL(s)	;

 /* symbols may or may not be defined */
#define S_IS_DEFINED(s)		(0)


#define DEFAULT_MAGIC_NUMBER_FOR_OBJECT_FILE (0) /* your magic number */

#define OBJ_EMIT_LINENO(a,b,c) /* must be *something*.  This no-op's it out. */

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of obj-generic.h */
