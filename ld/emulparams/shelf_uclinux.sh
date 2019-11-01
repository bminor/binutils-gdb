source_sh ${srcdir}/emulparams/shelf.sh

# We do not want a .stack section
unset STACK_ADDR
GENERATE_SHLIB_SCRIPT=yes
