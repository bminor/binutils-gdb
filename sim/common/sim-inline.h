/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation, Inc.

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SIM_INLINE_H_
#define _SIM_INLINE_H_


/* INLINE CODE SELECTION:

   GCC -O3 attempts to inline any function or procedure in scope.  The
   options below facilitate fine grained control over what is and what
   isn't made inline.  For instance it can control things down to a
   specific modules static routines.  Doing this allows the compiler
   to both eliminate the overhead of function calls and (as a
   consequence) also eliminate further dead code.

   On a CISC (x86) I've found that I can achieve an order of magintude
   speed improvement (x3-x5).  In the case of RISC (sparc) while the
   performance gain isn't as great it is still significant.

   Each module is controled by the macro <module>_INLINE which can
   have the values described below

       0  Do not inline any thing for the given module

   The following additional values are `bit fields' and can be
   combined.

      REVEAL_MODULE:

         Include the C file for the module into the file being compiled
         but do not make the functions within the module inline.

	 While of no apparent benefit, this makes it possible for the
	 included module, when compiled to inline its calls to what
	 would otherwize be external functions.

      INLINE_MODULE:

         Make external functions within the module `inline'.  Thus if
         the module is included into a file being compiled, calls to
	 its funtions can be eliminated. 2 implies 1.

      INLINE_LOCALS:

         Make internal (static) functions within the module `inline'.

   In addition to this, modules have been put into two categories.

      INCLUDED_BY_MODULE

         eg sim-endian.h sim-bits.h

	 Because these modules are small and simple and do not have
	 any complex interdendencies they are configured, if
	 <module>_INLINE is so enabled, to inline themselves in all
	 modules that include those files.

	 For the default build, this is a real win as all byte
	 conversion and bit manipulation functions are inlined.

      !INCLUDED_BY_MODULE

	 Complex modules - the rest

	 These are all handled using the files sim-inline.h and
	 sim-inline.c.  The main simulator engine includes both of
	 these and hence includes all remaining code.

   The following abreviations are available:

      INCLUDE_MODULE == (REVEAL_MODULE | INLINE_MODULE)

      ALL_INLINE == (REVEAL_MODULE | INLINE_MODULE | INLINE_LOCALS)

      ALL_BY_MODULE = (INCLUDED_BY_MODULE | ALL_INLINE)

   IMPLEMENTATION:

   The inline ability is enabled by prefixing every data / function
   declaration and definition with one of the following:


       INLINE_<module>

       Prefix to any global function that is a candidate for being
       inline.

       values - `', `static', `static INLINE'


       EXTERN_<module>
      
       Prefix to any global data structures for the module.  Global
       functions that are not to be inlined shall also be prefixed
       with this.

       values - `', `static', `static'


       STATIC_INLINE_<module>

       Prefix to any local (static) function that is a candidate for
       being made inline.

       values - `static', `static INLINE'


       static

       Prefix all local data structures.  Local functions that are not
       to be inlined shall also be prefixed with this.

       values - `static', `static'

       nb: will not work for modules that are being inlined for every
       use (white lie).


       extern
       #ifndef _SIM_INLINE_C_
       #endif
       
       Prefix to any declaration of a global object (function or
       variable) that should not be inlined and should have only one
       definition.  The #ifndef wrapper goes around the definition
       propper to ensure that only one copy is generated.

       nb: this will not work when a module is being inlined for every
       use.


       STATIC_<module>

       Replaced by either `static' or `EXTERN_MODULE'.


   REALITY CHECK:

   This is not for the faint hearted.  I've seen GCC get up to 500mb
   trying to compile what this can create.

   Some of the modules do not yet implement the WITH_INLINE_STATIC
   option.  Instead they use the macro STATIC_INLINE to control their
   local function.

   Because of the way that GCC parses __attribute__(), the macro's
   need to be adjacent to the functioin name rather then at the start
   of the line vis:

   	int STATIC_INLINE_MODULE f(void);
	void INLINE_MODULE *g(void);

   */


#define REVEAL_MODULE			1
#define INLINE_MODULE			2
#define INCLUDE_MODULE			(INLINE_MODULE | REVEAL_MODULE)
#define INLINE_LOCALS			4
#define ALL_INLINE			7
#define ALL_BY_MODULE			(INCLUDED_BY_MODULE | ALL_INLINE)

