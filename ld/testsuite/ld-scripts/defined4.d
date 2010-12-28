#ld: -Tdefined4.t
#nm: -B
#source: defined4.s

# Check that arithmetic on DEFINED works.
#...
0+0 D defined
#...
0+0 D defined1
#pass
