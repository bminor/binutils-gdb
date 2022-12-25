## See sim/Makefile.am
##
## Copyright (C) 1995-2022 Free Software Foundation, Inc.
## Written by Cygnus Support.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

%C%_SIM_EXTRA_HW_DEVICES = tx3904cpu tx3904irc tx3904tmr tx3904sio
AM_MAKEFLAGS += %C%_SIM_EXTRA_HW_DEVICES="$(%C%_SIM_EXTRA_HW_DEVICES)"

%C%_BUILT_SRC_FROM_IGEN_ITABLE = \
	%D%/itable.h \
	%D%/itable.c
%C%_BUILT_SRC_FROM_GEN_MODE_SINGLE = \
	%D%/icache.h \
	%D%/icache.c \
	%D%/idecode.h \
	%D%/idecode.c \
	%D%/semantics.h \
	%D%/semantics.c \
	%D%/model.h \
	%D%/model.c \
	%D%/support.h \
	%D%/support.c \
	%D%/engine.h \
	%D%/engine.c \
	%D%/irun.c
%C%_BUILT_SRC_FROM_GEN_MODE_M16_M16 = \
	%D%/m16_icache.h \
	%D%/m16_icache.c \
	%D%/m16_idecode.h \
	%D%/m16_idecode.c \
	%D%/m16_semantics.h \
	%D%/m16_semantics.c \
	%D%/m16_model.h \
	%D%/m16_model.c \
	%D%/m16_support.h \
	%D%/m16_support.c \
%C%_BUILT_SRC_FROM_GEN_MODE_M16_M32 = \
	%D%/m32_icache.h \
	%D%/m32_icache.c \
	%D%/m32_idecode.h \
	%D%/m32_idecode.c \
	%D%/m32_semantics.h \
	%D%/m32_semantics.c \
	%D%/m32_model.h \
	%D%/m32_model.c \
	%D%/m32_support.h \
	%D%/m32_support.c
%C%_BUILD_OUTPUTS = \
	$(%C%_BUILT_SRC_FROM_IGEN_ITABLE) \
	%D%/stamp-igen-itable
if SIM_MIPS_GEN_MODE_SINGLE
%C%_BUILD_OUTPUTS += \
	$(%C%_BUILT_SRC_FROM_GEN_MODE_SINGLE) \
	%D%/stamp-gen-mode-single
endif
if SIM_MIPS_GEN_MODE_M16
%C%_BUILD_OUTPUTS += \
	$(%C%_BUILT_SRC_FROM_GEN_MODE_M16_M16) \
	$(%C%_BUILT_SRC_FROM_GEN_MODE_M16_M32) \
	%D%/stamp-gen-mode-m16-m16 \
	%D%/stamp-gen-mode-m16-m32
endif
if SIM_MIPS_GEN_MODE_MULTI
%C%_BUILD_OUTPUTS += \
	$(SIM_MIPS_MULTI_SRC) \
	%D%/stamp-gen-mode-multi-igen \
	%D%/stamp-gen-mode-multi-run
endif

## This makes sure build tools are available before building the arch-subdirs.
SIM_ALL_RECURSIVE_DEPS += $(%C%_BUILD_OUTPUTS)

$(%C%_BUILT_SRC_FROM_IGEN_ITABLE): %D%/stamp-igen-itable
$(%C%_BUILT_SRC_FROM_GEN_MODE_SINGLE): %D%/stamp-gen-mode-single
$(%C%_BUILT_SRC_FROM_GEN_MODE_M16_M16): %D%/stamp-gen-mode-m16-m16
$(%C%_BUILT_SRC_FROM_GEN_MODE_M16_M32): %D%/stamp-gen-mode-m16-m32
$(SIM_MIPS_MULTI_SRC): %D%/stamp-gen-mode-multi-igen %D%/stamp-gen-mode-multi-run

%C%_IGEN_TRACE = # -G omit-line-numbers # -G trace-rule-selection -G trace-rule-rejection -G trace-entries # -G trace-all
%C%_IGEN_INSN = $(srcdir)/%D%/mips.igen
%C%_IGEN_INSN_INC = \
	%D%/dsp.igen \
	%D%/dsp2.igen \
	%D%/m16.igen \
	%D%/m16e.igen \
	%D%/mdmx.igen \
	%D%/micromipsdsp.igen \
	%D%/micromips.igen \
	%D%/mips3264r2.igen \
	%D%/mips3264r6.igen \
	%D%/mips3d.igen \
	%D%/sb1.igen \
	%D%/tx.igen \
	%D%/vr.igen
