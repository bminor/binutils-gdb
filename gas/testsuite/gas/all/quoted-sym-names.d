#nm: --extern-only --numeric-sort
#name: quoted symbol names
# No quoted strings handling (TC_STRING_ESCAPES set to 0):
#notarget: powerpc*-*-aix* powerpc*-*-beos* powerpc-*-macos* rs6000-*-*
# Explicitly no escapes in quoted strings:
#notarget: z80-*-*

#...
0+00 T test-a
0+01 T back\\slash
0+02 T back"slash
0+03 T backslash\\
0+04 T backslash"
