# OBSOLETE # Sed commands that finish translating the GDB Unix Makefile to MPW syntax.
# OBSOLETE 
# OBSOLETE /^host_alias =/s/^/#/
# OBSOLETE /^target_alias =/s/^/#/
# OBSOLETE 
# OBSOLETE /^host_makefile_frag@$/d
# OBSOLETE /^target_makefile_frag@$/d
# OBSOLETE 
# OBSOLETE /@ENABLE_CFLAGS@/s/@ENABLE_CFLAGS@/{ENABLE_CFLAGS}/g
# OBSOLETE /^ENABLE_CFLAGS=/s/^/#/
# OBSOLETE 
# OBSOLETE # Edit all the symbolic definitions pointing to various libraries and such.
# OBSOLETE 
# OBSOLETE /^INCLUDE_DIR = /s/"{srcdir}":include/"{topsrcdir}"include:/
# OBSOLETE 
# OBSOLETE /^MMALLOC_DIR = /s/::mmalloc/mmalloc:/
# OBSOLETE /^MMALLOC_SRC = /s/"{srcdir}"/"{topsrcdir}"/
# OBSOLETE /^MMALLOC =/s/=.*$/=/
# OBSOLETE /MMALLOC_CFLAGS =/s/=.*$/= -u USE_MMALLOC/
# OBSOLETE 
# OBSOLETE /^BFD_DIR = /s/::bfd/bfd:/
# OBSOLETE /^BFD = /s/{BFD_DIR}:libbfd/{BFD_DIR}libbfd/
# OBSOLETE /^BFD_SRC = /s/"{srcdir}"/"{topsrcdir}"/
# OBSOLETE 
# OBSOLETE /^READLINE_DIR = /s/::readline/readline:/
# OBSOLETE /^READLINE =/s/=.*$/=/
# OBSOLETE /^READLINE_SRC = /s/"{srcdir}"/"{topsrcdir}"/
# OBSOLETE 
# OBSOLETE /^INCLUDE_CFLAGS = /s/$/ -i "{topsrcdir}"include:mpw: -i ::extra-include:/
# OBSOLETE 
# OBSOLETE /^SER_HARDWIRE =/s/ser-unix/ser-mac/
# OBSOLETE 
# OBSOLETE /^TERMCAP =/s/ =.*$/ =/
# OBSOLETE 
# OBSOLETE # Whack out various autoconf vars that we don't need.
# OBSOLETE /@CONFIG_LDFLAGS@/s/@CONFIG_LDFLAGS@//g
# OBSOLETE /@HLDFLAGS@/s/@HLDFLAGS@//g
# OBSOLETE /@DEFS@/s/@DEFS@//g
# OBSOLETE /@YACC@/s/@YACC@/byacc/g
# OBSOLETE /@ENABLE_OBS@/s/@ENABLE_OBS@//g
# OBSOLETE /@ENABLE_CLIBS@/s/@ENABLE_CLIBS@//g
# OBSOLETE /@LIBS@/s/@LIBS@//g
# OBSOLETE 
# OBSOLETE # Whack out autoconf hook for thread debugging.
# OBSOLETE /@THREAD_DB_OBS@/s/@THREAD_DB_OBS@//g
# OBSOLETE 
# OBSOLETE # Fix up paths to include directories.
# OBSOLETE /INCLUDE_DIR/s/"{s}"{INCLUDE_DIR}/{INCLUDE_DIR}/g
# OBSOLETE /INCLUDE_DIR/s/{INCLUDE_DIR}:/{INCLUDE_DIR}/g
# OBSOLETE /INCLUDE_DIR/s/"{INCLUDE_DIR}":/"{INCLUDE_DIR}"/g
# OBSOLETE 
# OBSOLETE /{BFD_DIR}/s/"{BFD_DIR}":/"{BFD_DIR}"/g
# OBSOLETE /{BFD_DIR}/s/\([ 	]\){BFD_DIR}/\1::{BFD_DIR}/g
# OBSOLETE /{BFD_DIR}/s/\([ 	]\)"{BFD_DIR}"/\1::"{BFD_DIR}"/g
# OBSOLETE 
# OBSOLETE /{BFD_SRC}/s/"{s}"{BFD_SRC}/{BFD_SRC}/g
# OBSOLETE /{BFD_SRC}/s/{BFD_SRC}:/{BFD_SRC}/g
# OBSOLETE 
# OBSOLETE /{READLINE_SRC}/s/"{s}"{READLINE_SRC}/{READLINE_SRC}/g
# OBSOLETE 
# OBSOLETE /^readline_headers =/,/^$/c\
# OBSOLETE readline_headers =\
# OBSOLETE 
# OBSOLETE 
# OBSOLETE # This isn't really useful, and seems to cause nonsensical complaints.
# OBSOLETE /{ALLDEPFILES}/s/{ALLDEPFILES}//g
# OBSOLETE 
# OBSOLETE /^copying.c \\Option-f /,/^$/d
# OBSOLETE 
# OBSOLETE # Fix the syntax of bits of C code that go into version.c.
# OBSOLETE /char /s/'char .Option-x/'char */
# OBSOLETE 
# OBSOLETE # Point at files in the obj dir rather than src dir.
# OBSOLETE /version/s/"{s}"version\.c/"{o}"version.c/g
# OBSOLETE /version/s/^version\.c/"{o}"version.c/
# OBSOLETE /config/s/"{s}"config\.h/"{o}"config.h/g
# OBSOLETE /config/s/^config\.h/"{o}"config.h/
# OBSOLETE /xm/s/"{s}"xm\.h/"{o}"xm.h/g
# OBSOLETE /xm/s/^xm\.h/"{o}"xm.h/
# OBSOLETE /tm/s/"{s}"tm\.h/"{o}"tm.h/g
# OBSOLETE /tm/s/^tm\.h/"{o}"tm.h/
# OBSOLETE /nm/s/"{s}"nm\.h/"{o}"nm.h/g
# OBSOLETE /nm/s/^nm\.h/"{o}"nm.h/
# OBSOLETE 
# OBSOLETE /exp.tab.c/s/"{s}"\([a-z0-9]*\)-exp\.tab\.c/"{o}"\1-exp.tab.c/g
# OBSOLETE /exp.tab.c/s/^\([a-z0-9]*\)-exp\.tab\.c/"{o}"\1-exp.tab.c/
# OBSOLETE 
# OBSOLETE /y.tab/s/"{s}"y.tab\.c/"{o}"y.tab.c/g
# OBSOLETE /y.tab/s/^y.tab\.c/"{o}"y.tab.c/
# OBSOLETE 
# OBSOLETE /init/s/"{s}"init\.c-tmp/"{o}"init.c-tmp/g
# OBSOLETE /init/s/^init\.c-tmp/"{o}"init.c-tmp/
# OBSOLETE /init/s/"{s}"init\.c/"{o}"init.c/g
# OBSOLETE /init/s/^init\.c/"{o}"init.c/
# OBSOLETE 
# OBSOLETE # Fix up the generation of version.c.
# OBSOLETE /"{o}"version.c \\Option-f Makefile/,/^$/c\
# OBSOLETE "{o}"version.c \\Option-f Makefile\
# OBSOLETE 	echo -n 'char *version = "'	 >"{o}"version.c\
# OBSOLETE 	echo -n "{VERSION}"		>>"{o}"version.c\
# OBSOLETE 	echo '";'			>>"{o}"version.c\
# OBSOLETE 	echo -n 'char *host_name = "'	>>"{o}"version.c\
# OBSOLETE 	echo -n "{host_alias}"		>>"{o}"version.c\
# OBSOLETE 	echo '";'			>>"{o}"version.c\
# OBSOLETE 	echo -n 'char *target_name = "'	>>"{o}"version.c\
# OBSOLETE 	echo -n "{target_alias}"	>>"{o}"version.c\
# OBSOLETE 	echo '";'			>>"{o}"version.c\
# OBSOLETE 
# OBSOLETE 
# OBSOLETE /ansidecl/s/include "{s}""ansidecl.h"/include "ansidecl.h"/
# OBSOLETE 
# OBSOLETE # Open-brace in a command causes much confusion; replace with the
# OBSOLETE # result from a script.
# OBSOLETE /initialize_all_files ()/c\
# OBSOLETE 	Echo -n 'void initialize_all_files () ' >> "{o}"init.c-tmp\
# OBSOLETE 	open-brace >> "{o}"init.c-tmp
# OBSOLETE 
# OBSOLETE # Replace the whole sed bit for init.c; it's simpler that way...
# OBSOLETE /echo {OBS} {TSOBS}/,/echo '}'/c\
# OBSOLETE 	For i in {OBS} {TSOBS}\
# OBSOLETE           Set filename "`Echo {i} | sed \\Option-d\
# OBSOLETE             -e '/^Onindy.c.o/d' \\Option-d\
# OBSOLETE             -e '/^nindy.c.o/d' \\Option-d\
# OBSOLETE             -e '/ttyflush.c.o/d' \\Option-d\
# OBSOLETE             -e '/xdr_ld.c.o/d' \\Option-d\
# OBSOLETE             -e '/xdr_ptrace.c.o/d' \\Option-d\
# OBSOLETE             -e '/xdr_rdb.c.o/d' \\Option-d\
# OBSOLETE             -e '/udr.c.o/d' \\Option-d\
# OBSOLETE             -e '/udip2soc.c.o/d' \\Option-d\
# OBSOLETE             -e '/udi2go32.c.o/d' \\Option-d\
# OBSOLETE             -e '/version.c.o/d' \\Option-d\
# OBSOLETE             -e '/[a-z0-9A-Z_]*-exp.tab.c.o/d' \\Option-d\
# OBSOLETE             -e 's/\\.c\\.o/.c/' \\Option-d\
# OBSOLETE             -e 's/^://'`"\
# OBSOLETE           If "{filename}" != ""\
# OBSOLETE             sed <"{s}""{filename}" >>"{o}"init.c-tmp -n \\Option-d\
# OBSOLETE             -e '/^_initialize_[a-z_0-9A-Z]* *(/s/^\\([a-z_0-9A-Z]*\\).*/  {extern void \\1 (); \\1 ();}/p'\
# OBSOLETE           End If\
# OBSOLETE 	End For\
# OBSOLETE 	Echo '}' >>"{o}"init.c-tmp
# OBSOLETE 
# OBSOLETE # Fix the main compile/link command.
# OBSOLETE /{CC_LD} {INTERNAL_LDFLAGS} -o gdb/,/"{o}"init.c.o {OBS} {TSOBS} {ADD_FILES} {CLIBS} {LOADLIBES}/c\
# OBSOLETE 	{CC_LD} {INTERNAL_LDFLAGS} -o gdb{PROG_EXT} "{o}"init.c.o {OBS} {TSOBS} {ADD_FILES} {CLIBS} {LOADLIBES} {EXTRALIBS}\
# OBSOLETE 	{MAKEPEF} gdb{PROG_EXT} -o gdb {MAKEPEF_TOOL_FLAGS} {MAKEPEF_FLAGS}\
# OBSOLETE 	{REZ} "{s}"mac-gdb.r -o gdb -append -d PROG_NAME='"'gdb'"' -d VERSION_STRING='"'{version}'"'\
# OBSOLETE 
# OBSOLETE # Replace the install actions with MPW-friendly script.
# OBSOLETE /^install \\Option-f /,/^$/c\
# OBSOLETE install \\Option-f all install-only\
# OBSOLETE \
# OBSOLETE install-only \\Option-f \
# OBSOLETE 	NewFolderRecursive "{bindir}"\
# OBSOLETE 	Duplicate -y gdb "{bindir}"gdb\
# OBSOLETE 	If "`Exists SiowGDB`" != ""\
# OBSOLETE 		Duplicate -y SiowGDB "{bindir}"SiowGDB\
# OBSOLETE 	End If\
# OBSOLETE 
# OBSOLETE 
# OBSOLETE # Don't do any recursive subdir stuff.
# OBSOLETE / subdir_do/s/{MAKE}/null-command/
# OBSOLETE 
# OBSOLETE # Edit out actions that only confuse MPW Make.
# OBSOLETE /^config.status \\Option-f/,/^$/d
# OBSOLETE /^Makefile \\Option-f/,/^$/d
# OBSOLETE 
# OBSOLETE # Don't test config.h dependencies.
# OBSOLETE /^"{o}"config.h \\Option-f/s/^/#/
# OBSOLETE 
# OBSOLETE # Add an action to build SIOWgdb.
# OBSOLETE $a\
# OBSOLETE SIOWgdb \\Option-f {OBS} {TSOBS} {ADD_DEPS} {CDEPS} "{o}"init.c.o\
# OBSOLETE 	Delete -i -y SIOWgdb\
# OBSOLETE 	{CC_LD} {INTERNAL_LDFLAGS} -t 'APPL' -c 'gdb ' -o SIOWgdb{PROG_EXT} "{o}"init.c.o {OBS} {TSOBS} {ADD_FILES} {CLIBS} {SIOW_LIB} {LOADLIBES} {EXTRALIBS}\
# OBSOLETE 	{MAKEPEF} SIOWgdb{PROG_EXT} -o SIOWgdb -ft 'APPL' -fc 'gdb ' {MAKEPEF_FLAGS} \
# OBSOLETE 	{REZ} -o SIOWgdb "{RIncludes}"siow.r -append -d __kPrefSize=5000 -d __kMinSize=2000 -d APPNAME='"'SIOWgdb'"' \
# OBSOLETE 	{REZ} "{s}"mac-gdb.r -o SIOWgdb -append -d VERSION_STRING='"'{version}'"'\

