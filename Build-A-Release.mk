### Makefile for preparing a progressive release.
###

### This makefile is distinct from test-build for several reasons.
### First, this version builds a "stage4" using src directories that
### have no CVS directories.  Second, the binaries are build from
### independent copies of the source.  That is, each host release will
### have it's own copy of the src directory.

###
### This file was written and is maintained by K. Richard Pixley,
### <rich@cygnus.com>.
###

CVS		:= /usr/latest/bin/cvs # using cvs-1.3
tree		:= progressive
rooting		:= /usr/cygnus
cvs_module	:= progressive1
gccflags	:= "-O"
release_tag	:= progressive-920529
bundle		:= emacs
host		:= sun4

ifeq (${bundle},emacs)
fixincludes :=
doc := emacs
else
fixincludes := ${host_release}/lib/fixincludes
doc := install
endif

host_release=${host}/${release_tag}
objdir=${host_release}/objdir

all: .${host}-done fluff

testfile: all blockit ${host}+.Z
${host}+.Z:
	(cd ${host} ; tar cf - ${release_tag} \
		| compress -vV \
		| ../blockit \
		> ../$@)

.${host}-done: .${host}-install .${host}-install-info
#	rm -rf ${objdir}
#	touch $@

.${host}-install: .${host}-all
	${MAKE} -C ${objdir} -w install-no-fixedincludes \
	prefix=`pwd`/${host_release} \
	exec_prefix=`pwd`/${host_release}/H-${host} \
	"CC=/usr/cygnus/${tree}.${tree}/H-${host}/bin/gcc ${gccflags}"
	touch $@

.${host}-install-info: .${host}-info
	${MAKE} -C ${objdir} -w install-info \
		prefix=`pwd`/${host_release} \
		exec_prefix=`pwd`/${host_release}/H-${host} \
		"CC=/usr/cygnus/${tree}.${tree}/H-${host}/bin/gcc \
		${gccflags}"
	touch $@

.${host}-all:	.${host}-config
	${MAKE} -C ${objdir} -w all "CC=/usr/cygnus/${tree}.${tree}/H-${host}/bin/gcc ${gccflags}"
	touch $@

.${host}-info:	.${host}-config
	${MAKE} -C ${objdir} -w info "CC=/usr/cygnus/${tree}.${tree}/H-${host}/bin/gcc ${gccflags}"
	touch $@

.${host}-config:	.${host}-src
	cd ${objdir} ; \
		../src/configure ${host} \
			-prefix=${rooting}/${release_tag} \
			-exec_prefix=${rooting}/${release_tag}/H-${host}
	touch $@

.${host}-src:	 .${release_tag}-co .${host}-dirs
	rm -rf ${host_release}/src
	(tar cf - ${release_tag} | (cd ${host_release} ; tar xvf -) ; true)
	(cd ${host_release}/${release_tag} ; find . \( -name CVS -o -name CVS.adm \) -depth -exec rm -rf {} \; )
	mv ${host_release}/${release_tag} ${host_release}/src
	touch $@

.${host}-dirs:
	[ -d ${host} ] || mkdir ${host}
	[ -d ${host_release} ] || mkdir ${host_release}
	[ -d ${objdir} ] || mkdir ${objdir}
	touch $@

.${release_tag}-co: .${release_tag}-tagged
	${CVS} co -r ${release_tag} ${cvs_module}
	mv ${tree} ${release_tag}
	touch $@

.${release_tag}-tagged:
	${CVS} rtag ${release_tag} ${cvs_module}
	touch $@

fluff:	${host_release}/src/Build-A-Release.mk \
	${host_release}/src/Pack-A-Progressive \
	${fixincludes} \
	${host_release}/src/README \
	${fixincludes} \
	${host}/Install \
	${host}/install.dvi \
	${host}/tape-labels.dvi \
	${host_release}/CYGNUS

${host}/tape-labels.dvi: ${host_release}/src/tape-labels.tex
	cp $< ${host}/`basename $<`
	(cd ${host} ; tex `basename $<`)
	rm ${host}/`basename $<`

${host}/install.dvi: ${host_release}/src/install.texinfo
	cp $< ${host}/`basename $<`
	(cd ${host} ; tex `basename $<`)
	(cd ${host} ; tex `basename $<`)
	(cd ${host} ; tex `basename $<`)
	rm ${host}/`basename $<`