#define INCLUDED_BY_MODULE             16
#define REGPARM_MODULE                 32


/* Default macro to simplify control several of key the inlines */

#ifndef DEFAULT_INLINE
#define	DEFAULT_INLINE			INLINE_LOCALS
#endif



/* Your compilers inline prefix */

#ifndef INLINE
#if defined(__GNUC__) && defined(__OPTIMIZE__)
#define INLINE __inline__
#else
#define INLINE /*inline*/
#endif
#endif



/* Your compiler's static prefix */

#ifndef STATIC_INLINE
#define STATIC_INLINE static INLINE
#endif



/* Your compiler's no-return reserved word */

#ifndef NORETURN
#define NORETURN
#endif



/* Your compilers's unused reserved word */

#if !defined (UNUSED)
#if (!defined(__GNUC__) \
     || (__GNUC__ < 2) \
     || (__GNUC__ == 2 && __GNUC_MINOR__ < 7))
#define UNUSED
#else
#define UNUSED __attribute__((__unused__))
#endif
#endif




/* Your compilers nonstandard function call mechanism prefix */

#if !defined REGPARM
#if defined(__GNUC__) && (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__))
#if (WITH_REGPARM && WITH_STDCALL)
#define REGPARM __attribute__((__regparm__(WITH_REGPARM),__stdcall__))
#else
#if (WITH_REGPARM && !WITH_STDCALL)
#define REGPARM __attribute__((__regparm__(WITH_REGPARM)))
#else
#if (!WITH_REGPARM && WITH_STDCALL)
#define REGPARM __attribute__((__stdcall__))
#endif
#endif
#endif
#endif
#endif

#if !defined REGPARM
#define REGPARM
#endif



/* sim-bits */

#if (SIM_BITS_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_BITS REGPARM
#else
# define REGPARM_SIM_BITS
#endif

#if (((SIM_BITS_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_BITS_C_) && (SIM_BITS_INLINE & INCLUDE_MODULE))
# if (SIM_BITS_INLINE & INLINE_MODULE)
#  define INLINE_SIM_BITS(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_BITS(TYPE) static TYPE UNUSED REGPARM_SIM_BITS
# else
#  define INLINE_SIM_BITS(TYPE) static TYPE UNUSED REGPARM_SIM_BITS
#  define EXTERN_SIM_BITS(TYPE) static TYPE UNUSED REGPARM_SIM_BITS
# endif
#else
# define INLINE_SIM_BITS(TYPE) TYPE REGPARM_SIM_BITS
# define EXTERN_SIM_BITS(TYPE) TYPE REGPARM_SIM_BITS
#endif

#if (SIM_BITS_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_BITS(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_BITS(TYPE) static TYPE REGPARM_SIM_BITS
#endif



/* sim-core */

#if (SIM_CORE_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_CORE REGPARM
#else
# define REGPARM_SIM_CORE
#endif

#if (((SIM_CORE_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_CORE_C_) && (SIM_CORE_INLINE & INCLUDE_MODULE))
# if (SIM_CORE_INLINE & INLINE_MODULE)
#  define INLINE_SIM_CORE(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_CORE(TYPE) static TYPE UNUSED REGPARM_SIM_CORE
#else
#  define INLINE_SIM_CORE(TYPE) static TYPE UNUSED REGPARM_SIM_CORE
#  define EXTERN_SIM_CORE(TYPE) static TYPE UNUSED REGPARM_SIM_CORE
#endif
#else
# define INLINE_SIM_CORE(TYPE) TYPE REGPARM_SIM_CORE
# define EXTERN_SIM_CORE(TYPE) TYPE REGPARM_SIM_CORE
#endif

#if (SIM_CORE_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_CORE(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_CORE(TYPE) static TYPE REGPARM_SIM_CORE
#endif



/* sim-endian */

#if (SIM_ENDIAN_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_ENDIAN REGPARM
#else
# define REGPARM_SIM_ENDIAN
#endif

