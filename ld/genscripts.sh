# This shell script does the work of generating the ld-emulation-target
# specific information from a specific file of paramaters.
# Usage: genscripts.sh srcdir host_alias target_alias emulation_name

srcdir=$1
host_alias=$2
target_alias=$3

# Include the emulation-specic parameters:
. ${srcdir}/$4

# Set the library search path (for libraries named by -lfoo).
# If LIB_PATH is defined (and non-empty), it is used.
# (The format is the usual list of colon-separated directories.)
# (To force a logically empty LIB_PATH, do LIBPATH=":".)
# Otherwise, the default is /lib:/usr/lib:/usr/local/lib,
# unless cross-compiling, in which case the default remains empty.

if [ "x${LIB_PATH}" = "x" -a "x${host_alias}" = "x${target_alias}" ] ; then
   LIB_PATH=/lib:/usr/lib:/usr/local/lib
fi
LIB_SEARCH_DIRS=`echo ${LIB_PATH} | tr ':' ' ' | sed -e 's/\([^ ][^ ]*\)/SEARCH_DIR(\1);/g'`

# This  script generates 5 script files from the master script template
# in ${srcdir}/${SCRIPT_NAME}.sh.  Which one of the 5 script files
# is actually used depends on command line flags given to ld.
# The actual script is filtered through the mkscript program
# to convert it to a form suitable for including in a C program
# as a C string literal.
#
# A .x script file is the default script.
# A .xr script is for linking without relocation (-r flag).
# A .xu script is like .xr, but *do* create constructors (-Ur flag).
# A .xn script is for linking with -n flag (mix text and data on same page).
# A .xN script is for linking with -N flag (mix text and data on same page).

THIS_TEXT_START_ADDR=${TEXT_START_ADDR}
SEGMENT_SIZE=${SEGMENT_SIZE-${PAGE_SIZE}}

DATA_ALIGNMENT="ALIGN(${SEGMENT_SIZE})"
FILTER_SCRIPT=""
(. ${srcdir}/${SCRIPT_NAME}.sc-sh) | sed -e '/^ *$/d' | ./mkscript \
  >${EMULATION_NAME}.xr
CONSTRUCTING=
(. ${srcdir}/${SCRIPT_NAME}.sc-sh) | sed -e '/^ *$/d' | ./mkscript \
  >${EMULATION_NAME}.xu
RELOCATING=
(. ${srcdir}/${SCRIPT_NAME}.sc-sh) | sed -e '/^ *$/d' | ./mkscript \
  >${EMULATION_NAME}.x
THIS_TEXT_START_ADDR=${NONPAGED_TEXT_START_ADDR-${TEXT_START_ADDR}}
(. ${srcdir}/${SCRIPT_NAME}.sc-sh) | sed -e '/^ *$/d' | ./mkscript \
  >${EMULATION_NAME}.xn
DATA_ALIGNMENT="."
(. ${srcdir}/${SCRIPT_NAME}.sc-sh) | sed -e '/^ *$/d' | ./mkscript \
  >${EMULATION_NAME}.xN

#sed -e s/"<ldtarget>"/${EMULATION_NAME}/g -e s/"<arch>"/${ARCH}/g \
# -e s/"<target>"/${EMULATION_NAME}/g -e s/"<target_name>"/${OUTPUT_FORMAT}/g \
# <${srcdir}/${TEMPLATE_NAME-ldtemplate} >ld__${EMULATION_NAME}.c

. ${srcdir}/${TEMPLATE_NAME-generic}.em
