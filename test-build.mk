###
### Makefile used to three-stage build a tree of source code.  Also used to
### compile other bundles, first with cc, then with gcc.
###

### This file was written and is maintained by K. Richard Pixley,
### <rich@cygnus.com>.

### Every invocation of this Makefile needs to have a variable set (host), 
### which is the named used for ./configure, and also the prefix for the
### various files and directories used in a three stage.

ifndef host
error:; @echo You must set the variable \"host\" to use this Makefile ; exit 1
else

### from here to very near the end of the file is the real guts of this 
### Makefile, and it is not seen if the variable 'host' is not set 

###
### START EDITTING HERE!!!
### These things will need to be set differently for each release.
###

### from which cvs tree are we working?
TREE := progressive

### binaries should be installed into?
ROOTING := /usr/cygnus

### When working from a tagged set of source, this should be the tag.  If not,
### then set the macro to be empty.
CVS_TAG := -r emacs-920529

### The name of the cvs module for this release.  The intersection of
### CVS_MODULE and CVS_TAG defines the source files in this release.
CVS_MODULE := progressive-emacs

### Historically, this was identical to CVS_TAG.  This is changing.
RELEASE_TAG := progressive-920529

### Historically, binaries were installed here.  This is changing.
release_root := $(ROOTING)/$(RELEASE_TAG)

### STOP EDITTING HERE!!!
### With luck, eventually, nothing else will need to be editted.

TIME := time
GCC := gcc -O
GNU_MAKE := /usr/latest/bin/make
override MAKE := make
override MAKEFLAGS :=
override MFLAGS :=

SHELL := /bin/sh

.NOEXPORT:

FLAGS_TO_PASS := \
	"GCC=$(GCC)" \
	"TIME=$(TIME)" \
	"MF=$(MF)"

### general config stuff
WORKING_DIR := $(host)-objdir
STAGE1DIR := $(WORKING_DIR).1
STAGE2DIR := $(WORKING_DIR).2
STAGE3DIR := $(WORKING_DIR).3
HOLESDIR := $(host)-holes

.PHONY: all
all:	$(TREE) do1 do2 do3 comparison

.PHONY: do1
do1:	$(host)-stamp-holes $(host)-stamp-stage1

$(host)-stamp-stage1:
	if [ -d $(STAGE1DIR) ] ; then \
		mv $(STAGE1DIR) $(WORKING_DIR) ; \
	else \
		true ; \
	fi
	PATH=`pwd`/$(HOLESDIR) ; \
		export PATH ; \
		SHELL=sh ; export SHELL ; \
		$(TIME) $(GNU_MAKE) $(host)-stamp-stage1-installed host=$(host) $(FLAGS_TO_PASS)
	touch $(host)-stamp-stage1
	mv $(WORKING_DIR) $(STAGE1DIR)

$(host)-stamp-stage1-installed: $(host)-stamp-stage1-checked
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) $(MF) install host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) $(MF) install-info host=$(host))
	touch $@

$(host)-stamp-stage1-checked: $(host)-stamp-stage1-built
#	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) $(MF) check host=$(host))
	touch $@

$(host)-stamp-stage1-built: $(host)-stamp-stage1-configured
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) $(MF) all host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) $(MF) info host=$(host))
	touch $@

$(host)-stamp-stage1-configured:
	[ -d $(WORKING_DIR) ] || mkdir $(WORKING_DIR)
	(cd $(WORKING_DIR) ; \
		$(TIME) ../$(TREE)/configure $(host) -v -srcdir=../$(TREE) \
			-prefix=$(release_root) \
			-exec_prefix=$(release_root)/H-$(host))
	touch $@

.PHONY: do2
do2:	$(HOLESDIR) $(host)-stamp-stage2

$(host)-stamp-stage2:
	if [ -d $(STAGE2DIR) ] ; then \
		mv $(STAGE2DIR) $(WORKING_DIR) ; \
	else \
		true ; \
	fi
	PATH=$(release_root)/H-$(host)/bin:`pwd`/$(HOLESDIR) ; \
		export PATH ; \
		SHELL=sh ; export SHELL ; \
		$(TIME) $(MAKE) -w $(STAGE2DIR) host=$(host) $(FLAGS_TO_PASS)
	touch $(host)-stamp-stage2

$(STAGE2DIR): $(host)-stamp-stage2-installed
	mv $(WORKING_DIR) $(STAGE2DIR)

$(host)-stamp-stage2-installed: $(host)-stamp-stage2-checked
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" install host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" install-info host=$(host))
	touch $@

$(host)-stamp-stage2-checked: $(host)-stamp-stage2-built
#	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" check host=$(host))
	touch $@

$(host)-stamp-stage2-built: $(host)-stamp-stage2-configured
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" all host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" info host=$(host))
	touch $@

$(host)-stamp-stage2-configured:
	[ -d $(WORKING_DIR) ] || mkdir $(WORKING_DIR)
	(cd $(WORKING_DIR) ; \
		$(TIME) ../$(TREE)/configure $(host) -v -srcdir=../$(TREE) \
			-prefix=$(release_root) \
			-exec_prefix=$(release_root)/H-$(host))
	touch $@

.PHONY: do3
do3:	$(HOLESDIR) $(host)-stamp-stage3