print-notes: ${host}/install.dvi
	lpr $^

print-labels: ${host}/tape-labels.dvi
	lpr $^

clean-fluff:
	rm -f ${host_release}/src/Build-A-Release.mk \
		${host_release}/src/Pack-A-Progressive \
		${host_release}/lib/fixincludes \
		${host_release}/Install \
		${host_release}/src/${doc}-texi.in \
		${host_release}/src/tape-labels-tex.in \
		${host_release}/src/tape-labels.tex \
		${host_release}/src/install.texinfo \
		${host_release}/src/Install.in \
		
${host_release}/src/Build-A-Release.mk: ${release_tag}/Build-A-Release.mk
	cp $< $@

${host_release}/src/Pack-A-Progressive: ${release_tag}/Pack-A-Progressive
	cp $< $@

${host_release}/src/Install.in: ${release_tag}/Install.in
	cp $< $@

${host_release}/src/CYGNUS: ${release_tag}/CYGNUS
	cp $< $@

${host_release}/CYGNUS: ${host_release}/src/CYGNUS
	cp $< $@

${host_release}/src/${doc}-texi.in: ${release_tag}/${doc}-texi.in
	cp $< $@

${host_release}/src/tape-labels-tex.in: ${release_tag}/tape-labels-tex.in
	cp $< $@

${host_release}/lib/fixincludes:	${host_release}/src/gcc/fixincludes
	cp $< $@
	chmod a+x $@

${host}/Install: ${host_release}/Install
	cp $< $@
	chmod a+x $@

${host_release}/Install: ${host_release}/src/Install
	cp $< $@
	chmod a+x $@

${host_release}/src/install.texinfo: ${host_release}/src/${doc}-texi.in
	$(MAKE) process -f Build-A-Release.mk IN=$< OUT=$@ relno=`echo ${release_tag} | sed "/${tree}-/s///"`

${host_release}/src/tape-labels.tex: ${host_release}/src/tape-labels-tex.in
	$(MAKE) process -f Build-A-Release.mk IN=$< OUT=$@ relno=`echo ${release_tag} | sed "/${tree}-/s///"`

${host_release}/src/README: ${host_release}/src/install.texinfo
	-makeinfo +no-headers +no-split $< -o $@
### remove tags table
	sed -e '/^Tag Table/,$$d' $@ | sed -e '$$d' > $@.new
	mv -f $@.new $@

${host_release}/src/Install: ${host_release}/src/Install.in
	$(MAKE) process -f Build-A-Release.mk IN=$< OUT=$@ relno=`echo ${release_tag} | sed "/${tree}-/s///"`
	chmod a+x $@

process: process-${host}
	@if grep '||' ${OUT}.new ; then \
		echo $0 missed at least one of the triggers. 1>&2 ; \
		exit 1 ; \
	else \
		true ; \
	fi
	mv ${OUT}.new ${OUT}

process-sun4:
	sed -e "/^@clear HOSTsun4/s::@set HOSTsun4:" \
		-e "/^@set HOSTsun3/s::@clear HOSTsun3:" \
		-e "/^@set HOSTdecstation/s::@clear HOSTdecstation:" \
		-e "/^@set HOSTiris/s::@clear HOSTiris:" \
		-e "/^@set HOSTrs6000/s::@clear HOSTrs6000:" \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/\|\|RELNO\|\|/s::${relno}:g" \
		-e "/\|\|TAPdflt\|\|/s::QIC-24:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/nrst8:g" \
		-e "/\|\|BD\|\|/s::35:g" \
		-e "/\|\|SD\|\|/s::34:g" \
		-e "/\|\|DF\|\|/s::69:g" \
		-e "/\|\|HOST\|\|/s::Sun-4:g" \
		-e "/\|\|HOSTstr\|\|/s::sun4:g" \
		-e "/\|\|TARGET\|\|/s::sun4:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g" \
		-e "/\|\|BUNDLE\|\|/s::${bundle}:g" \
		< ${IN} > ${OUT}.new

