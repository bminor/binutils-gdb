#  Build all of the targets for any given host.....
#
#  This file is going to be ugly.  It will be VERY specific to the
#  Cygnus environment and build-process.
#
# Useful targets (rt = recursion target):
# (please correct or expand on this)
# FIXME: Might some of these be superfluous?
#
# all-emacs
# all-cygnus	- set up install directories, build 3stage native and all
#		supported cross targets, then check 3stage'd native
#		(rt = $(canonhost)-stamp-3stage-done, do-cygnus for cross)
# native	- set up install directories, build 3stage native
#		(rt = $(canonhost)-stamp-3stage-1)
# build-cygnus	- build 3stage native and all supported cross targets
#		(rt = $(canonhost)-stamp-3stage-done, build-cygnus for cross)
# build-latest	- build native and all supported cross targets
#		(rt = build-latest)
# all-native	- set up install directories, build native
#		(rt = do-native)
# all-cross	- set up install directories, build all targets
#		(rt = do-cygnus)
# config	- configure native and all supported cross targets
#		(rt = do1-config, do-native-config for cross)
# build		- build native and all supported cross targets
#		(rt = do1-build, build-native for cross)
# 3build	- build 3stage native and all supported cross targets
#		(rt = all, build-cygnus for cross)
# build-all-latest - build 3stage native and all supported cross targets
#		(rt = $(canonhost)-stamp-3stage-done, build-latest for cross)
# clean		- remove objdir directories, stamp files
#		(rt = clean)
#
# To configure/build for fewer targets, specify TARGETS="native cross1 ...".
  
TREE	= devo
include $(TREE)/release-info

TEST_INSTALL_DISK = /galt

INSTALLDIR = $(TEST_INSTALL_DISK)/$(TREE)-test/$(RELEASE_TAG)

ifndef host
host := $(shell $(TREE)/config.guess)
endif

NATIVE  = native

GCC	 = gcc -O 
CFLAGS	 = -g
CXXFLAGS = -g -O
MAKEINFOFLAGS =

log	= 1>$(canonhost)-build-log 2>&1
clog	= 1>$(canonhost)-check-log 2>&1
cyglog    = 1> $(canonhost)-x-$$i-cygnus-build-log 2>&1
latestlog = 1> $(canonhost)-x-$$i-latest-build-log 2>&1
natlog    = 1> $(canonhost)-x-$$i-native-build-log 2>&1

canonhost := $(shell $(TREE)/config.sub $(host))

# Convert config.guess results to a simpler form.
ifeq ($(canonhost),sparc-sun-solaris2.1)
canonhost := sparc-sun-solaris2
endif
ifeq ($(canonhost),sparc-sun-solaris2.3)
canonhost := sparc-sun-solaris2
endif
ifeq ($(canonhost),mips-dec-ultrix4.2)
canonhost := mips-dec-ultrix
endif
ifeq ($(canonhost),mips-sgi-irix4.0.1)
canonhost := mips-sgi-irix4
endif
ifeq ($(canonhost),mips-sgi-irix4.0.5H)
canonhost := mips-sgi-irix4
endif
ifeq ($(canonhost),rs6000-ibm-aix3.2)
canonhost := rs6000-ibm-aix
endif
ifeq ($(canonhost),i486-unknown-sco3.2v4.0)
canonhost := i386-sco3.2v4
endif
ifeq ($(canonhost),i386-unknown-sco3.2v4.0)
canonhost := i386-sco3.2v4
endif
ifeq ($(canonhost),i386-unknown-sco3.2v4)
canonhost := i386-sco3.2v4
endif
ifeq ($(canonhost),i386-unknown-go32)
canonhost := i386-go32
endif
ifeq ($(canonhost),i386-unknown-sysv4.2)
canonhost := i386-sysv4.2
endif
ifeq ($(canonhost),i386-lynx-lynxos)
canonhost := i386-lynxos
endif
ifeq ($(canonhost),m68k-lynx-lynxos)
canonhost := m68k-lynxos
endif

ifeq ($(canonhost),sparc-sun-sunos4.1.3)
TARGETS = $(NATIVE) \
	i386-go32 	\
	a29k-amd-udi 	\
	h8300-hms 	\
	i386-aout	\
	i386-lynx 	\
	i386-netware 	\
	i960-vxworks 	i960-intel-nindy \
	mips-idt-ecoff	\
	m68k-aout	m68k-vxworks 	m68k-coff \
	m68k-lynx 	\
	sh-hms 		\
	sparc-aout	sparc-vxworks	\
	sparclite-aout  sparclite-vxworks \
	z8k-coff
GCC = gcc -O -pipe
all: all-cygnus
endif

ifeq ($(canonhost),m68k-sun-sunos4.1.1)
TARGETS = $(NATIVE) \
	m68k-coff 
GCC = gcc -O -msoft-float
CC = cc -J
all: all-cygnus
endif

ifeq ($(canonhost),sparc-sun-solaris2)
TARGETS = $(NATIVE) \
	a29k-amd-udi \
	i960-vxworks \
	m68k-aout	m68k-coff \
	m88k-coff	\
	mipsel-idt-ecoff \
	sparclite-aout
CC = cc -Xs
GCC = gcc -O -pipe
all: all-cygnus
endif

ifeq ($(canonhost),mips-dec-ultrix)
TARGETS = $(NATIVE) m68k-aout
CC = cc -Wf,-XNg1000
all: all-cygnus
endif

ifeq ($(canonhost),mips-sgi-irix4)
TARGETS	= $(NATIVE) \
	mips-idt-ecoff	sh-hms
CC = cc -cckr -Wf,-XNg1500 -Wf,-XNk1000 -Wf,-XNh2000
all: all-cygnus
endif

