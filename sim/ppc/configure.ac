dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../.. ../../config])

AC_MSG_CHECKING([for sim ppc bitsize settings])
AC_ARG_ENABLE(sim-bitsize,
[AS_HELP_STRING([--enable-sim-bitsize=n], [Specify target bitsize (32 or 64).])],
[case "${enableval}" in
  32|64) sim_bitsize="-DWITH_TARGET_WORD_BITSIZE=$enableval";;
  *)	 AC_MSG_ERROR("--enable-sim-bitsize was given $enableval.  Expected 32 or 64");;
esac], [sim_bitsize=""])
AC_MSG_RESULT($sim_bitsize)

AC_MSG_CHECKING([for sim ppc decode mechanism])
AC_ARG_ENABLE(sim-decode-mechanism,
[AS_HELP_STRING([--enable-sim-decode-mechanism=which], [Specify the instruction decode mechanism.])],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-decode-mechanism=file");;
  array|switch|padded-switch|goto-switch)	sim_decode_mechanism="-T ${enableval}";;
  *)		AC_MSG_ERROR("File $enableval is not an opcode rules file");;
esac], [sim_decode_mechanism=""])
AC_MSG_RESULT($sim_decode_mechanism)

AC_MSG_CHECKING([for sim ppc default model])
AC_ARG_ENABLE(sim-default-model,
[AS_HELP_STRING([--enable-sim-default-model=which], [Specify default PowerPC to model.])],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-default-model=model");;
  *)		sim_default_model="-DWITH_DEFAULT_MODEL=${enableval}";;
esac], [sim_default_model=""])
AC_MSG_RESULT($sim_default_model)

AC_MSG_CHECKING([for sim ppc duplicate settings])
AC_ARG_ENABLE(sim-duplicate,
[AS_HELP_STRING([--enable-sim-duplicate], [Expand (duplicate) semantic functions.])],
[case "${enableval}" in
  yes)	sim_dup="-E";;
  no)	sim_dup="";;
  *)	AC_MSG_ERROR("--enable-sim-duplicate does not take a value");;
esac], [sim_dup="-E"])
AC_MSG_RESULT($sim_dup)

AC_MSG_CHECKING([for sim ppc filter rules])
AC_ARG_ENABLE(sim-filter,
[AS_HELP_STRING([--enable-sim-filter=rule], [Specify filter rules.])],
[case "${enableval}" in
  yes)	AC_MSG_ERROR("--enable-sim-filter must be specified with a rule to filter or no");;
  no)	sim_filter="";;
  *)	sim_filter="-F $enableval";;
esac], [sim_filter="-F 32,f,o"])
AC_MSG_RESULT($sim_filter)

AC_MSG_CHECKING([for sim ppc float settings])
AC_ARG_ENABLE(sim-float,
[AS_HELP_STRING([--enable-sim-float], [Specify whether the target has hard, soft, altivec or e500 floating point.])],
[case "${enableval}" in
  yes | hard)	sim_float="-DWITH_FLOATING_POINT=HARD_FLOATING_POINT";;
  no | soft)	sim_float="-DWITH_FLOATING_POINT=SOFT_FLOATING_POINT";;
  altivec)      sim_float="-DWITH_ALTIVEC" ; sim_filter="${sim_filter},av" ;;
  *spe*|*simd*) sim_float="-DWITH_E500" ; sim_filter="${sim_filter},e500" ;;
  *)		AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-float");;
esac],
[case "${target}" in
  *altivec*) sim_float="-DWITH_ALTIVEC" ; sim_filter="${sim_filter},av" ;;
  *spe*|*simd*)	sim_float="-DWITH_E500" ; sim_filter="${sim_filter},e500" ;;
  *) sim_float=""
esac])
AC_MSG_RESULT($sim_float)

AC_MSG_CHECKING([for sim ppc hardware settings])
hardware="cpu,memory,nvram,iobus,htab,disk,trace,register,vm,init,core,pal,com,eeprom,opic,glue,phb,ide,sem,shm"
AC_ARG_ENABLE(sim-hardware,
[AS_HELP_STRING([--enable-sim-hardware=list], [Specify the hardware to be included in the build.])],
[case "${enableval}" in
  yes)	;;
  no)	AC_MSG_ERROR("List of hardware must be specified for --enable-sim-hardware");;
  ,*)   hardware="${hardware}${enableval}";;
  *,)   hardware="${enableval}${hardware}";;
  *)	hardware="${enableval}"'';;