%C%_IGEN_DC = $(srcdir)/%D%/mips.dc
%C%_M16_DC = $(srcdir)/%D%/m16.dc
%C%_MICROMIPS32_DC = $(srcdir)/%D%/micromips.dc
%C%_MICROMIPS16_DC = $(srcdir)/%D%/micromips16.dc

## NB:	Since these can be built by a number of generators, care
##	must be taken to ensure that they are only dependant on
##	one of those generators.
%D%/stamp-igen-itable: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(IGEN)
	$(AM_V_GEN)$(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		-Wnowidth \
		-Wnounimplemented \
		$(SIM_MIPS_IGEN_ITABLE_FLAGS) \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-i $(%C%_IGEN_INSN) \
		-n itable.h    -ht %D%/itable.h \
		-n itable.c    -t  %D%/itable.c
	$(AM_V_at)touch $@

%D%/stamp-gen-mode-single: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(%C%_IGEN_DC) $(IGEN)
	$(AM_V_GEN)$(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		$(SIM_MIPS_SINGLE_FLAGS) \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-B 32 \
		-H 31 \
		-i $(%C%_IGEN_INSN) \
		-o $(%C%_IGEN_DC) \
		-x \
		-n icache.h    -hc %D%/icache.h \
		-n icache.c    -c  %D%/icache.c \
		-n semantics.h -hs %D%/semantics.h \
		-n semantics.c -s  %D%/semantics.c \
		-n idecode.h   -hd %D%/idecode.h \
		-n idecode.c   -d  %D%/idecode.c \
		-n model.h     -hm %D%/model.h \
		-n model.c     -m  %D%/model.c \
		-n support.h   -hf %D%/support.h \
		-n support.c   -f  %D%/support.c \
		-n engine.h    -he %D%/engine.h \
		-n engine.c    -e  %D%/engine.c \
		-n irun.c      -r  %D%/irun.c
	$(AM_V_at)touch $@

%D%/stamp-gen-mode-m16-m16: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(%C%_M16_DC) $(IGEN)
	$(AM_V_GEN)$(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		$(SIM_MIPS_M16_FLAGS) \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-B 16 \
		-H 15 \
		-i $(%C%_IGEN_INSN) \
		-o $(%C%_M16_DC) \
		-P m16_ \
		-x \
		-n m16_icache.h    -hc %D%/m16_icache.h \
		-n m16_icache.c    -c  %D%/m16_icache.c \
		-n m16_semantics.h -hs %D%/m16_semantics.h \
		-n m16_semantics.c -s  %D%/m16_semantics.c \
		-n m16_idecode.h   -hd %D%/m16_idecode.h \
		-n m16_idecode.c   -d  %D%/m16_idecode.c \
		-n m16_model.h     -hm %D%/m16_model.h \
		-n m16_model.c     -m  %D%/m16_model.c \
		-n m16_support.h   -hf %D%/m16_support.h \
		-n m16_support.c   -f  %D%/m16_support.c
	$(AM_V_at)touch $@

%D%/stamp-gen-mode-m16-m32: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(%C%_IGEN_DC) $(IGEN)
	$(AM_V_GEN)$(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		$(SIM_MIPS_SINGLE_FLAGS) \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-B 32 \
		-H 31 \
		-i $(%C%_IGEN_INSN) \
		-o $(%C%_IGEN_DC) \
		-P m32_ \
		-x \
		-n m32_icache.h    -hc %D%/m32_icache.h \
		-n m32_icache.c    -c  %D%/m32_icache.c \
		-n m32_semantics.h -hs %D%/m32_semantics.h \
		-n m32_semantics.c -s  %D%/m32_semantics.c \
		-n m32_idecode.h   -hd %D%/m32_idecode.h \
		-n m32_idecode.c   -d  %D%/m32_idecode.c \
		-n m32_model.h     -hm %D%/m32_model.h \
		-n m32_model.c     -m  %D%/m32_model.c \
		-n m32_support.h   -hf %D%/m32_support.h \
		-n m32_support.c   -f  %D%/m32_support.c
	$(AM_V_at)touch $@

%D%/stamp-gen-mode-multi-igen: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(%C%_IGEN_DC) $(%C%_M16_DC) $(%C%_MICROMIPS32_DC) $(%C%_MICROMIPS16_DC) $(IGEN)
	$(AM_V_GEN)\
	for t in $(SIM_MIPS_MULTI_IGEN_CONFIGS); do \
	  p=`echo $${t} | sed -e 's/:.*//'` ; \
	  m=`echo $${t} | sed -e 's/.*:\(.*\):.*/\1/'` ; \
	  f=`echo $${t} | sed -e 's/.*://'` ; \
	  case $${p} in \
	    micromips16*) \
	      e="-B 16 -H 15 -o $(%C%_MICROMIPS16_DC) -F 16" ;; \
	    micromips32* | micromips64*) \
	      e="-B 32 -H 31 -o $(%C%_MICROMIPS32_DC) -F $${f}" ;; \
	    micromips_m32*) \
	      e="-B 32 -H 31 -o $(%C%_IGEN_DC) -F $${f}"; \
	      m="mips32r2,mips3d,mdmx,dsp,dsp2,smartmips" ;; \
	    micromips_m64*) \
	      e="-B 32 -H 31 -o $(%C%_IGEN_DC) -F $${f}"; \
	      m="mips64r2,mips3d,mdmx,dsp,dsp2,smartmips" ;; \
	    m16*) \
	      e="-B 16 -H 15 -o $(%C%_M16_DC) -F 16" ;; \
	    *) \
	      e="-B 32 -H 31 -o $(%C%_IGEN_DC) -F $${f}" ;; \
	  esac; \
	  $(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		$${e} \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		-M $${m} \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-i $(%C%_IGEN_INSN) \
		-P $${p}_ \
		-x \
		-n $${p}_icache.h    -hc %D%/$${p}_icache.h \
		-n $${p}_icache.c    -c  %D%/$${p}_icache.c \
		-n $${p}_semantics.h -hs %D%/$${p}_semantics.h \
		-n $${p}_semantics.c -s  %D%/$${p}_semantics.c \
		-n $${p}_idecode.h   -hd %D%/$${p}_idecode.h \
		-n $${p}_idecode.c   -d  %D%/$${p}_idecode.c \
		-n $${p}_model.h     -hm %D%/$${p}_model.h \
		-n $${p}_model.c     -m  %D%/$${p}_model.c \
		-n $${p}_support.h   -hf %D%/$${p}_support.h \
		-n $${p}_support.c   -f  %D%/$${p}_support.c \
		-n $${p}_engine.h    -he %D%/$${p}_engine.h \
		-n $${p}_engine.c    -e  %D%/$${p}_engine.c \
	    || exit; \
	done
	$(AM_V_at)touch $@

%D%/stamp-gen-mode-multi-run: %D%/m16run.c %D%/micromipsrun.c
	$(AM_V_GEN)\
	for t in $(SIM_MIPS_MULTI_IGEN_CONFIGS); do \
	  case $${t} in \
	    m16*) \
	      m=`echo $${t} | sed -e 's/^m16//' -e 's/:.*//'`; \
	      o=%D%/m16$${m}_run.c; \
	      sed < $(srcdir)/%D%/m16run.c > $$o.tmp \
		    -e "s/^sim_/m16$${m}_/" \
		    -e "/include/s/sim-engine/m16$${m}_engine/" \
		    -e "s/m16_/m16$${m}_/" \
		    -e "s/m32_/m32$${m}_/" \
		    || exit 1; \
	      $(SHELL) $(srcroot)/move-if-change $$o.tmp $$o; \
	      ;;\
	    micromips32*) \
	      m=`echo $${t} | sed -e 's/^micromips32//' -e 's/:.*//'`; \
	      o=%D%/micromips$${m}_run.c; \
	      sed < $(srcdir)/%D%/micromipsrun.c > $$o.tmp \
		    -e "s/^sim_/micromips32$${m}_/" \
		    -e "/include/s/sim-engine/micromips32$${m}_engine/" \
		    -e "s/micromips16_/micromips16$${m}_/" \
		    -e "s/micromips32_/micromips32$${m}_/" \
		    -e "s/m32_/m32$${m}_/" \
		    || exit 1; \
	      $(SHELL) $(srcroot)/move-if-change $$o.tmp $$o; \
	      ;;\
	    micromips64*) \
	      m=`echo $${t} | sed -e 's/^micromips64//' -e 's/:.*//'`; \
	      o=%D%/micromips$${m}_run.c; \
	      sed < $(srcdir)/%D%/micromipsrun.c > $$o.tmp \
		    -e "s/^sim_/micromips64$${m}_/" \
		    -e "/include/s/sim-engine/micromips64$${m}_engine/" \
		    -e "s/micromips16_/micromips16$${m}_/" \
		    -e "s/micromips32_/micromips64$${m}_/" \
		    -e "s/m32_/m64$${m}_/" \
		    || exit 1; \
	      $(SHELL) $(srcroot)/move-if-change $$o.tmp $$o; \
	      ;;\
	  esac \
	done
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)
## These are created by mips/acinclude.m4 during configure time.
DISTCLEANFILES += %D%/multi-include.h %D%/multi-run.c
