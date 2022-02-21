# Checking that we include RELRO sections in PCC bounds.
#
# Check is done using the fragment values to show what the size of the PCC
# bounds is given as, and we reorder sections so that at least one RELRO
# section is ater all READONLY ALLOC section.
#
# Test only works if we have relro, so is unsupported bare-metal.
#
# Test is implemented by ensuring that the .data.rel.ro is the last section in
# the RELRO segment, and that .data is directly after it.  This is ensured with
# a linker script.  If we don't include RELRO in our PCC bounds then
# .data.rel.ro will not be within the range of the `obj` capability (since the
# .data.rel.ro section itself is marked as being writeable).
#
#source: morello-sec-round-choose-linker-syms.s
#as: -march=morello+c64
#ld: -static -T morello-sec-round-choose-linker-syms.ld
#objdump: --section-headers -j .data.rel.ro

.*:     file format .*
#...
.* \.data\.rel\.ro  00010020  [0-9a-f]+  [0-9a-f]+  [0-9a-f]+  2\*\*5
#pass