esac])
sim_hw_src=`echo $hardware | sed -e 's/,/.c hw_/g' -e 's/^/hw_/' -e s'/$/.c/'`
sim_hw_obj=`echo $sim_hw_src | sed -e 's/\.c/.o/g'`
AC_MSG_RESULT($hardware)

AC_MSG_CHECKING([for sim ppc icache settings])
AC_ARG_ENABLE(sim-icache,
[AS_HELP_STRING([--enable-sim-icache=size], [Specify instruction-decode cache size and type.])],
[icache="-R"
 case "${enableval}" in
  yes)		icache="1024"; sim_icache="-I $icache";;
  no)		sim_icache="-R";;
  *) icache=1024
     sim_icache="-"
     for x in `echo "${enableval}" | sed -e "s/,/ /g"`; do
       case "$x" in
         define)	sim_icache="${sim_icache}R";;
         semantic)	sim_icache="${sim_icache}C";;
	 insn)		sim_icache="${sim_icache}S";;
	 0*|1*|2*|3*|4*|5*|6*|7*|8*|9*)	icache=$x;;
         *)		AC_MSG_ERROR("Unknown value $x for --enable-sim-icache");;
       esac
     done
     sim_icache="${sim_icache}I $icache";;
esac], [sim_icache="-CSRI 1024"])
AC_MSG_RESULT($sim_icache)

AC_MSG_CHECKING([for sim ppc jump settings])
AC_ARG_ENABLE(sim-jump,
[AS_HELP_STRING([--enable-sim-jump], [Jump between semantic code (instead of call/return).])],
[case "${enableval}" in
  yes)	sim_jump="-J";;
  no)	sim_jump="";;
  *)	AC_MSG_ERROR("--enable-sim-jump does not take a value");;
esac], [sim_jump=""])
AC_MSG_RESULT($sim_jump)

AC_MSG_CHECKING([for sim ppc source debug line numbers])
AC_ARG_ENABLE(sim-line-nr,
[AS_HELP_STRING([--enable-sim-line-nr=opts], [Generate extra CPP code that references source rather than generated code])],
[case "${enableval}" in
  yes)	sim_line_nr="";;
  no)	sim_line_nr="-L";;
  *)	AC_MSG_ERROR("--enable-sim-line-nr does not take a value");;
esac], [sim_line_nr=""])
AC_MSG_RESULT($sim_line_nr)

AC_MSG_CHECKING([for sim ppc model])
AC_ARG_ENABLE(sim-model,
[AS_HELP_STRING([--enable-sim-model=which], [Specify PowerPC to model.])],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-model=model");;
  *)		sim_model="-DWITH_MODEL=${enableval}";;
esac], [sim_model=""])
AC_MSG_RESULT($sim_model)

AC_MSG_CHECKING([for sim ppc model issue])
AC_ARG_ENABLE(sim-model-issue,
[AS_HELP_STRING([--enable-sim-model-issue], [Specify whether to simulate model specific actions])],
[case "${enableval}" in
  yes)	sim_model_issue="-DWITH_MODEL_ISSUE=MODEL_ISSUE_PROCESS";;
  no)	sim_model_issue="-DWITH_MODEL_ISSUE=MODEL_ISSUE_IGNORE";;
  *)	AC_MSG_ERROR("--enable-sim-model-issue does not take a value");;
esac], [sim_model_issue=""])
AC_MSG_RESULT($sim_model_issue)

AC_MSG_CHECKING([for sim ppc event monitoring])
AC_ARG_ENABLE(sim-monitor,
[AS_HELP_STRING([--enable-sim-monitor=mon], [Specify whether to enable monitoring events.])],
[case "${enableval}" in
  yes)		sim_monitor="-DWITH_MON='MONITOR_INSTRUCTION_ISSUE | MONITOR_LOAD_STORE_UNIT'";;
  no)		sim_monitor="-DWITH_MON=0";;
  instruction)	sim_monitor="-DWITH_MON=MONITOR_INSTRUCTION_ISSUE";;
  memory)	sim_monitor="-DWITH_MON=MONITOR_LOAD_STORE_UNIT";;
  *)		AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-mon");;
