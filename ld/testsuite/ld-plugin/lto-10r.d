#ld: -r tmpdir/lto-10a.o tmpdir/lto-10b.o
#source: dummy.s
#nm: -p

#...
[0-9a-f]+ C __gnu_lto_v.*
#pass
