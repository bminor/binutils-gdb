### Makefile for preparing a progressive release.
###
### This makefile is distinct from the makefile for three-staging for
### several reasons.  First, this version builds a "stage4" using src
### directories that have no CVS directories.  Second, the binaries
### are build from independent copies of the source.  That is, each
### host release will have it's own copy of the src directory.
###
### This file was written and is maintained by K. Richard Pixley
###

RELNO		= 1.0

PWD 		= $(shell pwd)
CVS		= $(PWD)/cirdancvs
tree		= progressive
rooting		= /opt/cygnus-sol2-$(RELNO)
cvs_module	= progressive-solaris
gccflags	= "-O"
release_tag	= solaris-1-0
release_dir	= cygnus-sol2-1.0



host_release	= ${host}/cygnus-sol2-$(RELNO)
objdir		= ${host_release}/objdir


# prefixes are passed as arguments to configure, they have to start with -
prefixes 	= -prefix=$(rooting)

# inst_prefixes are passed as arguments to make, without the -
inst_prefixes 	= prefix=$(PWD)/$(host_release)

### What are these for?
create		=
restart		=
usage		=

## which C compiler to use
#CC		= $(rooting)/bin/gcc
CC		= /opt/cygnus-sol2-1.0/bin/gcc
C_COMPILER	= "CC=$(CC) ${gccflags}"

all: .${host}-done fluff
	chmod -R a-w $(host)
	chmod -R go=u $(host)
	find $(host) -type d -print | xargs chmod u+w

testfile: all blockit ${host}+.Z
${host}+.Z:
	(cd ${host} ; tar cf - ${release_dir} \
		| compress -vV \
		| ../blockit \
		> ../$@)

.${host}-done: .${host}-install .${host}-install-info
	rm -rf ${objdir}
	touch $@

.${host}-install: .${host}-all
	${MAKE} -C ${objdir} -w install-no-fixedincludes \
		$(inst_prefixes) $(C_COMPILER)
	touch $@

.${host}-install-info: .${host}-info
	${MAKE} -C ${objdir} -w install-info $(inst_prefixes) $(C_COMPILER)
	touch $@

.${host}-all:	.${host}-config
	${MAKE} -C ${objdir} -w all $(C_COMPILER)
	touch $@

.${host}-info:	.${host}-config
	${MAKE} -C ${objdir} -w info $(C_COMPILER)
	touch $@

.${host}-config:	.${host}-src
	cd ${objdir} ; \
		../src/configure ${host} $(prefixes) 
	touch $@

.${host}-src:	 .${release_tag}-co .${host}-dirs
	rm -rf ${host_release}/src
	(tar cf - ${release_dir} | (cd ${host_release} ; tar xvf -) ; true)
	(cd ${host_release}/${release_dir} ; find . \( -name CVS -o -name CVS.adm \) -depth -exec rm -rf {} \; )
	mv ${host_release}/${release_dir} ${host_release}/src
	touch $@

.${host}-dirs:
	[ -d ${host} ] || mkdir ${host}
	[ -d ${host_release} ] || mkdir ${host_release}
	[ -d ${objdir} ] || mkdir ${objdir}
	touch $@

.${release_tag}-co: .${release_tag}-tagged
	${CVS} co -r ${release_tag} ${cvs_module}
	mv ${tree} ${release_dir}
	touch $@

.${release_tag}-tagged:
	${CVS} rtag ${release_tag} ${cvs_module}
	touch $@

fluff:	${host_release}/src/Build-A-Release.mk \
	${host_release}/src/Pack-A-Progressive \
	${host_release}/lib/fixincludes \
	${host_release}/src/INSTALL \
	${host_release}/README \
	${host}/Install \
	${host}/install.dvi \
	${host_release}/CYGNUS

${host}/install.dvi: ${host_release}/src/install.texinfo
	cp $< ${host}/install.texinfo
	(cd ${host} ; tex install.texinfo)
	(cd ${host} ; tex install.texinfo)
	(cd ${host} ; tex install.texinfo)
	rm ${host}/install.texinfo

install-notes: ${host}/install.dvi
	lpr $^

