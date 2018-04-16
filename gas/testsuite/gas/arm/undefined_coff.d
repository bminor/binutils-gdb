#name: Undefined local label error
# COFF and aout based ports, except Windows CE, 
# use a different naming convention for local labels.
#not-skip: *-*-*coff *-unknown-pe *-*-*aout* *-*-netbsd *-*-riscix*
#error-output: undefined_coff.l