#if (((SIM_ENDIAN_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_ENDIAN_C_) && (SIM_ENDIAN_INLINE & INCLUDE_MODULE))
# if (SIM_ENDIAN_INLINE & INLINE_MODULE)
#  define INLINE_SIM_ENDIAN(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_ENDIAN(TYPE) static TYPE UNUSED REGPARM_SIM_ENDIAN
# else
#  define INLINE_SIM_ENDIAN(TYPE) static TYPE UNUSED REGPARM_SIM_ENDIAN
#  define EXTERN_SIM_ENDIAN(TYPE) static TYPE UNUSED REGPARM_SIM_ENDIAN
# endif
#else
# define INLINE_SIM_ENDIAN(TYPE) TYPE REGPARM_SIM_ENDIAN
# define EXTERN_SIM_ENDIAN(TYPE) TYPE REGPARM_SIM_ENDIAN
#endif

#if (SIM_ENDIAN_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_ENDIAN(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_ENDIAN(TYPE) static TYPE REGPARM_SIM_ENDIAN
#endif



/* sim-events */

#if (SIM_EVENTS_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_EVENTS REGPARM
#else
# define REGPARM_SIM_EVENTS
#endif

#if (((SIM_EVENTS_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_EVENTS_C_) && (SIM_EVENTS_INLINE & INCLUDE_MODULE))
# if (SIM_EVENTS_INLINE & INLINE_MODULE)
#  define INLINE_SIM_EVENTS(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_EVENTS(TYPE) static TYPE UNUSED REGPARM_SIM_EVENTS
# else
#  define INLINE_SIM_EVENTS(TYPE) static TYPE UNUSED REGPARM_SIM_EVENTS
#  define EXTERN_SIM_EVENTS(TYPE) static TYPE UNUSED REGPARM_SIM_EVENTS
# endif
#else
# define INLINE_SIM_EVENTS(TYPE) TYPE REGPARM_SIM_EVENTS
# define EXTERN_SIM_EVENTS(TYPE) TYPE REGPARM_SIM_EVENTS
#endif

#if (SIM_EVENTS_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_EVENTS(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_EVENTS(TYPE) static TYPE REGPARM_SIM_EVENTS
#endif



/* sim-fpu */

#if (SIM_FPU_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_FPU REGPARM
#else
# define REGPARM_SIM_FPU
#endif

#if (((SIM_FPU_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_FPU_C_) && (SIM_FPU_INLINE & INCLUDE_MODULE))
# if (SIM_FPU_INLINE & INLINE_MODULE)
#  define INLINE_SIM_FPU(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_FPU(TYPE) static TYPE UNUSED REGPARM_SIM_FPU
# else
#  define INLINE_SIM_FPU(TYPE) static TYPE UNUSED REGPARM_SIM_FPU
#  define EXTERN_SIM_FPU(TYPE) static TYPE UNUSED REGPARM_SIM_FPU
# endif
#else
# define INLINE_SIM_FPU(TYPE) TYPE REGPARM_SIM_FPU
# define EXTERN_SIM_FPU(TYPE) TYPE REGPARM_SIM_FPU
#endif

#if (SIM_FPU_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_FPU(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_FPU(TYPE) static TYPE REGPARM_SIM_FPU
#endif



/* sim-types */

#if (SIM_TYPES_INLINE & REGPARM_MODULE)
# define REGPARM_SIM_TYPES REGPARM
#else
# define REGPARM_SIM_TYPES
#endif

#if (((SIM_TYPES_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SIM_TYPES_C_) && (SIM_TYPES_INLINE & INCLUDE_MODULE))
# if (SIM_TYPES_INLINE & INLINE_MODULE)
#  define INLINE_SIM_TYPES(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SIM_TYPES(TYPE) static TYPE UNUSED REGPARM_SIM_TYPES
# else
#  define INLINE_SIM_TYPES(TYPE) static TYPE UNUSED REGPARM_SIM_TYPES
#  define EXTERN_SIM_TYPES(TYPE) static TYPE UNUSED REGPARM_SIM_TYPES
# endif
#else
# define INLINE_SIM_TYPES(TYPE) TYPE REGPARM_SIM_TYPES
# define EXTERN_SIM_TYPES(TYPE) TYPE REGPARM_SIM_TYPES
#endif

#if (SIM_TYPES_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SIM_TYPES(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SIM_TYPES(TYPE) static TYPE REGPARM_SIM_TYPES
#endif



/* icache */

#if (ICACHE_INLINE & REGPARM_MODULE)
# define REGPARM_ICACHE REGPARM
#else
# define REGPARM_ICACHE
#endif

