# Sed commands to finish translating the opcodes Makefile.in into MPW syntax.

# Empty HDEFINES.
/HDEFINES/s/@HDEFINES@//

/INCDIR=/s/"{srcdir}":/"{topsrcdir}"/
/^CSEARCH = .*$/s/$/ -i "{INCDIR}":mpw: -i ::extra-include:/
/BFD_MACHINES/s/@BFD_MACHINES@/{BFD_MACHINES}/
/archdefs/s/@archdefs@/{ARCHDEFS}/

# Use a ppc-opc.c that has been tweaked to have less macro nesting.
# (really only needed for Metrowerks)
/ppc-opc.c/s/"{s}"ppc-opc\.c/"{o}"hacked_ppc-opc.c/
/ppc-opc.c.o/s/"{o}"ppc-opc\.c\.o/"{o}"hacked_ppc-opc.c.o/