esac], [sim_monitor=""])
AC_MSG_RESULT($sim_monitor)

AC_MSG_CHECKING([for sim ppc opcode lookup settings])
AC_ARG_ENABLE(sim-opcode,
[AS_HELP_STRING([--enable-sim-opcode=which], [Override default opcode lookup.])],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-opcode=file");;
  *)		if test -f "${srcdir}/${enableval}"; then
		  sim_opcode="${enableval}"
		elif test -f "${srcdir}/dc-${enableval}"; then
		  sim_opcode="dc-${enableval}"
		else
		  AC_MSG_ERROR("File $enableval is not an opcode rules file")
		fi;;
esac], [sim_opcode="dc-complex"])
AC_MSG_RESULT($sim_opcode)

AC_MSG_CHECKING([for sim ppc smp settings])
AC_ARG_ENABLE(sim-smp,
[AS_HELP_STRING([--enable-sim-smp=n], [Specify number of processors to configure for.])],
[case "${enableval}" in
  yes)	sim_smp="-DWITH_SMP=5" ; sim_igen_smp="-N 5";;
  no)	sim_smp="-DWITH_SMP=0" ; sim_igen_smp="-N 0";;
  *)	sim_smp="-DWITH_SMP=$enableval" ; sim_igen_smp="-N $enableval";;
esac], [sim_smp="-DWITH_SMP=5" ; sim_igen_smp="-N 5"])
AC_MSG_RESULT($sim_smp)

AC_MSG_CHECKING([for sim ppc switch table settings])
AC_ARG_ENABLE(sim-switch,
[AS_HELP_STRING([--enable-sim-switch], [Use a switch instead of a table for instruction call.])],
[case "${enableval}" in
  yes)	sim_switch="-DWITH_SPREG_SWITCH_TABLE";;
  no)	sim_switch="";;
  *)	AC_MSG_ERROR("--enable-sim-switch does not take a value");;
esac], [sim_switch=""])
AC_MSG_RESULT($sim_switch)

AC_MSG_CHECKING([for sim ppc timebase])
AC_ARG_ENABLE(sim-timebase,
[AS_HELP_STRING([--enable-sim-timebase], [Specify whether the PPC timebase is supported.])],
[case "${enableval}" in
  yes)	sim_timebase="-DWITH_TIME_BASE=1";;
  no)	sim_timebase="-DWITH_TIME_BASE=0";;
  *)	AC_MSG_ERROR("--enable-sim-timebase does not take a value");;
esac], [sim_timebase=""])
AC_MSG_RESULT($sim_timebase)

AC_MSG_CHECKING([for sim ppc xor endian settings])
AC_ARG_ENABLE(sim-xor-endian,
[AS_HELP_STRING([--enable-sim-xor-endian=n], [Specify number bytes involved in PowerPC XOR bi-endian mode (default 8).])],
[case "${enableval}" in
  yes)	sim_xor_endian="-DWITH_XOR_ENDIAN=8";;
  no)	sim_xor_endian="-DWITH_XOR_ENDIAN=0";;
  *)	sim_xor_endian="-DWITH_XOR_ENDIAN=$enableval";;
esac], [sim_xor_endian=""])
AC_MSG_RESULT($sim_xor_endian)

AC_SUBST(sim_line_nr)
AC_SUBST(sim_opcode)
AC_SUBST(sim_switch)
AC_SUBST(sim_dup)
AC_SUBST(sim_decode_mechanism)
AC_SUBST(sim_jump)
AC_SUBST(sim_filter)
AC_SUBST(sim_icache)
AC_SUBST(sim_hw_src)
AC_SUBST(sim_hw_obj)
AC_SUBST(sim_xor_endian)
AC_SUBST(sim_smp)
AC_SUBST(sim_igen_smp)
AC_SUBST(sim_bitsize)
AC_SUBST(sim_timebase)
AC_SUBST(sim_float)
AC_SUBST(sim_monitor)
AC_SUBST(sim_model)
AC_SUBST(sim_default_model)
AC_SUBST(sim_model_issue)

AC_OUTPUT(Makefile)
