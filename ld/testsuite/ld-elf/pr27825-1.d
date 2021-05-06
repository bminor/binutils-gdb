#source: pr27825-1a.s
#source: pr27825-1b.s
#ld: -e _start --emit-relocs -z unique-symbol
#nm: --defined-only
#xfail: [is_generic]
#xfail: fr30-*-* frv-*-* ft32-*-* iq2000-*-* mn10200-*-* msp*-* mt-*-*
# These targets don't support -z.

#...
[0-9a-f]+ t bar.0
#...
[0-9a-f]+ t bar.1
#...
[0-9a-f]+ t bar.1.0
#...
[0-9a-f]+ t bar.1.1
#...
[0-9a-f]+ t bar.2.0
#...
[0-9a-f]+ t bar.2.1
#pass
