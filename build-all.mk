#  Build all of the targets for any given host.....
#
#  This file is going to be ugly.  It will be VERY specific to the
#  Cygnus environment and build-process.
#
#

TREE	= devo
include $(TREE)/release-info

TEST_INSTALL_DISK = /cirdan/abc

INSTALLDIR = $(TEST_INSTALL_DISK)/$(TREE)-test/$(RELEASE_TAG)

ifndef host
host := $(shell $(TREE)/config.guess)
endif

NATIVE  = native

GCC     = gcc -O 
CFLAGS  = -g

log	= 1>$(canonhost)-build-log 2>&1
tlog    = 1> $(canonhost)-x-$$i-build-log 2>&1

canonhost := $(shell $(TREE)/config.sub $(host))

# Convert config.guess results to a simpler form.
ifeq ($(canonhost),mips-dec-ultrix4.2)
canonhost := mips-dec-ultrix
endif
ifeq ($(canonhost),mips-sgi-irix4.0.1)
canonhost := mips-sgi-irix4
endif
ifeq ($(canonhost),rs6000-ibm-aix3.2)
canonhost := rs6000-ibm-aix
endif
ifeq ($(canonhost),i486-unknown-sco3.2v4.0)
canonhost := i386-sco3.2v4
endif

ifeq ($(canonhost),sparc-sun-sunos4.1.1)
TARGETS = $(NATIVE)	i386-go32	m68k-aout	m68k-vxworks \
	i960-intel-nindy 		i386-aout	a29k-amd-udi \
	sparc-vxworks	m68k-coff 	i960-vxworks	sparc-aout \
	sparclite-aout	sparclitefrwcompat-aout 	h8300-hms \
	z8k-sim		mips-idt-ecoff
all: all-cygnus
endif

ifeq ($(canonhost),m68k-sun-sunos4.1.1)
TARGETS = $(NATIVE) m68k-aout m68k-vxworks m68k-coff
GCC = gcc -O -msoft-float
CC = cc -J
all: all-cygnus
endif

ifeq ($(canonhost),sparc-sun-solaris2)
TARGETS = $(NATIVE) m68k-vxworks m68k-aout sparc-aout mips-idt-ecoff
CC = cc -Xs
all: all-cygnus
endif

ifeq ($(canonhost),mips-dec-ultrix)
TARGETS = $(NATIVE) m68k-aout
# TARGETS	= $(NATIVE) m68k-vxworks m68k-aout i960-vxworks \
#	  sparc-vxworks m68k-coff i386-aout sparc-aout i960-intel-nindy
CC = cc -Wf,-XNg1000
all: all-cygnus
endif

ifeq ($(canonhost),mips-sgi-irix4)
TARGETS	= $(NATIVE) m68k-vxworks
CC = cc -cckr -Wf,-XNg1500 -Wf,-XNk1000 -Wf,-XNh1500
all: all-cygnus
endif

ifeq ($(canonhost),rs6000-ibm-aix)
TARGETS	= $(NATIVE) m68k-aout m68k-vxworks i960-vxworks
all: all-cygnus
endif

ifeq ($(canonhost),m68k-hp-hpux)
TARGETS	= $(NATIVE) m68k-vxworks
TMPDIR := $(shell mkdir $(canonhost)-tmpdir; cd $(canonhost)-tmpdir ; pwd)
CC = cc +O1000 -Wp,-P
CFLAGS = 
all: all-cygnus
endif

ifeq ($(canonhost),hppa1.1-hp-hpux)
TARGETS = i960-vxworks m68k-aout m68k-vxworks m68k-hp-hpux
#TARGETS	= m68k-aout m68k-coff m68k-vxworks i960-vxworks a29k-amd-udi
#TARGETS	= m68k-aout	i386-aout	a29k-amd-udi \
#	  i960-vxworks	m68k-coff	m68k-vxworks \
#	  sparc-aout			sparc-vxworks \
#	  sparclite-aout		sparclitefrwcompat-aout
CC = cc 
all: all-native
endif

ifeq ($(canonhost),i386-sco3.2v4)
TARGETS = $(NATIVE) i386-aout
all: all-cygnus
endif

FLAGS_TO_PASS := \
	"GCC=$(GCC)" \
	"CC=$(CC)" \
	"CFLAGS=$(CFLAGS)" \
	"host=$(canonhost)" \
	"RELEASE_TAG=$(RELEASE_TAG)"

all-emacs:
	@echo build started at `date`
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG) 
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do1 > $(canonhost)-native-log 2>&1 
	$(MAKE) -f test-build.mk $(FLAGS_TO_PASS) do-latest > $(canonhost)-latest-log 2>&1 
	@echo done at `date`

all-cygnus:
	@echo build started at `date`
	[ -d $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
	rm -f $(ROOTING)/$(RELEASE_TAG)
	ln -s $(INSTALLDIR) $(ROOTING)/$(RELEASE_TAG) 
	@for i in $(TARGETS) ; do \
	  if [ "$$i" = "native" ] ; then \
            if [ ! -f $(canonhost)-3stage-done ] ; then \
	      echo "3staging $(canonhost) native" ; \
	      $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) $(canonhost)-stamp-3stage-done $(log) && \
	         echo "     completed successfully" ; \
	    fi \
	  else \
	    echo "building $(canonhost) cross to $$i" ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-cygnus $(tlog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done
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
	    fi \
	  else \
	    echo "building $(canonhost) cross to $$i:" `date` ; \
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-cygnus $(tlog) && \
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
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-native $(tlog) && \
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
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i do-native-config $(tlog) && \
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
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-native $(tlog) && \
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
            $(MAKE) -f test-build.mk $(FLAGS_TO_PASS) target=$$i build-cygnus $(tlog) && \
	       echo "     completed successfully" ; \
	  fi ; \
	done

### Local Variables:
### fill-column: 131
### End:
