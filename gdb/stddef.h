#ifndef _STDDEF_H
#define _STDDEF_H

/* Signed type of difference of two pointers.  */

typedef long ptrdiff_t;

/* Unsigned type of `sizeof' something.  */

#ifndef _SIZE_T	/* in case <sys/types.h> has defined it. */
#define _SIZE_T
typedef unsigned long size_t;
#endif /* _SIZE_T */

/* A null pointer constant.  */

#undef NULL		/* in case <stdio.h> has defined it. */
#define NULL 0

/* Offset of member MEMBER in a struct of type TYPE.  */

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif /* _STDDEF_H */
