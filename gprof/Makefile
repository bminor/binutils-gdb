# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = 
host_alias = sparc-sun-solaris
host_cpu = sparc
host_vendor = sun
host_os = solaris
target_alias = sparc-sun-solaris
target_cpu = sparc
target_vendor = sun
target_os = solaris
target_makefile_frag = ./config/mt-sparc
ALL=all.internal
#	@(#)Makefile	5.17 (Berkeley) 5/11/90

#### host and target dependent Makefile fragments come in here.
MACHINE=sparc
###

PROG=	gprof
SRCS=	gprof.c arcs.c dfn.c lookup.c ${MACHINE}.c hertz.c \
	printgprof.c printlist.c
LIBS = ../bfd/libbfd.a ../libiberty/libiberty.a

OBJS=	gprof.o arcs.o dfn.o lookup.o ${MACHINE}.o hertz.o \
	printgprof.o printlist.o

CFLAGS=	-I. -I../include -DMACHINE_H=\"${MACHINE}.h\" ${TCFLAGS} ${HCFLAGS}
.c.o:
	$(CC) -c $(CFLAGS) $<

all:	${PROG}

.PHONY: check
check:

beforeinstall:
	install -c -o ${BINOWN} -g ${BINGRP} -m ${BINMODE} \
	    ${.CURDIR}/gprof.flat ${.CURDIR}/gprof.callg \
	    ${DESTDIR}/usr/share/misc

$(PROG):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS)

clean:
	-rm -f $(OBJS) core gprof nohup.out
