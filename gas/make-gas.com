$! Set the def dir to proper place for use in batch. Works for interactive to.
$flnm = f$enviroment("PROCEDURE")     ! get current procedure name
$set default 'f$parse(flnm,,,"DEVICE")''f$parse(flnm,,,"DIRECTORY")'
$!
$!	Command file to build a GNU assembler on VMS
$!
$!	If you are using a version of GCC that supports global constants
$!	you should remove the define="const=" from the gcc lines.
$!
$!	Caution:  Versions 1.38.1 and earlier had a bug in the handling of
$!	some static constants. If you are using such a version of the
$!	assembler, and you wish to compile without the "const=" hack,
$!	you should first build this version *with* the "const="
$!	definition, and then use that assembler to rebuild it without the
$!	"const=" definition.  Failure to do this will result in an assembler
$!	that will mung floating point constants.
$!
$!	Note: The version of gas shipped on the GCC VMS tapes has been patched
$!	to fix the above mentioned bug.
$!
$ write sys$output "If this assembler is going to be used with GCC 1.n, you"
$ write sys$Output "need to modify the driver to supply the -1 switch to gas."
$ write sys$output "This is required because of a small change in how global"
$ write sys$Output "constant variables are handled.  Failure to include this"
$ write sys$output "will result in linker warning messages about mismatched
$ write sys$output "psect attributes."
$!
$ C_DEFS :="""VMS"""
$! C_DEFS :="""VMS""","""const="""
$ C_INCLUDES :=/include=([],[.config],[-.include],[-.include.aout])
$ C_FLAGS := /debug 'c_includes'
$!
$!
$ if "''p1'" .eqs. "LINK" then goto Link
$!
$!  This helps gcc 1.nn find the aout/* files.
$!
$ aout_dev = f$parse(flnm,,,"DEVICE")
$ tmp = aout_dev - ":"
$if f$trnlnm(tmp).nes."" then aout_dev = f$trnlnm(tmp)
$ aout_dir = aout_dev+f$parse(flnm,,,"DIRECTORY")' -
	- "GAS]" + "INCLUDE.AOUT.]" - "]["
$assign 'aout_dir' aout/tran=conc
$ opcode_dir = aout_dev+f$parse(flnm,,,"DIRECTORY")' -
	- "GAS]" + "INCLUDE.OPCODE.]" - "]["
$assign 'opcode_dir' opcode/tran=conc
$!
$ gcc 'c_flags'/define=('C_DEFS') targ-cpu.c
$ gcc 'c_flags'/define=('C_DEFS') obj-format.c
$ gcc 'c_flags'/define=('C_DEFS') atof-targ.c
$ gcc 'c_flags'/define=('C_DEFS') app.c
$ gcc 'c_flags'/define=('C_DEFS') as.c
$ gcc 'c_flags'/define=('C_DEFS') atof-generic.c
$ gcc 'c_flags'/define=('C_DEFS') bignum-copy.c
$ gcc 'c_flags'/define=('C_DEFS') cond.c
$ gcc 'c_flags'/define=('C_DEFS') expr.c
$ gcc 'c_flags'/define=('C_DEFS') flonum-konst.c
$ gcc 'c_flags'/define=('C_DEFS') flonum-copy.c
$ gcc 'c_flags'/define=('C_DEFS') flonum-mult.c
$ gcc 'c_flags'/define=('C_DEFS') frags.c
$ gcc 'c_flags'/define=('C_DEFS') hash.c
$ gcc 'c_flags'/define=('C_DEFS') hex-value.c
$ gcc 'c_flags'/define=('C_DEFS') input-file.c
$ gcc 'c_flags'/define=('C_DEFS') input-scrub.c
$ gcc 'c_flags'/define=('C_DEFS') literal.c
$ gcc 'c_flags'/define=('C_DEFS') messages.c
$ gcc 'c_flags'/define=('C_DEFS') output-file.c
$ gcc 'c_flags'/define=('C_DEFS') read.c
$ gcc 'c_flags'/define=('C_DEFS') subsegs.c
$ gcc 'c_flags'/define=('C_DEFS') symbols.c
$ gcc 'c_flags'/define=('C_DEFS') write.c
$ gcc 'c_flags'/define=('C_DEFS') listing.c
$ gcc 'c_flags'/define=('C_DEFS') ecoff.c
$ gcc 'c_flags'/define=('C_DEFS') stabs.c
$ gcc 'c_flags'/define=('C_DEFS') xmalloc.c
$ gcc 'c_flags'/define=('C_DEFS') [-.libiberty]obstack.c
$ gcc 'c_flags'/define=('C_DEFS') [-.libiberty]strdup.c
$if f$search("obstack.obj").eqs."" then copy [-.libiberty]obstack.obj *.*
$if f$search("strdup.obj").eqs."" then copy [-.libiberty]strdup.obj *.*
$ link/nomap/exec=gcc-as version.opt/opt+sys$input:/opt
!
!	Linker options file for GNU assembler
!
targ-cpu.o,-
obj-format.o,-
atof-targ.o,-
app.o,-
as.o,-
atof-generic.o,-
bignum-copy.o,-
cond.o,-
expr.o,-
flonum-konst.o,-
flonum-copy.o,-
flonum-mult.o,-
frags.o,-
hash.o,-
hex-value.o,-
input-file.o,-
input-scrub.o,-
literal.o,-
messages.o,-
output-file.o,-
read.o,-
subsegs.o,-
symbols.o,-
write.o,-
listing.o,-
ecoff.o,-
stabs.o,-
xmalloc.o,-
obstack.o,-
strdup.o,-
gnu_cc:[000000]gcclib/lib,sys$share:vaxcrtl/lib
