dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../.. ../../config])

AC_PROG_INSTALL
AC_PROG_CC

dnl The sim shouldn't be checking $target and changing behavior.  But it is,
dnl and until we clean that up, we need to expand --target for use below.
AC_CANONICAL_SYSTEM

AC_ARG_ENABLE(sim-bitsize,
[  --enable-sim-bitsize=n		Specify target bitsize (32 or 64).],
[case "${enableval}" in
  32|64) sim_bitsize="-DWITH_TARGET_WORD_BITSIZE=$enableval";;
  *)	 AC_MSG_ERROR("--enable-sim-bitsize was given $enableval.  Expected 32 or 64"); sim_bitsize="";;
esac
if test x"$silent" != x"yes" && test x"$sim_bitsize" != x""; then
  echo "Setting bitsize flags = $sim_bitsize" 6>&1
fi],[sim_bitsize=""])dnl


AC_ARG_ENABLE(sim-decode-mechanism,
[  --enable-sim-decode-mechanism=which		Specify the instruction decode mechanism.],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-decode-mechanism=file");;
  array|switch|padded-switch|goto-switch)	sim_decode_mechanism="-T ${enableval}";;
  *)		AC_MSG_ERROR("File $enableval is not an opcode rules file");
		sim_decode_mechanism="switch";;
esac
if test x"$silent" != x"yes" && test x"$sim_decode_mechanism" != x""; then
  echo "Setting decode mechanism flags = $sim_decode_mechanism" 6>&1
fi],[sim_decode_mechanism=""
if test x"$silent" != x"yes"; then
  echo "Setting decode mechanism flags = $sim_decode_mechanism"
fi])dnl


AC_ARG_ENABLE(sim-default-model,
[  --enable-sim-default-model=which	Specify default PowerPC to model.],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-default-model=model");;
  *)		sim_default_model="-DWITH_DEFAULT_MODEL=${enableval}";;
esac
if test x"$silent" != x"yes" && test x"$sim_default_model" != x""; then
  echo "Setting default-model flags = $sim_default_model" 6>&1
fi],[sim_default_model=""])dnl


AC_ARG_ENABLE(sim-duplicate,
[  --enable-sim-duplicate		Expand (duplicate) semantic functions.],
[case "${enableval}" in
  yes)	sim_dup="-E";;
  no)	sim_dup="";;
  *)	AC_MSG_ERROR("--enable-sim-duplicate does not take a value"); sim_dup="";;
esac
if test x"$silent" != x"yes" && test x"$sim_dup" != x""; then
  echo "Setting duplicate flags = $sim_dup" 6>&1
fi],[sim_dup="-E"
if test x"$silent" != x"yes"; then
  echo "Setting duplicate flags = $sim_dup" 6>&1
fi])dnl


AC_ARG_ENABLE(sim-filter,
[  --enable-sim-filter=rule		Specify filter rules.],
[case "${enableval}" in
  yes)	AC_MSG_ERROR("--enable-sim-filter must be specified with a rule to filter or no"); sim_filter="";;
  no)	sim_filter="";;
  *)	sim_filter="-F $enableval";;
esac
if test x"$silent" != x"yes" && test x"$sim_filter" != x""; then
  echo "Setting filter flags = $sim_filter" 6>&1
fi],[sim_filter="-F 32,f,o"
if test x"$silent" != x"yes"; then
  echo "Setting filter flags = $sim_filter" 6>&1
fi])dnl


AC_ARG_ENABLE(sim-float,
[  --enable-sim-float			Specify whether the target has hard, soft, altivec or e500 floating point.],
[case "${enableval}" in
  yes | hard)	sim_float="-DWITH_FLOATING_POINT=HARD_FLOATING_POINT";;
  no | soft)	sim_float="-DWITH_FLOATING_POINT=SOFT_FLOATING_POINT";;
  altivec)      sim_float="-DWITH_ALTIVEC" ; sim_filter="${sim_filter},av" ;;
  *spe*|*simd*) sim_float="-DWITH_E500" ; sim_filter="${sim_filter},e500" ;;
  *)		AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-float"); sim_float="";;
esac
if test x"$silent" != x"yes" && test x"$sim_float" != x""; then
  echo "Setting float flags = $sim_float" 6>&1
fi],[
case "${target}" in
  *altivec*) sim_float="-DWITH_ALTIVEC" ; sim_filter="${sim_filter},av" ;;
  *spe*|*simd*)	sim_float="-DWITH_E500" ; sim_filter="${sim_filter},e500" ;;
  *) sim_float=""
esac
])dnl


