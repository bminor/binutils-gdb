#ld: --defsym foo=1 tmpdir/start.o tmpdir/libfoo.a
#nm: -B
#source: dummy.s

# Check that --defsym works on archive.
#failif
#...
0.* T bar
#pass
