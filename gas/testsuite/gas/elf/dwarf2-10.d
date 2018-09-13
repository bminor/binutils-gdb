#as:
#name: DWARF2 10
#error_output: dwarf2-10.l
# The mep target tries to relay code sections which breaks symbolic view computations.
# The tile targets require 8-byte instructions, but the test only simulates 4-byte instructions.
#notarget: mep-* tile*-*