process-sun3:
	sed -e "/^@set HOSTsun4/s::@clear HOSTsun4:" \
		-e "/^@clear HOSTsun3/s::@set HOSTsun3:" \
		-e "/^@set HOSTdecstation/s::@clear HOSTdecstation:" \
		-e "/^@set HOSTiris/s::@clear HOSTiris:" \
		-e "/^@set HOSTrs6000/s::@clear HOSTrs6000:" \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/\|\|RELNO\|\|/s::${relno}:g" \
		-e "/\|\|TAPdflt\|\|/s::QIC-24:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/nrst8:g" \
		-e "/\|\|BD\|\|/s::32:g" \
		-e "/\|\|SD\|\|/s::34:g" \
		-e "/\|\|DF\|\|/s::65:g" \
		-e "/\|\|HOST\|\|/s::Sun-3:g" \
		-e "/\|\|HOSTstr\|\|/s::sun3:g" \
		-e "/\|\|TARGET\|\|/s::sun3:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g" \
		-e "/\|\|BUNDLE\|\|/s::${bundle}:g" \
		< ${IN} > ${OUT}.new

process-decstation:
	sed -e "/^@set HOSTsun4/s::@clear HOSTsun4:" \
		-e "/^@set HOSTsun3/s::@clear HOSTsun3:" \
		-e "/^@clear HOSTdecstation/s::@set HOSTdecstation:" \
		-e "/^@set HOSTiris/s::@clear HOSTiris:" \
		-e "/^@set HOSTrs6000/s::@clear HOSTrs6000:" \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/\|\|RELNO\|\|/s::${relno}:g" \
		-e "/\|\|TAPdflt\|\|/s::QIC-24:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/nrmt0:g" \
		-e "/\|\|BD\|\|/s::27:g" \
		-e "/\|\|SD\|\|/s::34:g" \
		-e "/\|\|DF\|\|/s::61:g" \
		-e "/\|\|HOST\|\|/s::Decstation:g" \
		-e "/\|\|HOSTstr\|\|/s::decstation:g" \
		-e "/\|\|TARGET\|\|/s::decstation:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g" \
		-e "/\|\|BUNDLE\|\|/s::${bundle}:g" \
		< ${IN} > ${OUT}.new
	true

process-iris4:
	sed -e "/^@set HOSTsun4/s::@clear HOSTsun4:" \
		-e "/^@set HOSTsun3/s::@clear HOSTsun3:" \
		-e "/^@set HOSTdecstation/s::@clear HOSTdecstation:" \
		-e "/^@clear HOSTiris/s::@set HOSTiris:" \
		-e "/^@set HOSTrs6000/s::@clear HOSTrs6000:" \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/\|\|RELNO\|\|/s::${relno}:g" \
		-e "/\|\|TAPdflt\|\|/s::QIC-24:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/mt/tps0d0nrns:g" \
		-e "/\|\|BD\|\|/s::25:g" \
		-e "/\|\|SD\|\|/s::34:g" \
		-e "/\|\|DF\|\|/s::59:g" \
		-e "/\|\|HOST\|\|/s::Iris:g" \
		-e "/\|\|HOSTstr\|\|/s::iris4:g" \
		-e "/\|\|TARGET\|\|/s::iris4:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g" \
		-e "/\|\|BUNDLE\|\|/s::${bundle}:g" \
		< ${IN} > ${OUT}.new

process-rs6000:
	sed -e "/^@set HOSTsun4/s::@clear HOSTsun4:" \
		-e "/^@set HOSTsun3/s::@clear HOSTsun3:" \
		-e "/^@set HOSTdecstation/s::@clear HOSTdecstation:" \
		-e "/^@set HOSTiris/s::@clear HOSTiris:" \
		-e "/^@clear HOSTrs6000/s::@set HOSTrs6000:" \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/\|\|RELNO\|\|/s::${relno}:g" \
		-e "/\|\|TAPdflt\|\|/s::QIC-24:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/rmt0.1:g" \
		-e "/\|\|BD\|\|/s::36:g" \
		-e "/\|\|SD\|\|/s::34:g" \
		-e "/\|\|DF\|\|/s::70:g" \
		-e "/\|\|HOST\|\|/s::RS/6000:g" \
		-e "/\|\|HOSTstr\|\|/s::rs6000:g" \
		-e "/\|\|TARGET\|\|/s::rs6000:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g" \
		-e "/\|\|BUNDLE\|\|/s::${bundle}:g" \
		< ${IN} > ${OUT}.new

clean:
	rm -rf ${host}

blockit: blockit.c

### EOF
