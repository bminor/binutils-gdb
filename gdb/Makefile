# Makefile for GDB
#   Copyright (C) 1986, 1988 Free Software Foundation, Inc.
#
#GDB is distributed in the hope that it will be useful, but WITHOUT ANY
#WARRANTY.  No author or distributor accepts responsibility to anyone
#for the consequences of using it or for whether it serves any
#particular purpose or works at all, unless he says so in writing.
#Refer to the GDB General Public License for full details.
#
#Everyone is granted permission to copy, modify and redistribute GDB,
#but only under the conditions described in the GDB General Public
#License.  A copy of this license is supposed to have been given to you
#along with GDB so you can know your rights and responsibilities.  It
#should be in a file named COPYING.  Among other things, the copyright
#notice and this notice must be preserved on all copies.
#
#In other words, go ahead and share GDB, but don't try to stop
#anyone else from sharing it farther.  Help stamp out software hoarding!


# On HPUX, you need to add -Ihpux to CFLAGS.
# The headers in the subdir hpux override system headers
# and tell GDB to use BSD executable file format.
# You also need to add -lGNU to CLIBS, and perhaps CC = gcc.

# -I. for "#include <obstack.h>"
CFLAGS = -g -I.
# NOTE!!! -O  may FAIL TO WORK!  See initialize.h for some weird hacks.

# define this to be "obstack.o" if you don't have the obstack library installed
# you must at the same time define OBSTACK1 as "obstack.o" 
# so that the dependencies work right.
OBSTACK = obstack.o
OBSTACK1 = obstack.o

CLIBS = $(OBSTACK)

STARTOBS = main.o firstfile.o

OBS = blockframe.o breakpoint.o findvar.o stack.o source.o \
    values.o eval.o valops.o valarith.o valprint.o printcmd.o \
    symtab.o symmisc.o coffread.o dbxread.o infcmd.o infrun.o remote.o

TSOBS = core.o inflow.o

NTSOBS = standalone.o

ENDOBS = lastfile.o command.o utils.o expread.o expprint.o pinsn.o \
	environ.o version.o

TSSTART = /lib/crt0.o

NTSSTART = kdb-start.o

gdb : $(STARTOBS) $(OBS) $(TSOBS) $(ENDOBS) $(OBSTACK1)
	$(CC) $(LDFLAGS) -o gdb $(STARTOBS) $(OBS) $(TSOBS) $(ENDOBS) $(CLIBS)

xgdb : $(STARTOBS) $(OBS) $(TSOBS) xgdb.o $(ENDOBS) $(OBSTACK1)
	$(CC) $(LDFLAGS) -o xgdb $(STARTOBS) $(OBS) $(TSOBS) xgdb.o $(ENDOBS) \
           -lXaw -lXt -lX11 $(CLIBS)

kdb : $(NTSSTART) $(STARTOBS) $(OBS) $(NTSOBS) $(ENDOBS) $(OBSTACK1)
	ld -o kdb $(NTSSTART) $(STARTOBS) $(OBS) $(NTSOBS) $(ENDOBS) -lc $(CLIBS)

blockframe.o : blockframe.c defs.h initialize.h param.h symtab.h frame.h
breakpoint.o : breakpoint.c defs.h initialize.h param.h symtab.h frame.h
command.o : command.c command.h
coffread.o : coffread.c defs.h initialize.h param.h symtab.h
core.o : core.c defs.h initialize.h param.h
dbxread.o : dbxread.c defs.h initialize.h param.h symtab.h
environ.o : environ.c environ.h
expprint.o : expprint.c defs.h symtab.h expression.h
expread.tab.c : expread.y
	@echo 'Expect 96 shift/reduce conflicts.'
	yacc expread.y
	mv y.tab.c expread.tab.c
expread.o : expread.tab.c defs.h param.h symtab.h frame.h expression.h
	$(CC) -c ${CFLAGS} expread.tab.c
	mv expread.tab.o expread.o
eval.o : eval.c defs.h initialize.h symtab.h value.h expression.h
findvar.o : findvar.c defs.h initialize.h param.h symtab.h frame.h value.h
firstfile.o : firstfile.c initialize.h
infcmd.o : infcmd.c defs.h initialize.h param.h symtab.h frame.h inferior.h environ.h value.h
inflow.o : inflow.c defs.h initialize.h param.h frame.h inferior.h
infrun.o : infrun.c defs.h initialize.h param.h symtab.h frame.h inferior.h wait.h
kdb-start.o : kdb-start.c defs.h param.h
lastfile.o : lastfile.c
main.o : main.c defs.h command.h
# pinsn.o depends on ALL the opcode printers
# since we don't know which one is really being used.
pinsn.o : pinsn.c defs.h param.h symtab.h \
    vax-opcode.h vax-pinsn.c m68k-opcode.h m68k-pinsn.c
printcmd.o : printcmd.c defs.h initialize.h param.h symtab.h value.h expression.h
remote.o : remote.c defs.h initialize.h param.h frame.h inferior.h
source.o : source.c defs.h initialize.h symtab.h
stack.o : stack.c defs.h initialize.h param.h symtab.h frame.h
standalone.o : standalone.c defs.h initialize.h param.h symtab.h frame.h inferior.h wait.h
symmisc.o : symmisc.c defs.h initialize.h symtab.h
symtab.o : symtab.c defs.h initialize.h param.h symtab.h
utils.o : utils.c defs.h
valarith.o : valarith.c defs.h initialize.h param.h symtab.h value.h expression.h
valops.o : valops.c defs.h initialize.h param.h symtab.h value.h
valprint.o : valprint.c defs.h initialize.h param.h symtab.h value.h
values.o : values.c defs.h initialize.h param.h symtab.h value.h
version.o : version.c
xgdb.o : xgdb.c defs.h initialize.h param.h symtab.h frame.h
	$(CC) -c $(CFLAGS) xgdb.c -o $@

obstack.o : obstack.c