#if (((ICACHE_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_ICACHE_C_) && (ICACHE_INLINE & INCLUDE_MODULE))
# if (ICACHE_INLINE & INLINE_MODULE)
#  define INLINE_ICACHE(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_ICACHE(TYPE) static TYPE UNUSED REGPARM_ICACHE
#else
#  define INLINE_ICACHE(TYPE) static TYPE UNUSED REGPARM_ICACHE
#  define EXTERN_ICACHE(TYPE) static TYPE UNUSED REGPARM_ICACHE
#endif
#else
# define INLINE_ICACHE(TYPE) TYPE REGPARM_ICACHE
# define EXTERN_ICACHE(TYPE) TYPE REGPARM_ICACHE
#endif

#if (ICACHE_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_ICACHE(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_ICACHE(TYPE) static TYPE REGPARM_ICACHE
#endif



/* idecode */

#if (IDECODE_INLINE & REGPARM_MODULE)
# define REGPARM_IDECODE REGPARM
#else
# define REGPARM_IDECODE
#endif

#if (((IDECODE_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_IDECODE_C_) && (IDECODE_INLINE & INCLUDE_MODULE))
# if (IDECODE_INLINE & INLINE_MODULE)
#  define INLINE_IDECODE(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_IDECODE(TYPE) static TYPE UNUSED REGPARM_IDECODE
#else
#  define INLINE_IDECODE(TYPE) static TYPE UNUSED REGPARM_IDECODE
#  define EXTERN_IDECODE(TYPE) static TYPE UNUSED REGPARM_IDECODE
#endif
#else
# define INLINE_IDECODE(TYPE) TYPE REGPARM_IDECODE
# define EXTERN_IDECODE(TYPE) TYPE REGPARM_IDECODE
#endif

#if (IDECODE_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_IDECODE(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_IDECODE(TYPE) static TYPE REGPARM_IDECODE
#endif



/* semantics */

#if (SEMANTICS_INLINE & REGPARM_MODULE)
# define REGPARM_SEMANTICS REGPARM
#else
# define REGPARM_SEMANTICS
#endif

#if (((SEMANTICS_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SEMANTICS_C_) && (SEMANTICS_INLINE & INCLUDE_MODULE))
# if (SEMANTICS_INLINE & INLINE_MODULE)
#  define INLINE_SEMANTICS(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SEMANTICS(TYPE) static TYPE UNUSED REGPARM_SEMANTICS
#else
#  define INLINE_SEMANTICS(TYPE) static TYPE UNUSED REGPARM_SEMANTICS
#  define EXTERN_SEMANTICS(TYPE) static TYPE UNUSED REGPARM_SEMANTICS
#endif
#else
# define INLINE_SEMANTICS(TYPE) TYPE REGPARM_SEMANTICS
# define EXTERN_SEMANTICS(TYPE) TYPE REGPARM_SEMANTICS
#endif

#if (SEMANTICS_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SEMANTICS(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SEMANTICS(TYPE) static TYPE REGPARM_SEMANTICS
#endif



/* support */

#if (SUPPORT_INLINE & REGPARM_MODULE)
# define REGPARM_SUPPORT REGPARM
#else
# define REGPARM_SUPPORT
#endif

#if (((SUPPORT_INLINE & INCLUDED_BY_MODULE) || defined(_SIM_INLINE_C_)) \
     && !defined(_SUPPORT_C_) && (SUPPORT_INLINE & INCLUDE_MODULE))
# if (SUPPORT_INLINE & INLINE_MODULE)
#  define INLINE_SUPPORT(TYPE) static INLINE TYPE UNUSED
#  define EXTERN_SUPPORT(TYPE) static TYPE UNUSED REGPARM_SUPPORT
#else
#  define INLINE_SUPPORT(TYPE) static TYPE UNUSED REGPARM_SUPPORT
#  define EXTERN_SUPPORT(TYPE) static TYPE UNUSED REGPARM_SUPPORT
#endif
#else
# define INLINE_SUPPORT(TYPE) TYPE REGPARM_SUPPORT
# define EXTERN_SUPPORT(TYPE) TYPE REGPARM_SUPPORT
#endif

#if (SUPPORT_INLINE & INLINE_LOCALS)
# define STATIC_INLINE_SUPPORT(TYPE) static INLINE TYPE
#else
# define STATIC_INLINE_SUPPORT(TYPE) static TYPE REGPARM_SUPPORT
#endif



#endif