AC_ARG_ENABLE(sim-hardware,
[  --enable-sim-hardware=list		Specify the hardware to be included in the build.],
[hardware="cpu,memory,nvram,iobus,htab,disk,trace,register,vm,init,core,pal,com,eeprom,opic,glue,phb,ide,sem,shm"
case "${enableval}" in
  yes)	;;
  no)	AC_MSG_ERROR("List of hardware must be specified for --enable-sim-hardware"); hardware="";;
  ,*)   hardware="${hardware}${enableval}";;
  *,)   hardware="${enableval}${hardware}";;
  *)	hardware="${enableval}"'';;
esac
sim_hw_src=`echo $hardware | sed -e 's/,/.c hw_/g' -e 's/^/hw_/' -e s'/$/.c/'`
sim_hw_obj=`echo $sim_hw_src | sed -e 's/\.c/.o/g'`
if test x"$silent" != x"yes" && test x"$hardware" != x""; then
  echo "Setting hardware to $sim_hw_src, $sim_hw_obj"
fi],[hardware="cpu,memory,nvram,iobus,htab,disk,trace,register,vm,init,core,pal,com,eeprom,opic,glue,phb,ide,sem,shm"
sim_hw_src=`echo $hardware | sed -e 's/,/.c hw_/g' -e 's/^/hw_/' -e s'/$/.c/'`
sim_hw_obj=`echo $sim_hw_src | sed -e 's/\.c/.o/g'`
if test x"$silent" != x"yes"; then
  echo "Setting hardware to $sim_hw_src, $sim_hw_obj"
fi])dnl


AC_ARG_ENABLE(sim-icache,
[  --enable-sim-icache=size		Specify instruction-decode cache size and type.],
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
         *)		AC_MSG_ERROR("Unknown value $x for --enable-sim-icache"); sim_icache="";;
       esac
     done
     sim_icache="${sim_icache}I $icache";;
esac
if test x"$silent" != x"yes" && test x"$icache" != x""; then
  echo "Setting instruction cache size to $icache ($sim_icache)"
fi],[sim_icache="-CSRI 1024"
if test x"$silent" != x"yes"; then
  echo "Setting instruction cache size to 1024 ($sim_icache)"
fi])dnl


AC_ARG_ENABLE(sim-jump,
[  --enable-sim-jump		Jump between semantic code (instead of call/return).],
[case "${enableval}" in
  yes)	sim_jump="-J";;
  no)	sim_jump="";;
  *)	AC_MSG_ERROR("--enable-sim-jump does not take a value"); sim_jump="";;
esac
if test x"$silent" != x"yes" && test x"$sim_jump" != x""; then
  echo "Setting jump flag = $sim_jump" 6>&1
fi],[sim_jump=""
if test x"$silent" != x"yes"; then
  echo "Setting jump flag = $sim_jump" 6>&1
fi])dnl


AC_ARG_ENABLE(sim-line-nr,
[  --enable-sim-line-nr=opts		Generate extra CPP code that references source rather than generated code],
[case "${enableval}" in
  yes)	sim_line_nr="";;
  no)	sim_line_nr="-L";;
  *)	AC_MSG_ERROR("--enable-sim-line-nr does not take a value"); sim_line_nr="";;
esac
if test x"$silent" != x"yes" && test x"$sim_line_nr" != x""; then
  echo "Setting warning flags = $sim_line_nr" 6>&1
fi],[sim_line_nr=""])dnl


AC_ARG_ENABLE(sim-model,
[  --enable-sim-model=which		Specify PowerPC to model.],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-model=model");;
  *)		sim_model="-DWITH_MODEL=${enableval}";;
esac
if test x"$silent" != x"yes" && test x"$sim_model" != x""; then
  echo "Setting model flags = $sim_model" 6>&1
fi],[sim_model=""])dnl


AC_ARG_ENABLE(sim-model-issue,
[  --enable-sim-model-issue		Specify whether to simulate model specific actions],
[case "${enableval}" in
  yes)	sim_model_issue="-DWITH_MODEL_ISSUE=MODEL_ISSUE_PROCESS";;
  no)	sim_model_issue="-DWITH_MODEL_ISSUE=MODEL_ISSUE_IGNORE";;
  *)	AC_MSG_ERROR("--enable-sim-model-issue does not take a value"); sim_model_issue="";;
esac
if test x"$silent" != x"yes"; then
  echo "Setting model-issue flags = $sim_model_issue" 6>&1
fi],[sim_model_issue=""])dnl


AC_ARG_ENABLE(sim-monitor,
[  --enable-sim-monitor=mon		Specify whether to enable monitoring events.],
[case "${enableval}" in
  yes)		sim_monitor="-DWITH_MON='MONITOR_INSTRUCTION_ISSUE | MONITOR_LOAD_STORE_UNIT'";;
  no)		sim_monitor="-DWITH_MON=0";;
  instruction)	sim_monitor="-DWITH_MON=MONITOR_INSTRUCTION_ISSUE";;
  memory)	sim_monitor="-DWITH_MON=MONITOR_LOAD_STORE_UNIT";;
  *)		AC_MSG_ERROR("Unknown value $enableval passed to --enable-sim-mon");;
esac
if test x"$silent" != x"yes" && test x"$sim_monitor" != x""; then
  echo "Setting monitor flags = $sim_monitor" 6>&1
fi],[sim_monitor=""])dnl


AC_ARG_ENABLE(sim-opcode,
[  --enable-sim-opcode=which		Override default opcode lookup.],
[case "${enableval}" in
  yes|no)	AC_MSG_ERROR("No value supplied for --enable-sim-opcode=file");;
  *)		if test -f "${srcdir}/${enableval}"; then
		  sim_opcode="${enableval}"
		elif test -f "${srcdir}/dc-${enableval}"; then
		  sim_opcode="dc-${enableval}"
		else
		  AC_MSG_ERROR("File $enableval is not an opcode rules file");
		  sim_opcode="dc-complex"
		fi;;
esac
if test x"$silent" != x"yes" && test x"$sim_opcode" != x""; then
  echo "Setting opcode flags = $sim_opcode" 6>&1
fi],[sim_opcode="dc-complex"
if test x"$silent" != x"yes"; then
  echo "Setting opcode flags = $sim_opcode"
fi])dnl


AC_ARG_ENABLE(sim-smp,
[  --enable-sim-smp=n			Specify number of processors to configure for.],
[case "${enableval}" in
  yes)	sim_smp="-DWITH_SMP=5" ; sim_igen_smp="-N 5";;
  no)	sim_smp="-DWITH_SMP=0" ; sim_igen_smp="-N 0";;
  *)	sim_smp="-DWITH_SMP=$enableval" ; sim_igen_smp="-N $enableval";;
esac
if test x"$silent" != x"yes" && test x"$sim_smp" != x""; then
  echo "Setting smp flags = $sim_smp" 6>&1
fi],[sim_smp="-DWITH_SMP=5" ; sim_igen_smp="-N 5"
if test x"$silent" != x"yes"; then
  echo "Setting smp flags = $sim_smp" 6>&1
fi])dnl


AC_ARG_ENABLE(sim-switch,
[  --enable-sim-switch			Use a switch instead of a table for instruction call.],
[case "${enableval}" in
  yes)	sim_switch="-DWITH_SPREG_SWITCH_TABLE";;
  no)	sim_switch="";;
  *)	AC_MSG_ERROR("--enable-sim-switch does not take a value"); sim_switch="";;
esac
if test x"$silent" != x"yes" && test x"$sim_switch" != x""; then
  echo "Setting switch flags = $sim_switch" 6>&1
fi],[sim_switch="";
if test x"$silent" != x"yes"; then
  echo "Setting switch flags = $sim_switch" 6>&1
fi])dnl


AC_ARG_ENABLE(sim-timebase,
[  --enable-sim-timebase			Specify whether the PPC timebase is supported.],
[case "${enableval}" in
  yes)	sim_timebase="-DWITH_TIME_BASE=1";;
  no)	sim_timebase="-DWITH_TIME_BASE=0";;
  *)	AC_MSG_ERROR("--enable-sim-timebase does not take a value"); sim_timebase="";;
esac
if test x"$silent" != x"yes" && test x"$sim_timebase" != x""; then
  echo "Setting timebase flags = $sim_timebase" 6>&1
fi],[sim_timebase=""])dnl


AC_ARG_ENABLE(sim-xor-endian,
[  --enable-sim-xor-endian=n		Specify number bytes involved in PowerPC XOR bi-endian mode (default 8).],
[case "${enableval}" in
  yes)	sim_xor_endian="-DWITH_XOR_ENDIAN=8";;
  no)	sim_xor_endian="-DWITH_XOR_ENDIAN=0";;
  *)	sim_xor_endian="-DWITH_XOR_ENDIAN=$enableval";;
esac
if test x"$silent" != x"yes" && test x"$sim_xor_endian" != x""; then
  echo "Setting xor-endian flag = $sim_xor_endian" 6>&1
fi],[sim_xor_endian=""])dnl


AC_CONFIG_HEADER(config.h:config.in)


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

AC_OUTPUT(Makefile,
[case x$CONFIG_HEADERS in xconfig.h:config.in) echo > stamp-h ;; esac])
