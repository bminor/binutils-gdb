#ifndef _STDDEF_H
#define _STDDEF_H

/* Signed type of difference of two pointers.  */

typedef long ptrdiff_t;

/* Unsigned type of `sizeof' something.  */

/* in case <sys/types.h> has defined it. */
/* DECstation uses _SIZE_T_.  */
#if !defined (_SIZE_T) && !defined (_SIZE_T_)
#define _SIZE_T
typedef unsigned long size_t;
#endif /* _SIZE_T */

/* A null pointer constant.  */

#undef NULL		/* in case <stdio.h> has defined it. */
#define NULL 0

/* Offset of member MEMBER in a struct of type TYPE.  */

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif /* _STDDEF_H */