clean-fluff:
	rm -f ${host_release}/src/Build-A-Release.mk \
		${host_release}/src/Pack-A-Progressive \
		${host_release}/lib/fixincludes \
		${host_release}/Install \
		${host_release}/src/install-texi.in \
		${host_release}/src/install.texinfo \
		${host_release}/src/Install.in \
		
${host_release}/src/Build-A-Release.mk: ${release_dir}/Build-A-Release.mk
	rm -f $@; cp $< $@

${host_release}/src/Pack-A-Progressive: ${release_dir}/Pack-A-Progressive
	rm -f $@; cp $< $@

${host_release}/src/Install.in: ${release_dir}/Install.in
	rm -f $@; cp $< $@

${host_release}/src/CYGNUS: ${release_dir}/CYGNUS
	rm -f $@; cp $< $@

${host_release}/CYGNUS: ${host_release}/src/CYGNUS
	rm -f $@; cp $< $@

${host_release}/src/install-texi.in: ${release_dir}/sol-inst.texi
	rm -f $@; cp $< $@

${host_release}/lib/fixincludes:	${host_release}/src/gcc/fixincludes
	rm -f $@; cp $< $@
	chmod a+x $@

${host}/Install: ${host_release}/Install
	rm -f $@; cp $< $@
	chmod a+x $@

${host_release}/Install: ${host_release}/src/Install
	rm -f $@; cp $< $@
	chmod a+x $@

${host_release}/src/install.texinfo: ${host_release}/src/install-texi.in
	$(MAKE) process -f Build-A-Release.mk IN=$< OUT=$@ relno=`echo ${release_dir} | sed "/${tree}-/s///"`

${host_release}/README: ${host_release}/src/README-bin
	rm -f $@; cp $< $@

${host_release}/src/INSTALL: ${host_release}/src/install.texinfo
	-makeinfo +no-headers +no-split $< -o $@
### remove tags table
	sed -e '/^Tag Table/,$$d' $@ | sed -e '$$d' > $@.new
	mv -f $@.new $@

${host_release}/src/Install: ${host_release}/src/Install.in
	$(MAKE) process -f Build-A-Release.mk IN=$< OUT=$@ relno=`echo ${release_dir} | sed "/${tree}-/s///"`
	chmod a+x $@

process: process-${host}
	@if grep '||' ${OUT}.new ; then \
		echo $0 missed at least one of the triggers. 1>&2 ; \
		exit 1 ; \
	else \
		true ; \
	fi
	mv -f ${OUT}.new ${OUT}

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
		< ${IN} > ${OUT}.new


process-sparc-sun-solaris2:
	sed \
		-e "/^@set FIXMES/s::@clear FIXMES:" \
		-e "/^@clear CUSTOMER/s::@set CUSTOMER:" \
		-e "/^@set CDROMinst/s::@clear CDROMinst:" \
		-e "/^@clear TAPEinst/s::@set TAPEinst:" \
		-e "/^@set FTPsingle/s::@clear FTPsingle:" \
		-e "/^@set FTPsplit/s::@clear FTPsplit:" \
		-e "/\|\|MEDIUM\|\|/s::tape:g" \
		-e "/\|\|MEDstr\|\|/s::/dev/rmt/0ln:g" \
		-e "/\|\|DEVdflt\|\|/s::/dev/rmt/0ln:g" \
		-e "/\|\|MEDvar\|\|/s::/dev/@var{tape}:g" \
		-e "/\|\|TAPdflt\|\|/s::/dev/@var{tape}:g" \
        	-e "/\|\|RELNO\|\|/s::1.0:g" \
		-e "/\|\|BD\|\|/s::20:g" \
		-e "/\|\|SD\|\|/s::28:g" \
		-e "/\|\|DF\|\|/s::48:g" \
		-e "/\|\|BUNDLE\|\|/s::comp:g" \
		-e "/\|\|HOST\|\|/s::SPARC computer:g" \
		-e "/\|\|HOSTstr\|\|/s::sparc-sun-solaris2:g" \
		-e "/\|\|TARGET\|\|/s::sparc-sun-solaris2:g" \
		-e "/\|\|GCCvn\|\|/s::cygnus-2.0.2:g"  \
		< $(IN) > $(OUT).new

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
		< ${IN} > ${OUT}.new

pclean:
	rm -rf ${host}

blockit: blockit.c



