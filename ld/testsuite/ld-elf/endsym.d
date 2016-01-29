#source: start.s
#source: endsym.s
#ld: --sort-common
#nm: -n
#notarget: hppa*-*-hpux*
#xfail: m68hc1*-* xgate-*

#...
.* end
#...
.* end2
#...
.* _?_end
#pass
