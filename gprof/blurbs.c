#include <stdio.h>
#include "gprof.h"

void
flat_blurb (file)
     FILE *file;
{
   fputs("\n\
 %         the percentage of the total running time of the\n\
time       program used by this function.\n\
\n\
cumulative a running sum of the number of seconds accounted\n\
 seconds   for by this function and those listed above it.\n\
\n\
 self      the number of seconds accounted for by this\n\
seconds    function alone.  This is the major sort for this\n\
           listing.\n\
\n\
calls      the number of times this function was invoked, if\n\
           this function is profiled, else blank.\n\
 \n\
 self      the average number of milliseconds spent in this\n\
ms/call    function per call, if this function is profiled,\n\
	   else blank.\n\
\n\
 total     the average number of milliseconds spent in this\n\
ms/call    function and its descendents per call, if this \n\
	   function is profiled, else blank.\n\
\n\
name       the name of the function.  This is the minor sort\n\
           for this listing. The index shows the location of\n\
	   the function in the gprof listing. If the index is\n\
	   in parenthesis it shows where it would appear in\n\
	   the gprof listing if it were to be printed.\n\
\n", file);
}

static char *callg_blurb_bsd = "\n\
\n\
\n\
call graph profile:\n\
          The sum of self and descendents is the major sort\n\
          for this listing.\n\
\n\
          function entries:\n\
\n\
index     the index of the function in the call graph\n\
          listing, as an aid to locating it (see below).\n\
\n\
%time     the percentage of the total time of the program\n\
          accounted for by this function and its\n\
          descendents.\n\
\n\
self      the number of seconds spent in this function\n\
          itself.\n\
\n\
descendents\n\
          the number of seconds spent in the descendents of\n\
          this function on behalf of this function.\n\
\n\
called    the number of times this function is called (other\n\
          than recursive calls).\n\
\n\
self      the number of times this function calls itself\n\
          recursively.\n\
\n\
name      the name of the function, with an indication of\n\
          its membership in a cycle, if any.\n\
\n\
index     the index of the function in the call graph\n\
          listing, as an aid to locating it.\n\
\n\
\n\
\n\
          parent listings:\n\
\n\
self*     the number of seconds of this function's self time\n\
          which is due to calls from this parent.\n\
\n\
descendents*\n\
          the number of seconds of this function's\n\
          descendent time which is due to calls from this\n\
          parent.\n\
\n\
called**  the number of times this function is called by\n\
          this parent.  This is the numerator of the\n\
          fraction which divides up the function's time to\n\
          its parents.\n\
\n\
total*    the number of times this function was called by\n\
          all of its parents.  This is the denominator of\n\
          the propagation fraction.\n\
\n\
parents   the name of this parent, with an indication of the\n\
          parent's membership in a cycle, if any.\n\
\n\
index     the index of this parent in the call graph\n\
          listing, as an aid in locating it.\n\
\n\
\n\
\n\
          children listings:\n\
\n\
self*     the number of seconds of this child's self time\n\
          which is due to being called by this function.\n\
\n\
descendent*\n\
          the number of seconds of this child's descendent's\n\
          time which is due to being called by this\n\
          function.\n\
\n\
called**  the number of times this child is called by this\n\
          function.  This is the numerator of the\n\
          propagation fraction for this child.\n\
\n\
total*    the number of times this child is called by all\n\
          functions.  This is the denominator of the\n\
          propagation fraction.\n\
\n\
children  the name of this child, and an indication of its\n\
          membership in a cycle, if any.\n\
\n\
index     the index of this child in the call graph listing,\n\
          as an aid to locating it.\n\
\n\
\n\
\n\
          * these fields are omitted for parents (or\n\
          children) in the same cycle as the function.  If\n\
          the function (or child) is a member of a cycle,\n\
          the propagated times and propagation denominator\n\
          represent the self time and descendent time of the\n\
          cycle as a whole.\n\
\n\
          ** static-only parents and children are indicated\n\
          by a call count of 0.\n\
\n\
\n\
\n\
          cycle listings:\n\
          the cycle as a whole is listed with the same\n\
          fields as a function entry.  Below it are listed\n\
          the members of the cycle, and their contributions\n\
          to the time and call counts of the cycle.\n\
\n";

static char *callg_blurb_fsf = "\n\
 This table describes the call tree of the program, and was sorted by\n\
 the total amount of time spent in each function and its children.\n\n\
 Each entry in this table consists of several lines.  The line with the\n\
 index number at the left hand margin lists the current function.\n\
 The lines above it list the functions that called this function,\n\
 and the lines below it list the functions this one called.\n\
 This line lists:\n\
     index	A unique number given to each element of the table.\n\
		Index numbers are sorted numerically.\n\
		The index number is printed next to every function name so\n\
		it is easier to look up where the function in the table.\n\n\
     % time	This is the percentage of the `total' time that was spent\n\
		in this function and its children.  Note that due to\n\
		different viewpoints, functions excluded by options, etc,\n\
		these numbers will NOT add up to 100%.\n\n\
     self	This is the total amount of time spent in this function.\n\n\
     children	This is the total amount of time propagated into this\n\
		function by its children.\n\n\
     called	This is the number of times the function was called.\n\
		If the function called itself recursively, the number\n\
		only includes non-recursive calls, and is followed by\n\
		a `+' and the number of recursive calls.\n\n\
     name	The name of the current function.  The index number is\n\
		printed after it.  If the function is a member of a\n\
		cycle, the cycle number is printed between the\n\
		function's name and the index number.\n\n\n\
 For the function's parents, the fields have the following meanings:\n\n\
     self	This is the amount of time that was propagated directly\n\
		from the function into this parent.\n\n\
     children	This is the amount of time that was propagated from\n\
		the function's children into this parent.\n\n\
     called	This is the number of times this parent called the\n\
		function `/' the total number of times the function\n\
		was called.  Recursive calls to the function are not\n\
		included in the number after the `/'.\n\n\
     name	This is the name of the parent.  The parent's index\n\
		number is printed after it.  If the parent is a\n\
		member of a cycle, the cycle number is printed between\n\
		the name and the index number.\n\n\
 If the parents of the function cannot be determined, the word\n\
 `<spontaneous>' is printed in the `name' field, and all the other\n\
 fields are blank.\n\n\
 For the function's children, the fields have the following meanings:\n\n\
     self	This is the amount of time that was propagated directly\n\
		from the child into the function.\n\n\
     children	This is the amount of time that was propagated from the\n\
		child's children to the function.\n\n\
     called	This is the number of times the function called\n\
		this child `/' the total number of times the child\n\
		was called.  Recursive calls by the child are not\n\
		listed in the number after the `/'.\n\n\
     name	This is the name of the child.  The child's index\n\
		number is printed after it.  If the child is a\n\
		member of a cycle, the cycle number is printed\n\
		between the name and the index number.\n\n\
 If there are any cycles (circles) in the call graph, there is an\n\
 entry for the cycle-as-a-whole.  This entry shows who called the\n\
 cycle (as parents) and the members of the cycle (as children.)\n\
 The `+' recursive calls entry shows the number of function calls that\n\
 were internal to the cycle, and the calls entry for each member shows,\n\
 for that member, how many times it was called from other members of\n\
 the cycle.\n\n";

void
callg_blurb (file)
     FILE *file;
{
    if (bsd_style_output)
	fputs(callg_blurb_bsd, file);
    else
	fputs(callg_blurb_fsf, file);
}
