/*> support.h <*/

#ifndef __support_h
#define __support_h

/* For 64bit values either use the GCC "long long" feature, or have a
   structure made from two 32bit values. We can then have macros for
   accessing the LO and HI parts of the value. Also we can provide
   macros for the basic operations we want to perform on 64bit values
   (i.e. ADD64,SUB64,SHIFTLEFT64, etc.). This means we should be able
   to host the simulator on non-GCC compilers, and 32bit only
   architectures if desired. */

/* Control via a build boolean for the moment */
#if 1 && defined(__GNUC__)

typedef long long word64;
typedef unsigned long long uword64;

#define WORD64LO(t)     (unsigned int)((t)&0xFFFFFFFF)
#define WORD64HI(t)     (unsigned int)((t)>>32)

/* Sign-extend the given value (e) as a value (b) bits long. We cannot
   assume the HI32bits of the operand are zero, so we must perform a
   mask to ensure we can use the simple subtraction to sign-extend. */
#define SIGNEXTEND(e,b) (((e) & ((unsigned long long)1 << ((b) - 1))) ? (((e) & (((unsigned long long)1 << (b)) - 1)) - ((unsigned long long)1 << (b))) : (e))

/* Check if a value will fit within a word (unsigned int): */
#define NOTWORDVALUE(v) ((((((unsigned long long)(v)>>32) == 0) && !((v) & ((unsigned)1 << 31))) || ((((unsigned long long)(v)>>32) == 0xFFFFFFFF) && ((v) & ((unsigned)1 << 31)))) ? (1 == 0) : (1 == 1))

/* Check if a value will fit within a halfword: */
#define NOTHALFWORDVALUE(v) ((((((unsigned long long)(v)>>16) == 0) && !((v) & ((unsigned)1 << 15))) || (((((unsigned long long)(v)>>32) == 0xFFFFFFFF) && ((((unsigned long long)(v)>>16) & 0xFFFF) == 0xFFFF)) && ((v) & ((unsigned)1 << 15)))) ? (1 == 0) : (1 == 1))

/* The following should be executed once at the start of day in the
   main emulator control function. The simulator assumes char is
   8bits, and from this: */
#define CHECKSIM() {\
                     if (sizeof(int) != (4 * sizeof(char)))\
                      SignalException(SimulatorFault,"sizeof(int) != 4");\
                     if (sizeof(long long) != (8 * sizeof(char)))\
                      SignalException(SimulatorFault,"sizeof(long long) != 8");\
                   }

#else /* non-GCC build */

#error "non-GCC build to be completed" /* avoid using long long */

typedef struct word64 {
 unsigned int lo;
 unsigned int hi;
} word64;

#define WORD64LO(t)     (unsigned int)(t.lo)
#define WORD64HI(t)     (unsigned int)(t.hi)

/* TODO: Update these to manipulate the split structure values */
#define SIGNEXTEND(e,b)         /* TODO */
#define NOTWORDVALUE(v)         /* TODO */
#define NOTHALFWORDVALUE(v)     /* TODO */

/* The following should be executed once at the start of day in the
   main emulator control function. The simulator assumes char is
   8bits, and from this: */
#define CHECKSIM() {\
                     if (sizeof(int) != (4 * sizeof(char)))\
                      SignalException(SimulatorFault,"sizeof(int) != 4");\a
                   }

#endif /* non-GCC build */

#endif /* __support_h */

/*> EOF support.h <*/
