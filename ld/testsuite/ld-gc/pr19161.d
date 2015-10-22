#name: --gc-sections with __start_SECTIONNAME
#source: dummy.s
#ld: --gc-sections -e main tmpdir/pr19161-1.o tmpdir/pr19161-2.o
#nm: --format=bsd
#xfail: sh64*-*-* iq2000-*-* lm32-*-* epiphany-*-* mips64vr-*-* frv-*-* m32c-*-* rl78-*-* rx-*-* sh-*-* powerpc*-*-eabivle msp430-*-*

#...
0*[1-9a-f]+[0-9a-f]*[ 	](D)[ 	]_*__start_my_section
#...