ifeq ($(canonhost),rs6000-ibm-aix)
TARGETS	= $(NATIVE) \
	i960-vxworks \
	m68k-aout
all: all-cygnus
endif

ifeq ($(canonhost),m68k-hp-hpux)
TARGETS	= $(NATIVE)	m68k-aout
TMPDIR := $(shell mkdir $(canonhost)-tmpdir; cd $(canonhost)-tmpdir ; pwd)
CC = cc -Wp,-P 
#CFLAGS = +O1000 
CFLAGS = 
all: all-cygnus
endif

ifeq ($(canonhost),hppa1.1-hp-hpux)
TARGETS = \
	$(NATIVE) \
	i960-vxworks \
	m68k-aout	m68k-vxworks
CC = cc 
#CFLAGS = +Obb2000
CFLAGS = -g
all: all-native
endif

ifeq ($(canonhost),i386-sco3.2v4)
TARGETS = $(NATIVE) i386-aout
all: all-cygnus
endif

ifeq ($(canonhost),i386-go32)
TARGETS = \
	a29k-amd-udi \
	h8300-hms 	\
	i386-aout 	\
	m68k-aout	m68k-coff \
	mips-idt-ecoff 	\
	sh-hms 		\
	sparclite-aout
CC = i386-go32-gcc
GCC = i386-go32-gcc -O
CFLAGS =
CXXFLAGS = -O
MAKEINFOFLAGS = --no-split
all: all-cross
endif

ifeq ($(canonhost),i386-sysv4.2)
TARGETS = $(NATIVE) i386-netware
CC = cc
all: all-cygnus
endif

ifeq ($(canonhost),i386-lynxos)
TARGETS = $(NATIVE)
CC = /bin/gcc
all: all-cygnus
SHELL=/bin/bash
endif

ifeq ($(canonhost),m68k-lynxos)
TARGETS = $(NATIVE)
CC = /bin/gcc
all: all-cygnus
SHELL=/bin/bash
endif


FLAGS_TO_PASS := \
	"GCC=$(GCC)" \
	"CC=$(CC)" \
	"CFLAGS=$(CFLAGS)" \
	"CXXFLAGS=$(CXXFLAGS)" \
	"host=$(canonhost)" \
	"MAKEINFOFLAGS=$(MAKEINFOFLAGS)" \
	"RELEASE_TAG=$(RELEASE_TAG)" \
	"SHELL=$(SHELL)"

# set GNU_MAKE and CONFIG_SHELL correctly in sub-builds
ifeq ($(patsubst %-lynxos,lynxos,$(canonhost)),lynxos)
FLAGS_TO_PASS := $(FLAGS_TO_PASS) "GNU_MAKE=$(MAKE)" "CONFIG_SHELL=/bin/bash"
endif

all-emacs:
	@echo build started at `date`
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	-rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG) 
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do1 > $(canonhost)-emacs-native-log 2>&1 
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do-latest > $(canonhost)-emacs-latest-log 2>&1 
	@echo done at `date`

all-cygnus:
	@echo build started at `date`
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	-rm -f $(ROOTING)/$(RELEASE_TAG) && ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG) 
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
            if [ ! -f $(canonhost)-stamp-3stage-done ] ; then \
	      echo "3staging $(canonhost) native" ; \
	      $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-stamp-3stage-done $(log) && \
	         echo "     completed successfully" ; \
	    else \
	      true ; \
	    fi ; \
	  else \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-cygnus $(cyglog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done
	@if [ ! -f $(canonhost)-stamp-3stage-checked ] ; then \
	  echo checking $(canonhost) native ; \
	  $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-check-3stage $(clog) ; \
	  touch $(canonhost)-stamp-3stage-checked ; \
	else \
	  true ; \
	fi
	@echo done at `date`

native:
	@echo build started at `date`
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG) 
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-stamp-3stage-done $(log)
	@echo done at `date`

build-cygnus:
	@echo build started at `date`
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
            if [ ! -f $(canonhost)-3stage-done ] ; then \
	      echo "3staging $(canonhost) native" ; \
	      $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-stamp-3stage-done $(log) && \
	         echo "     completed successfully" ; \
	    fi ; \
	  else \
	    echo "building $(canonhost) cross to $$i:" `date` ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-cygnus $(cyglog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done
	@echo done at `date`

build-latest:
	@echo build started at `date`
	@for i in $(TARGETS) ; do \
	  if [ "$$i" != "native" ] ; then \
	    echo "building $(canonhost) cross to $$i:" `date` ; \
	    $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-latest $(latestlog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done
	@echo done at `date`

all-native:
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG)
	@for i in $(TARGETS) ; do \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-native $(natlog) && \
	       echo "     completed successfully" ; \
	done

all-cross:
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG)
	@for i in $(TARGETS) ; do \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build=$(build) do-cygnus $(cyglog) && \
	       echo "     completed successfully" ; \
	done

config:
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
	    echo "config stage1 for $(canonhost)" ; \
	    $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do1-config $(log) && \
	       echo "     completed successfully" ; \
	  else \
	    echo "config $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-native-config $(cyglog) && \
	       echo "     completed successfully" ;  \
	  fi ; \
	done


build:
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
	    $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do1-build $(log) && \
	       echo "     completed successfully" ; \
	  else \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-native $(natlog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done


3build:
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
	    echo "building 3stage for $(canonhost)" ; \
	    $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) all $(log) && \
	       echo "     completed successfully" ; \
	  else \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-cygnus $(cyglog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done

build-all-latest:
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
	    echo "building 3stage for $(canonhost)" ; \
	    $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-stamp-3stage-done $(log) && \
	       echo "     completed successfully" ; \
	  else \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-latest $(cyglog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done

clean:
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) clean $(log)

### Local Variables:
### fill-column: 131
### End:
