 .text
/* PR 32391: Automatic counters inside macros should increment when nested
   macros finish execution.  */
 .macro o1
 .global before_at_\@
before_at_\@:
 .global before_plus_\+
before_plus_\+:

 .macro i1
 .global inside_at_\@
inside_at_\@:
 .global inside_plus_\+
inside_plus_\+:
 .endm

 i1
 i1

 .global after_at_\@
after_at_\@:
 .global after_plus_\+
after_plus_\+:

 .endm

/* Invoking o1 should produce these symbols in this order:

   before_at_0
   before_plus_0
   inside_at_1
   inside_plus_0
   inside_at_2
   inside_plus_1
   after_at_0
   after_plus_0  */
 o1

/* A second invocation of o1 should not produce any errors about
   symbols or macros being redefined.  */
 o1

/* This definition should not collide with the definition inside o1.  */
 .macro i1
 .global other_inner_\@
other_inner_\@:
 .endm

/* And invoking it should invoke the second defintion of i1, not the first.  */
 i1

 .macro o2
 .global _m\@_
_m\@_:
 .macro i2
 .global _m\@_
_m\@_:
 .endm
 i2
 .endm

/* This should not generate conflicting symbols because the assembler
   inserts the contents of o2 into the input buffer as pure text (ie
   without evaluating i2).  The first use of \@ is evaluated at this
   time, creating _m4_.  But the second use is not evaluated because
   it is inside a .macro definition.

   This finishes the evaluation of o2, so the \@ counter is incremented.

   Next the input buffer is re-evaluated and the i2 macro definition
   and invocation are encounterd.  The text from i2 are inserted into
   the input buffer and at this point the second use of \@ is evaluated
   resulting in the creation of a symbol called _m5_.  */
 o2

/* Macro arguments should be independent of nesting.  */
 .macro O3 arg
 .global o3_\arg
o3_\arg:

 .macro I3 arg
 .global i3_\arg
i3_\arg:
 .endm

 i3 bar		/* Macro names are case insensitive.  */
 .endm

 o3 foo		/* Should produce two labels: o3_foo and i3_bar.  */

/* Nested macros can access the arguments of their parents.
   In addition their arguments can be substituted into the arguments
   that are substited from their parents:  */
 .macro OUTER arg1, arg2, arg3:vararg
 .macro INNER arg4 arg2
        .dc.a \arg2
 .dc.a \arg3
 .endm
 INNER \arg1 bert
 .dc.a \arg2
 .endm

/* This produces references to "jim", "bert" and "harryfred".  */
 OUTER fred, jim, harry\arg4