$(host)-stamp-stage3:
	if [ -d $(STAGE3DIR) ] ; then \
		mv $(STAGE3DIR) $(WORKING_DIR) ; \
	else \
		true ; \
	fi
	PATH=$(release_root)/H-$(host)/bin:`pwd`/$(HOLESDIR) ; \
		export PATH ; \
		SHELL=sh ; export SHELL ; \
		$(TIME) $(MAKE) -w $(STAGE3DIR) host=$(host) $(FLAGS_TO_PASS)
	touch $(host)-stamp-stage3

$(STAGE3DIR): $(host)-stamp-stage3-checked
	mv $(WORKING_DIR) $(STAGE3DIR)

$(host)-stamp-stage3-installed: $(host)-stamp-stage3-checked
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" install host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" install-info host=$(host))
	touch $@

$(host)-stamp-stage3-checked: $(host)-stamp-stage3-built
#	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" check host=$(host))
	touch $@

$(host)-stamp-stage3-built: $(host)-stamp-stage3-configured
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" all host=$(host))
	(cd $(WORKING_DIR) ; $(TIME) $(MAKE) -w $(MF) "CC=$(GCC)" info host=$(host))
	touch $@

$(host)-stamp-stage3-configured:
	[ -d $(WORKING_DIR) ] || mkdir $(WORKING_DIR)
	(cd $(WORKING_DIR) ; \
		$(TIME) ../$(TREE)/configure $(host) -v -srcdir=../$(TREE) \
			-prefix=$(release_root) \
			-exec_prefix=$(release_root)/H-$(host))
	touch $@

# These things are needed by a three-stage, but are not included locally.

HOLES := \
	[ \
	ar \
	as \
	awk \
	basename \
	cat \
	cc \
	chmod \
	cmp \
	cp \
	date \
	diff \
	echo \
	egrep \
	ex \
	expr \
	find \
	grep \
	hostname \
	install \
	ld \
	lex \
	ln \
	ls \
	make \
	mkdir \
	mv \
	nm \
	pwd \
	ranlib \
	rm \
	rmdir \
	sed \
	sh \
	sort \
	test \
	time \
	touch \
	tr \
	true \
	whoami

### so far only sun make supports VPATH
ifeq ($(subst sun3,sun4,$(host)),sun4)
MAKE_HOLE :=
else
MAKE_HOLE := make
endif

### rs6000 as is busted.  We cache a patched version in unsupported.
ifeq ($(subst sun3,rs6000,$(host)),rs6000)
AS_HOLE := as
else
AS_HOLE :=
endif

### These things are also needed by a three-stage, but in this case, the GNU version of the tool is required.
PARTIAL_HOLES := \
	$(AS_HOLE) \
	$(MAKE_HOLE) \
	flex \
	m4

### look in these directories for things missing from a three-stage
HOLE_DIRS := \
	/bin \
	/usr/bin \
	/usr/ucb \
	/usr/unsupported/bin

### look in these directories for alternate versions of some tools.
PARTIAL_HOLE_DIRS := \
	/usr/latest/bin \
	/usr/progressive/bin \
	/usr/vintage/bin \
	/usr/unsupported/bin

$(HOLESDIR): $(host)-stamp-holes

$(host)-stamp-holes:
	-rm -rf $(HOLESDIR)
	-mkdir $(HOLESDIR)
	for i in $(HOLES) ; do \
		found= ; \
		for j in $(HOLE_DIRS) ; do \
			if [ -x $$j/$$i ] ; then \
				cp $$j/$$i $(HOLESDIR) ; \
				echo $$i from $$j ; \
				found=t ; \
				break ; \
			fi ; \
		done ; \
		case "$$found" in \
		t) ;; \
		*) echo $$i is NOT found ;; \
		esac ; \
	done
	for i in $(PARTIAL_HOLES) ; do \
		found= ; \
		for j in $(PARTIAL_HOLE_DIRS) ; do \
			if [ -x $$j/$$i ] ; then \
				rm -f $(HOLESDIR)/$$i ; \
				cp $$j/$$i $(HOLESDIR)/$$i || exit 1; \
				echo $$i from $$j ; \
				found=t ; \
				break ; \
			fi ; \
		done ; \
		case "$$found" in \
		t) ;; \
		*) echo $$i is NOT found ;; \
		esac ; \
	done
	touch $(host)-stamp-holes

.PHONY: comparison
comparison:
ifeq ($(subst rs6000,iris4,$(subst decstation,iris4,$(host))),iris4)
	for i in `cd $(STAGE3DIR) ; find . -name \*.o -print` ; do \
		tail +10c $(STAGE2DIR)/$$i > foo1 ; \
		tail +10c $(STAGE3DIR)/$$i > foo2 ; \
		cmp foo1 foo2 || echo $$i ; \
	done
else
	for i in `cd $(STAGE3DIR) ; find . -name \*.o -print` ; do \
		cmp $(STAGE2DIR)/$$i $(STAGE3DIR)/$$i ; \
	done
endif

.PHONY: clean
clean:
	rm -rf $(HOLESDIR) $(WORKING_DIR)* $(host)-stamp-* *~

.PHONY: very
very:
	rm -rf $(TREE)

$(TREE):
	$(TIME) cvs co $(CVS_TAG) $(CVS_MODULE)

force:

endif # host

### Local Variables:
### fill-column: 131
### End:
