# genscripts.sh
# This shell script does the work of generating the ld-emulation-target
# specific information from a specific file of paramaters.
# Usage: genscripts.sh srcdir libdir host_alias target_alias emulation_name
# Sample usage:
# genscripts.sh /offsite/djm/work/devo/ld /usr/local/lib sparc-sun-sunos4.1.3 sparc-sun-sunos4.1.3 sun3.sh
# produces sun3.x sun3.xbn sun3.xn sun3.xr sun3.xu em_sun3.c

srcdir=$1
libdir=$2
host_alias=$3
target_alias=$4

# Include the emulation-specific parameters:
. ${srcdir}/emulparams/$5

# Set the library search path, for libraries named by -lfoo.
# If LIB_PATH is defined (e.g., by Makefile) and non-empty, it is used.
# Otherwise, the default is set here.
#
# The format is the usual list of colon-separated directories.
# To force a logically empty LIB_PATH, do LIBPATH=":".

if [ "x${LIB_PATH}" = "x" ] ; then
   if [ "x${host_alias}" = "x${target_alias}" ] ; then
      # Native.
      LIB_PATH=/lib:/usr/lib:${libdir}
      if [ "${libdir}" != /usr/local/lib ] ; then
        LIB_PATH=${LIB_PATH}:/usr/local/lib
      fi
   else
      # Cross.
      LIB_PATH=
   fi
fi
LIB_SEARCH_DIRS=`echo ${LIB_PATH} | tr ':' ' ' | sed -e 's/\([^ ][^ ]*\)/SEARCH_DIR(\1);/g'`

# Generate 5 script files from a master script template in
# ${srcdir}/scripttempl/${SCRIPT_NAME}.sh.  Which one of the 5 script files
# is actually used depends on command line options given to ld.
#
# A .x script file is the default script.
# A .xr script is for linking without relocation (-r flag).
# A .xu script is like .xr, but *do* create constructors (-Ur flag).
# A .xn script is for linking with -n flag (mix text and data on same page).
# A .xbn script is for linking with -N flag (mix text and data on same page).

SEGMENT_SIZE=${SEGMENT_SIZE-${PAGE_SIZE}}

# Determine DATA_ALIGNMENT for the 5 variants, using
# values specified in the emulparams/<emulation>.sh file or default.

DATA_ALIGNMENT_="${DATA_ALIGNMENT_-${DATA_ALIGNMENT-ALIGN(${SEGMENT_SIZE})}}"
DATA_ALIGNMENT_n="${DATA_ALIGNMENT_n-${DATA_ALIGNMENT_}}"
DATA_ALIGNMENT_N="${DATA_ALIGNMENT_N-${DATA_ALIGNMENT-.}}"
DATA_ALIGNMENT_r="${DATA_ALIGNMENT_r-${DATA_ALIGNMENT-}}"
DATA_ALIGNMENT_u="${DATA_ALIGNMENT_u-${DATA_ALIGNMENT_r}}"

LD_FLAG=r
DATA_ALIGNMENT=${DATA_ALIGNMENT_r}
DEFAULT_DATA_ALIGNMENT="ALIGN(${SEGMENT_SIZE})"
(. ${srcdir}/scripttempl/${SCRIPT_NAME}.sc) | sed -e '/^ *$/d' > \
  ldscripts/${EMULATION_NAME}.xr

LD_FLAG=u
DATA_ALIGNMENT=${DATA_ALIGNMENT_u}
CONSTRUCTING=
(. ${srcdir}/scripttempl/${SCRIPT_NAME}.sc) | sed -e '/^ *$/d' > \
  ldscripts/${EMULATION_NAME}.xu

LD_FLAG=
DATA_ALIGNMENT=${DATA_ALIGNMENT_}
RELOCATING=
(. ${srcdir}/scripttempl/${SCRIPT_NAME}.sc) | sed -e '/^ *$/d' > \
  ldscripts/${EMULATION_NAME}.x

LD_FLAG=n
DATA_ALIGNMENT=${DATA_ALIGNMENT_n}
TEXT_START_ADDR=${NONPAGED_TEXT_START_ADDR-${TEXT_START_ADDR}}
(. ${srcdir}/scripttempl/${SCRIPT_NAME}.sc) | sed -e '/^ *$/d' > \
  ldscripts/${EMULATION_NAME}.xn

LD_FLAG=N
DATA_ALIGNMENT=${DATA_ALIGNMENT_N}
(. ${srcdir}/scripttempl/${SCRIPT_NAME}.sc) | sed -e '/^ *$/d' > \
  ldscripts/${EMULATION_NAME}.xbn

#sed -e s/"<ldtarget>"/${EMULATION_NAME}/g -e s/"<arch>"/${ARCH}/g \
# -e s/"<target>"/${EMULATION_NAME}/g -e s/"<target_name>"/${OUTPUT_FORMAT}/g \
# <${srcdir}/${TEMPLATE_NAME-ldtemplate} >em_${EMULATION_NAME}.c

# Generate em_${EMULATION_NAME}.c.
. ${srcdir}/emultempl/${TEMPLATE_NAME-generic}.em
